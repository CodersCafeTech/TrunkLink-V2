/*
 * TrunkLink V2 - Combined ML Classifier + UART Switching
 *
 * SYSTEM ARCHITECTURE:
 * - ESP32-C6 with limited UARTs
 * - UART1 shared between GPS and LoRa (switched as needed)
 * - ADXL362 accelerometer with Edge Impulse ML classifier
 * - Default: Connected to GPS (reading location)
 * - Transmit: Switch to LoRa, send data, switch back to GPS
 * - ML inference runs continuously on accelerometer data
 *
 * TRANSMISSION BEHAVIOR:
 * - LoRa E5 operates in P2P (Point-to-Point) mode
 * - When "running" detected → Send GPS location immediately via LoRa P2P
 * - When other classes detected → No immediate transmission
 * - Periodic transmission → Send GPS location every 1 minute via LoRa P2P
 *
 * WIRING:
 * ADXL362: SCK=21, MISO=20, MOSI=19, CS=10
 * GPS:     RX=17, TX=16  (ESP32 Pins) - Shared UART1
 * LoRa:    RX=5,  TX=4   (ESP32 Pins) - Shared UART1
 */

#include <TrunkLink_V2_-_Movement_inferencing.h>
#include <ADXL362.h>
#include <SPI.h>
#include <HardwareSerial.h>

/* ==================== PIN CONFIGURATION ==================== */
// ADXL362 SPI
#define SPI_SCK        21
#define SPI_MISO       20
#define SPI_MOSI       19
#define ADXL362_CS     10

// SHARED UART PINS (UART1)
#define GPS_TX_PIN     16  // ESP32 TX -> GPS RX
#define GPS_RX_PIN     17  // ESP32 RX -> GPS TX
#define GPS_BAUD       115200

#define LORA_TX_PIN    4   // ESP32 TX -> LoRa RX
#define LORA_RX_PIN    5   // ESP32 RX -> LoRa TX
#define LORA_BAUD      9600

/* ==================== LORA SETTINGS ==================== */
#define LORA_FREQUENCY     865000000  // 865 MHz (India)
#define LORA_SF            7
#define LORA_BW            0
#define LORA_CR            1
#define LORA_PREAMBLE      8
#define LORA_POWER         14

/* ==================== TIMING ==================== */
#define NORMAL_TX_INTERVAL_MS  60000  // 1 minute (60 seconds) - periodic GPS transmission via LoRa P2P
#define SAMPLE_INTERVAL_MS     EI_CLASSIFIER_INTERVAL_MS  // 10ms (100Hz)
#define WINDOW_SIZE            EI_CLASSIFIER_RAW_SAMPLE_COUNT  // 200 samples
#define NUM_AXES               EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME  // 3 axes

/* ==================== ML SETTINGS ==================== */
#define CONVERT_G_TO_MS2       9.80665f
#define MAX_ACCEPTED_RANGE     2.0f
#define CALIBRATION_FACTOR     0.737f

/* ==================== GPS DATA STRUCTURE ==================== */
struct GPSData {
    bool valid;
    float latitude;
    float longitude;
    char lat_dir;    // 'N' or 'S'
    char lon_dir;    // 'E' or 'W'
    float altitude;
    int satellites;
    char time[11];   // HH:MM:SS format
};

/* ==================== GLOBAL VARIABLES ==================== */
ADXL362 adxl(ADXL362_CS);
HardwareSerial SharedSerial(1); // UART1 - shared between GPS and LoRa

// GPS data
GPSData currentGPS = {false, 0.0, 0.0, 'N', 'E', 0.0, 0, "00:00:00"};

// LoRa state
bool loraConfigured = false;
unsigned long lastNormalTxTime = 0;
bool currentModeIsGPS = true; // Track which device is connected

// ML sampling
static float sample_buffer[WINDOW_SIZE * NUM_AXES];
static volatile size_t sample_index = 0;
static volatile bool buffer_ready = false;
hw_timer_t *sampling_timer = NULL;
static volatile bool sample_flag = false;

// Debug flags
static bool debug_nn = false;
static bool debug_samples = false;

/* ==================== FORWARD DECLARATIONS ==================== */
void IRAM_ATTR onSamplingTimer();
void enableGPS();
void enableLoRa();
bool initLoRa();
void updateGPS();
bool parseNMEA(String nmea);
float nmeaToDecimal(float coord, char direction);
void waitForGPSFix();
String sendAtCommand(String cmd, int timeout);
void transmitData(const char* reason);
void collect_sample();
void run_inference();
float constrain_value(float value, float min_val, float max_val);

/* ==================== UART SWITCHING ==================== */

void enableGPS() {
    if (currentModeIsGPS) return; // Already on GPS
    
    SharedSerial.end();
    delay(50);
    SharedSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    currentModeIsGPS = true;
    Serial.println("[UART] Switched to GPS");
}

void enableLoRa() {
    SharedSerial.end();
    delay(50);
    SharedSerial.begin(LORA_BAUD, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);
    currentModeIsGPS = false;
    delay(100); // Give UART time to settle
    Serial.println("[UART] Switched to LoRa");
}

/* ==================== TIMER INTERRUPT ==================== */

void IRAM_ATTR onSamplingTimer() {
    sample_flag = true;
}

/* ==================== SETUP ==================== */

void setup() {
    delay(3000);
    Serial.begin(115200);
    while (!Serial) delay(10);

    Serial.println("\n=============================================");
    Serial.println("  TrunkLink V2 - ML + UART Switching");
    Serial.println("=============================================\n");

    // Print model information
    Serial.println("Edge Impulse Model:");
    Serial.printf("- Project: %s\n", EI_CLASSIFIER_PROJECT_NAME);
    Serial.printf("- Classes: ");
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.print(ei_classifier_inferencing_categories[i]);
        if (i < EI_CLASSIFIER_LABEL_COUNT - 1) Serial.print(", ");
    }
    Serial.println();
    Serial.printf("- Sampling: %d Hz, Window: %d samples (%.1fs)\n\n",
                  EI_CLASSIFIER_FREQUENCY, WINDOW_SIZE,
                  (float)WINDOW_SIZE / EI_CLASSIFIER_FREQUENCY);

    // 1. Initialize SPI & ADXL362
    Serial.println("[INIT] Initializing ADXL362...");
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, ADXL362_CS);
    
    short status = adxl.init();
    if (status <= 0) {
        Serial.println("[ERROR] ADXL362 init failed!");
        while (1) delay(1000);
    }
    
    delay(100);
    adxl.activateMeasure(ad_bandwidth_hz_100_lowpower, ad_range_2G, ad_noise_normal);
    Serial.printf("[ADXL] Ready (Rev: 0x%02X)\n", adxl.getRevisionId());

    // 2. Configure LoRa (switch to LoRa mode temporarily)
    Serial.println("\n[INIT] Configuring LoRa...");
    enableLoRa();
    if (initLoRa()) {
        loraConfigured = true;
        Serial.println("[LoRa] Configuration successful!");
    } else {
        Serial.println("[ERROR] LoRa configuration failed!");
    }
    
    // 3. Switch to GPS and wait for fix
    Serial.println("\n[INIT] Switching to GPS...");
    enableGPS();
    waitForGPSFix();
    
    // 4. Setup ML sampling timer (100Hz)
    Serial.println("\n[INIT] Starting ML inference...");
    memset(sample_buffer, 0, sizeof(sample_buffer));
    sample_index = 0;
    
    sampling_timer = timerBegin(1000000);
    if (sampling_timer == NULL) {
        Serial.println("[ERROR] Failed to initialize timer!");
        while (1) delay(1000);
    }
    
    timerAttachInterrupt(sampling_timer, &onSamplingTimer);
    timerAlarm(sampling_timer, 10000, true, 0); // 10ms = 100Hz
    
    Serial.println("\n=============================================");
    Serial.println("       System Ready!");
    Serial.println("=============================================\n");
    
    lastNormalTxTime = millis();
}

/* ==================== MAIN LOOP ==================== */

void loop() {
    unsigned long now = millis();
    
    // 1. UPDATE GPS (only when in GPS mode)
    if (currentModeIsGPS) {
        updateGPS();
    }
    
    // 2. COLLECT ACCELEROMETER SAMPLES
    if (sample_flag) {
        sample_flag = false;
        collect_sample();
        
        // Run inference when buffer is full
        if (buffer_ready) {
            buffer_ready = false;
            run_inference();
        }
    }
    
    // 3. PERIODIC LORA TRANSMISSION (every 1 minute = 60 seconds)
    // This sends GPS location every minute via LoRa P2P, regardless of detected activity
    if (currentGPS.valid && loraConfigured &&
        (now - lastNormalTxTime >= NORMAL_TX_INTERVAL_MS)) {
        transmitData("PERIODIC");
        lastNormalTxTime = now;
    }
}

/* ==================== GPS FUNCTIONS ==================== */

void updateGPS() {
    while (SharedSerial.available()) {
        String line = SharedSerial.readStringUntil('\n');
        line.trim();
        
        if (line.startsWith("$GPRMC") || line.startsWith("$GNRMC") ||
            line.startsWith("$GPGGA") || line.startsWith("$GNGGA")) {
            parseNMEA(line);
        }
    }
}

bool parseNMEA(String nmea) {
    int commaPos[20];
    int commaCount = 0;
    
    // Find all comma positions
    for (int i = 0; i < nmea.length() && commaCount < 20; i++) {
        if (nmea.charAt(i) == ',') {
            commaPos[commaCount++] = i;
        }
    }
    
    if (commaCount < 6) return false;
    
    // Parse GPRMC/GNRMC
    if (nmea.startsWith("$GPRMC") || nmea.startsWith("$GNRMC")) {
        String status = nmea.substring(commaPos[1] + 1, commaPos[2]);
        
        if (status == "A") {  // Valid fix
            // Parse time
            String timeStr = nmea.substring(commaPos[0] + 1, commaPos[1]);
            if (timeStr.length() >= 6) {
                snprintf(currentGPS.time, sizeof(currentGPS.time), "%c%c:%c%c:%c%c",
                        timeStr[0], timeStr[1], timeStr[2], timeStr[3], timeStr[4], timeStr[5]);
            }
            
            // Parse coordinates
            String latStr = nmea.substring(commaPos[2] + 1, commaPos[3]);
            String latDir = nmea.substring(commaPos[3] + 1, commaPos[4]);
            String lonStr = nmea.substring(commaPos[4] + 1, commaPos[5]);
            String lonDir = nmea.substring(commaPos[5] + 1, commaPos[6]);
            
            if (latStr.length() > 0 && lonStr.length() > 0) {
                currentGPS.latitude = nmeaToDecimal(latStr.toFloat(), latDir.charAt(0));
                currentGPS.longitude = nmeaToDecimal(lonStr.toFloat(), lonDir.charAt(0));
                currentGPS.lat_dir = latDir.charAt(0);
                currentGPS.lon_dir = lonDir.charAt(0);
                currentGPS.valid = true;
                return true;
            }
        }
    }
    
    // Parse GPGGA/GNGGA
    else if (nmea.startsWith("$GPGGA") || nmea.startsWith("$GNGGA")) {
        String fixQuality = nmea.substring(commaPos[5] + 1, commaPos[6]);
        
        if (fixQuality.toInt() > 0) {
            // Parse satellites
            String satsStr = nmea.substring(commaPos[6] + 1, commaPos[7]);
            currentGPS.satellites = satsStr.toInt();
            
            // Parse altitude
            if (commaCount > 9) {
                String altStr = nmea.substring(commaPos[8] + 1, commaPos[9]);
                currentGPS.altitude = altStr.toFloat();
            }
        }
    }
    
    return false;
}

float nmeaToDecimal(float coord, char direction) {
    int degrees = (int)(coord / 100);
    float minutes = coord - (degrees * 100);
    float decimal = degrees + (minutes / 60.0);
    
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}

void waitForGPSFix() {
    Serial.println("\n========================================");
    Serial.println("   Waiting for GPS Fix...");
    Serial.println("========================================");
    Serial.println("Ensure GPS has clear sky view");
    Serial.println("This may take 30-60 seconds\n");
    
    unsigned long lastDot = millis();
    int dotCount = 0;
    
    while (!currentGPS.valid) {
        updateGPS();
        
        if (millis() - lastDot >= 1000) {
            lastDot = millis();
            Serial.print(".");
            dotCount++;
            if (dotCount >= 50) {
                Serial.println();
                dotCount = 0;
            }
        }
        
        delay(100);
    }
    
    Serial.println("\n\n========================================");
    Serial.println("   GPS FIX OBTAINED!");
    Serial.println("========================================");
    Serial.printf("Latitude:  %.6f %c\n", abs(currentGPS.latitude), currentGPS.lat_dir);
    Serial.printf("Longitude: %.6f %c\n", abs(currentGPS.longitude), currentGPS.lon_dir);
    Serial.printf("Altitude:  %.1f m\n", currentGPS.altitude);
    Serial.printf("Satellites: %d\n", currentGPS.satellites);
    Serial.printf("Time:      %s UTC\n", currentGPS.time);
    Serial.println("----------------------------------------");
    Serial.printf("Maps: https://www.google.com/maps?q=%.6f,%.6f\n",
                 currentGPS.latitude, currentGPS.longitude);
    Serial.println("========================================\n");
}

/* ==================== LORA FUNCTIONS ==================== */

String sendAtCommand(String cmd, int timeout) {
    while(SharedSerial.available()) SharedSerial.read(); // Clear buffer
    SharedSerial.println(cmd);
    Serial.printf("[LoRa TX] %s\n", cmd.c_str());
    
    String response = "";
    unsigned long start = millis();
    
    while (millis() - start < timeout) {
        if (SharedSerial.available()) {
            response += (char)SharedSerial.read();
        }
    }
    
    if (response.length() > 0) {
        Serial.printf("[LoRa RX] %s\n", response.c_str());
    }
    
    return response;
}

bool initLoRa() {
    // Must be called when enableLoRa() has been called
    String resp = sendAtCommand("AT", 2000);
    if (resp.indexOf("OK") < 0) {
        Serial.println("[LoRa] No response to AT");
        return false;
    }
    
    sendAtCommand("AT+MODE=TEST", 1000);
    
    String cfg = "AT+TEST=RFCFG," + String(LORA_FREQUENCY) + "," +
                 String(LORA_SF) + "," + String(LORA_BW) + "," +
                 String(LORA_CR) + "," + String(LORA_PREAMBLE) + "," +
                 String(LORA_POWER);
    
    sendAtCommand(cfg, 1000);
    
    Serial.println("[LoRa] Configuration:");
    Serial.printf("  Freq: %d Hz (%.1f MHz)\n", LORA_FREQUENCY, LORA_FREQUENCY / 1000000.0);
    Serial.printf("  SF: %d, BW: %s, CR: 4/%d\n", LORA_SF, 
                  LORA_BW == 0 ? "125kHz" : (LORA_BW == 1 ? "250kHz" : "500kHz"),
                  LORA_CR + 4);
    Serial.printf("  Power: %d dBm\n", LORA_POWER);
    
    return true;
}

void transmitData(const char* reason) {
    if (!currentGPS.valid || !loraConfigured) {
        Serial.println("[TX] Skipping (GPS invalid or LoRa not configured)");
        return;
    }
    
    Serial.printf("\n[TX] ========== TRANSMITTING (%s) ==========\n", reason);
    
    // 1. SWITCH TO LORA
    enableLoRa();
    delay(100);
    
    // 2. FORMAT PACKET
    // Format: LAT,LON,ALT,SATS,TIME
    String packet = String(currentGPS.latitude, 6) + "," +
                    String(currentGPS.longitude, 6) + "," +
                    String(currentGPS.altitude, 1) + "," +
                    String(currentGPS.satellites) + "," +
                    String(currentGPS.time);
    
    Serial.printf("[TX] Data: %s\n", packet.c_str());
    Serial.printf("[TX] Size: %d bytes\n", packet.length());
    
    // 3. CONVERT TO HEX
    String hex = "";
    for (int i = 0; i < packet.length(); i++) {
        char buf[3];
        sprintf(buf, "%02X", (unsigned char)packet[i]);
        hex += buf;
    }
    
    // 4. TRANSMIT
    String cmd = "AT+TEST=TXLRPKT,\"" + hex + "\"";
    String resp = sendAtCommand(cmd, 3000);
    
    if (resp.indexOf("TX DONE") >= 0) {
        Serial.println("[TX] ✓ SUCCESS!");
    } else {
        Serial.println("[TX] ✗ FAILED or UNKNOWN");
    }
    
    // 5. SWITCH BACK TO GPS
    enableGPS();
    Serial.println("[TX] ========== Resumed GPS ==========\n");
}

/* ==================== ML FUNCTIONS ==================== */

void collect_sample() {
    MeasurementInMg m = adxl.getXYZLowPower(ad_range_2G);
    
    // Convert to g and apply calibration
    float x_g = ((float)m.x / 1000.0f) * CALIBRATION_FACTOR;
    float y_g = ((float)m.y / 1000.0f) * CALIBRATION_FACTOR;
    float z_g = ((float)m.z / 1000.0f) * CALIBRATION_FACTOR;
    
    // Constrain to ±2g
    x_g = constrain_value(x_g, -MAX_ACCEPTED_RANGE, MAX_ACCEPTED_RANGE);
    y_g = constrain_value(y_g, -MAX_ACCEPTED_RANGE, MAX_ACCEPTED_RANGE);
    z_g = constrain_value(z_g, -MAX_ACCEPTED_RANGE, MAX_ACCEPTED_RANGE);
    
    // Convert to m/s²
    float x_ms2 = x_g * CONVERT_G_TO_MS2;
    float y_ms2 = y_g * CONVERT_G_TO_MS2;
    float z_ms2 = z_g * CONVERT_G_TO_MS2;
    
    // Store in circular buffer
    size_t buf_idx = sample_index * NUM_AXES;
    sample_buffer[buf_idx + 0] = x_ms2;
    sample_buffer[buf_idx + 1] = y_ms2;
    sample_buffer[buf_idx + 2] = z_ms2;
    
    sample_index++;
    
    if (sample_index >= WINDOW_SIZE) {
        sample_index = 0;
        buffer_ready = true;
    }
}

void run_inference() {
    ei_impulse_result_t result = { 0 };
    
    // Optional: Print sample statistics
    if (debug_samples) {
        float sum_mag = 0;
        for (size_t i = 0; i < WINDOW_SIZE; i++) {
            float x = sample_buffer[i * 3 + 0];
            float y = sample_buffer[i * 3 + 1];
            float z = sample_buffer[i * 3 + 2];
            sum_mag += sqrt(x*x + y*y + z*z);
        }
        Serial.printf("[ML] Avg Magnitude: %.3f m/s²\n", sum_mag / WINDOW_SIZE);
    }
    
    // Create signal
    signal_t signal;
    int res = numpy::signal_from_buffer(sample_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (res != 0) {
        Serial.printf("[ML ERROR] Signal creation failed (%d)\n", res);
        return;
    }
    
    // Run classifier
    res = run_classifier(&signal, &result, debug_nn);
    if (res != 0) {
        Serial.printf("[ML ERROR] Classifier failed (%d)\n", res);
        return;
    }
    
    // Print results
    Serial.println("\n========================================");
    Serial.printf("[ML] Inference Time: %dms (DSP=%dms, NN=%dms)\n",
                  result.timing.dsp + result.timing.classification,
                  result.timing.dsp, result.timing.classification);
    Serial.println("----------------------------------------");
    
    // Find highest confidence
    float max_confidence = 0.0f;
    const char* predicted_class = "";
    
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        float conf = result.classification[i].value;
        Serial.printf("  %-12s: %5.1f%% ", result.classification[i].label, conf * 100.0f);
        
        // Print bar
        int bar_len = (int)(conf * 30);
        Serial.print("[");
        for (int j = 0; j < 30; j++) {
            Serial.print(j < bar_len ? "=" : " ");
        }
        Serial.println("]");
        
        if (conf > max_confidence) {
            max_confidence = conf;
            predicted_class = result.classification[i].label;
        }
    }
    
    Serial.println("----------------------------------------");

    if (max_confidence > EI_CLASSIFIER_THRESHOLD) {
        Serial.printf("✓ DETECTED: %s (%.1f%%)\n", predicted_class, max_confidence * 100.0f);

        // TRANSMISSION LOGIC:
        // - "running" detected → Send GPS immediately via LoRa P2P
        // - Other classes → Do nothing (no immediate transmission)
        // - Periodic transmission every 1 minute happens independently (see main loop)

        if (strcmp(predicted_class, "running") == 0) {
            // RUNNING DETECTED - Send location via LoRa P2P
            Serial.println("\n*** RUNNING DETECTED! ***");
            if (currentGPS.valid) {
                Serial.printf("Location: %.6f, %.6f\n", currentGPS.latitude, currentGPS.longitude);
                Serial.printf("Altitude: %.1f m, Sats: %d\n", currentGPS.altitude, currentGPS.satellites);

                // Transmit via LoRa P2P (with 5-second cooldown to prevent spam)
                if (loraConfigured && (millis() - lastNormalTxTime > 5000)) {
                    transmitData("RUNNING");
                    lastNormalTxTime = millis(); // Prevent duplicate transmissions
                }
            } else {
                Serial.println("GPS not valid - cannot send location");
            }
        } else {
            // OTHER CLASS DETECTED - Do nothing (no immediate transmission)
            Serial.printf("Class '%s' detected - no immediate action\n", predicted_class);
        }
    } else {
        Serial.printf("? UNCERTAIN (max: %.1f%% < %.1f%% threshold)\n",
                     max_confidence * 100.0f, EI_CLASSIFIER_THRESHOLD * 100.0f);
    }
    
    Serial.println("========================================\n");
}

float constrain_value(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}