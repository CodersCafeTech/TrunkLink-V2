/*
 * TrunkLink V2 - Unified Gateway System with 4G + Firebase
 *
 * SYSTEM ARCHITECTURE:
 * - PIR Motion Detection: Triggers camera-based inference
 * - Edge Impulse FOMO: Object detection for wildlife/human detection
 * - LoRa E5 Receiver: Receives GPS data from collar
 * - A7670C 4G Modem: Sends collar data to Firebase Realtime Database
 *
 * OPERATION MODES:
 * 1. IDLE: Listening for LoRa packets
 * 2. MOTION_DETECTED: PIR triggered, ready for inference
 * 3. INFERENCING: Running Edge Impulse model
 * 4. PROCESSING_LORA: Decoding received LoRa data & sending to Firebase
 *
 * WIRING (XIAO ESP32S3 Sense):
 * - LoRa E5: RX=44, TX=43
 * - A7670C 4G: RX=1, TX=2
 * - PIR Sensor: Signal=3, VCC=3.3V, GND=GND
 * - Camera: Built-in OV2640
 * - LED: D21 (built-in)
 *
 * AUTHOR: Coders Cafe R&D
 * PROJECT: TrunkLink V2 Anti-Poaching System with Firebase
 */

/* ==================== INCLUDES ==================== */
#include <TrunkLink_V2_-_Perimeter_Vision_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"
#include <HardwareSerial.h>

/* ==================== PIN CONFIGURATION ==================== */
// LoRa E5
#define LORA_TX_PIN         43
#define LORA_RX_PIN         44
#define LORA_BAUD           9600

// A7670C 4G Modem
#define AT_TX_PIN           2
#define AT_RX_PIN           1
#define AT_BAUD             115200

// PIR Sensor
#define PIR_PIN             3

// Camera (XIAO ESP32S3)
#define PWDN_GPIO_NUM       -1
#define RESET_GPIO_NUM      -1
#define XCLK_GPIO_NUM       10
#define SIOD_GPIO_NUM       40
#define SIOC_GPIO_NUM       39
#define Y9_GPIO_NUM         48
#define Y8_GPIO_NUM         11
#define Y7_GPIO_NUM         12
#define Y6_GPIO_NUM         14
#define Y5_GPIO_NUM         16
#define Y4_GPIO_NUM         18
#define Y3_GPIO_NUM         17
#define Y2_GPIO_NUM         15
#define VSYNC_GPIO_NUM      38
#define HREF_GPIO_NUM       47
#define PCLK_GPIO_NUM       13
#define LED_GPIO_NUM        21

/* ==================== LORA SETTINGS ==================== */
#define LORA_FREQUENCY      865000000  // 865 MHz (India)
#define LORA_SF             7
#define LORA_BW             0          // 0=125kHz
#define LORA_CR             1          // 1=4/5
#define LORA_PREAMBLE       8
#define LORA_POWER          14

/* ==================== FIREBASE SETTINGS ==================== */
// Replace with your Firebase Realtime Database URL
#define FIREBASE_DATABASE_URL "your-project.firebaseio.com"
#define FIREBASE_AUTH_TOKEN  "your-auth-token"  // Can be obtained from Firebase Console

/* ==================== EDGE IMPULSE SETTINGS ==================== */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS     320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS     240
#define EI_CAMERA_FRAME_BYTE_SIZE           3

/* ==================== DETECTION THRESHOLDS ==================== */
#define DETECTION_CONFIDENCE_THRESHOLD      0.6   // Minimum confidence for detection
#define PIR_DEBOUNCE_MS                     2000  // PIR debounce time
#define INFERENCE_INTERVAL_MS               5000  // Min time between inferences

/* ==================== SYSTEM STATES ==================== */
enum SystemState {
    STATE_IDLE,
    STATE_MOTION_DETECTED,
    STATE_INFERENCING,
    STATE_PROCESSING_LORA
};

/* ==================== DATA STRUCTURES ==================== */
struct ReceivedGPSData {
    float latitude;
    float longitude;
    float altitude;
    int satellites;
    char time[11];
    int rssi;
    float snr;
};

struct DetectionEvent {
    char label[32];
    float confidence;
    uint32_t x, y, width, height;
    unsigned long timestamp;
};

/* ==================== GLOBAL VARIABLES ==================== */
// System state
SystemState currentState = STATE_IDLE;
volatile bool pirTriggered = false;
unsigned long lastPirTrigger = 0;
unsigned long lastInference = 0;

// LoRa
HardwareSerial LoRaSerial(1);
ReceivedGPSData lastReceived = {0.0, 0.0, 0.0, 0, "00:00:00", 0, 0.0};
unsigned long lastPacketTime = 0;
unsigned long packetCount = 0;

// 4G Modem
HardwareSerial SerialAT(2);
bool modem4GInitialized = false;
bool networkRegistered = false;

// Edge Impulse
static bool debug_nn = false;
static bool is_camera_initialised = false;
uint8_t *snapshot_buf = nullptr;

// Detection tracking
DetectionEvent lastDetection = {"", 0.0, 0, 0, 0, 0, 0};
unsigned long detectionCount = 0;

/* ==================== CAMERA CONFIG ==================== */
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count = 1,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/* ==================== FORWARD DECLARATIONS ==================== */
// LoRa functions
String sendAtCommand(String cmd, int timeout);
String sendAtCommand4G(String cmd, int timeout);
bool initLoRa();
bool init4GModem();
void receiveLoop();
bool decodePacket(String hexData, int rssi, float snr);
void displayReceivedData();

// 4G & Firebase functions
bool waitForNetworkRegistration();
bool sendDataToFirebase(const char* jsonData);

// Camera functions
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

// Inference functions
void runInference();
void processDetections(ei_impulse_result_t &result);

// PIR functions
void IRAM_ATTR pirISR();
void handlePirEvent();

// System functions
void updateState(SystemState newState);
void blinkLED(int times, int delayMs);

/* ==================== SETUP ==================== */
void setup() {
    delay(2000);
    Serial.begin(115200);
    while (!Serial) delay(10);

    Serial.println("\n=============================================");
    Serial.println("  TrunkLink V2 - Gateway with 4G + Firebase");
    Serial.println("  PIR + Edge Impulse + LoRa + 4G Modem");
    Serial.println("=============================================\n");

    // Initialize LED
    pinMode(LED_GPIO_NUM, OUTPUT);
    digitalWrite(LED_GPIO_NUM, LOW);
    blinkLED(3, 200);

    // Initialize PIR
    Serial.println("[INIT] Setting up PIR sensor...");
    pinMode(PIR_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirISR, RISING);
    Serial.println("[PIR] Initialized on pin D0");

    // Initialize Camera
    Serial.println("[INIT] Initializing camera...");
    if (ei_camera_init() == false) {
        Serial.println("[ERROR] Camera initialization failed!");
        while (1) {
            blinkLED(5, 100);
            delay(1000);
        }
    }
    Serial.println("[CAMERA] OV2640 initialized");

    // Initialize 4G Modem (UART2)
    Serial.println("[INIT] Initializing A7670C 4G Modem...");
    SerialAT.begin(AT_BAUD, SERIAL_8N1, AT_RX_PIN, AT_TX_PIN);
    delay(3000);

    if (init4GModem()) {
        Serial.println("[4G] Modem initialized successfully!");
        if (waitForNetworkRegistration()) {
            networkRegistered = true;
            Serial.println("[4G] Network registered!");
        } else {
            Serial.println("[4G] Failed to register on 4G network");
        }
    } else {
        Serial.println("[ERROR] 4G Modem initialization failed!");
    }

    // Initialize LoRa (UART1)
    Serial.println("[INIT] Initializing LoRa E5...");
    LoRaSerial.begin(LORA_BAUD, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);
    delay(500);

    if (initLoRa()) {
        Serial.println("[LORA] Configuration successful!");
    } else {
        Serial.println("[ERROR] LoRa configuration failed!");
        Serial.println("[WARNING] Continuing without LoRa...");
    }

    Serial.println("\n=============================================");
    Serial.println("    System Ready - Entering IDLE Mode");
    Serial.println("=============================================\n");

    updateState(STATE_IDLE);
    blinkLED(2, 500);
}

/* ==================== MAIN LOOP ==================== */
void loop() {
    // Handle PIR trigger
    if (pirTriggered) {
        handlePirEvent();
    }

    // State machine
    switch (currentState) {
        case STATE_IDLE:
            // Listen for LoRa packets
            receiveLoop();
            break;

        case STATE_MOTION_DETECTED:
            // Check if enough time has passed since last inference
            if (millis() - lastInference >= INFERENCE_INTERVAL_MS) {
                updateState(STATE_INFERENCING);
            } else {
                // Too soon, return to idle
                updateState(STATE_IDLE);
            }
            break;

        case STATE_INFERENCING:
            runInference();
            updateState(STATE_IDLE);
            break;

        case STATE_PROCESSING_LORA:
            // Processing happens in receiveLoop
            updateState(STATE_IDLE);
            break;
    }

    delay(10);
}

/* ==================== PIR FUNCTIONS ==================== */
void IRAM_ATTR pirISR() {
    unsigned long now = millis();
    if (now - lastPirTrigger > PIR_DEBOUNCE_MS) {
        pirTriggered = true;
        lastPirTrigger = now;
    }
}

void handlePirEvent() {
    pirTriggered = false;

    Serial.println("\n[PIR] Motion detected!");
    digitalWrite(LED_GPIO_NUM, HIGH);

    updateState(STATE_MOTION_DETECTED);

    delay(100);
    digitalWrite(LED_GPIO_NUM, LOW);
}

/* ==================== INFERENCE FUNCTIONS ==================== */
void runInference() {
    Serial.println("\n========================================");
    Serial.println("    RUNNING INFERENCE");
    Serial.println("========================================");

    digitalWrite(LED_GPIO_NUM, HIGH);
    lastInference = millis();

    // Allocate snapshot buffer
    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS *
                                    EI_CAMERA_RAW_FRAME_BUFFER_ROWS *
                                    EI_CAMERA_FRAME_BYTE_SIZE);

    if (snapshot_buf == nullptr) {
        Serial.println("[ERROR] Failed to allocate snapshot buffer!");
        digitalWrite(LED_GPIO_NUM, LOW);
        return;
    }

    // Setup signal
    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    // Capture image
    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH,
                         (size_t)EI_CLASSIFIER_INPUT_HEIGHT,
                         snapshot_buf) == false) {
        Serial.println("[ERROR] Failed to capture image!");
        free(snapshot_buf);
        digitalWrite(LED_GPIO_NUM, LOW);
        return;
    }

    // Run classifier
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);

    if (err != EI_IMPULSE_OK) {
        Serial.printf("[ERROR] Failed to run classifier (%d)\n", err);
        free(snapshot_buf);
        digitalWrite(LED_GPIO_NUM, LOW);
        return;
    }

    // Print timing
    Serial.printf("Timing: DSP=%dms, Classification=%dms, Anomaly=%dms\n",
                  result.timing.dsp,
                  result.timing.classification,
                  result.timing.anomaly);

    // Process detections
    processDetections(result);

    // Cleanup
    free(snapshot_buf);
    snapshot_buf = nullptr;
    digitalWrite(LED_GPIO_NUM, LOW);
}

void processDetections(ei_impulse_result_t &result) {
    bool detectionFound = false;

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    Serial.println("\n--- Object Detections ---");

    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];

        if (bb.value >= DETECTION_CONFIDENCE_THRESHOLD) {
            detectionFound = true;
            detectionCount++;

            Serial.println("========================================");
            Serial.printf("DETECTION #%lu\n", detectionCount);
            Serial.println("========================================");
            Serial.printf("Label:      %s\n", bb.label);
            Serial.printf("Confidence: %.2f%%\n", bb.value * 100);
            Serial.printf("Bounding Box:\n");
            Serial.printf("  X: %u, Y: %u\n", bb.x, bb.y);
            Serial.printf("  Width: %u, Height: %u\n", bb.width, bb.height);
            Serial.println("========================================\n");

            // Store last detection
            strncpy(lastDetection.label, bb.label, sizeof(lastDetection.label) - 1);
            lastDetection.confidence = bb.value;
            lastDetection.x = bb.x;
            lastDetection.y = bb.y;
            lastDetection.width = bb.width;
            lastDetection.height = bb.height;
            lastDetection.timestamp = millis();

            // Alert behavior for critical detections
            if (strcmp(bb.label, "human") == 0 || strcmp(bb.label, "person") == 0) {
                Serial.println("!!! ALERT: HUMAN DETECTED !!!");
                blinkLED(10, 100);
            }
        }
    }

    if (!detectionFound) {
        Serial.println("No objects detected above threshold");
    }

#else
    // Classification mode
    Serial.println("\n--- Classification Results ---");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        float confidence = result.classification[i].value;
        if (confidence >= DETECTION_CONFIDENCE_THRESHOLD) {
            Serial.printf("  %s: %.2f%%\n",
                         ei_classifier_inferencing_categories[i],
                         confidence * 100);
            detectionFound = true;
        }
    }
#endif

#if EI_CLASSIFIER_HAS_ANOMALY
    Serial.printf("Anomaly: %.3f\n", result.anomaly);
#endif
}

/* ==================== LORA FUNCTIONS ==================== */
String sendAtCommand(String cmd, int timeout) {
    while(LoRaSerial.available()) LoRaSerial.read();
    LoRaSerial.println(cmd);

    String response = "";
    unsigned long start = millis();

    while (millis() - start < timeout) {
        if (LoRaSerial.available()) {
            response += (char)LoRaSerial.read();
        }
    }

    return response;
}

bool initLoRa() {
    String resp = sendAtCommand("AT", 2000);
    if (resp.indexOf("OK") < 0) {
        return false;
    }

    sendAtCommand("AT+MODE=TEST", 1000);
    delay(100);

    String cfg = "AT+TEST=RFCFG," + String(LORA_FREQUENCY) + "," +
                 String(LORA_SF) + "," + String(LORA_BW) + "," +
                 String(LORA_CR) + "," + String(LORA_PREAMBLE) + "," +
                 String(LORA_POWER) + ",ON,OFF,OFF";

    resp = sendAtCommand(cfg, 1000);

    Serial.println("[LoRa] Configuration:");
    Serial.printf("  Frequency: %.1f MHz\n", LORA_FREQUENCY / 1000000.0);
    Serial.printf("  SF: %d, BW: 125kHz, CR: 4/5\n", LORA_SF);
    Serial.printf("  Power: %d dBm\n", LORA_POWER);

    return true;
}

void receiveLoop() {
    static unsigned long lastRxStart = 0;
    static bool rxStarted = false;

    // Restart RX mode periodically
    if (!rxStarted || (millis() - lastRxStart > 30000)) {
        sendAtCommand("AT+TEST=RXLRPKT", 1000);
        rxStarted = true;
        lastRxStart = millis();
    }

    // Check for incoming data
    if (LoRaSerial.available()) {
        String response = "";

        while (LoRaSerial.available()) {
            char c = LoRaSerial.read();
            response += c;
            delay(2);
        }

        if (response.indexOf("+TEST: RX") >= 0) {
            updateState(STATE_PROCESSING_LORA);

            Serial.println("\n========================================");
            Serial.println("   LORA PACKET RECEIVED");
            Serial.println("========================================");

            int startQuote = response.indexOf('"');
            int endQuote = response.lastIndexOf('"');

            if (startQuote >= 0 && endQuote > startQuote) {
                String hexData = response.substring(startQuote + 1, endQuote);

                int rssi = 0;
                float snr = 0.0;

                int rssiIdx = response.indexOf("RSSI");
                if (rssiIdx >= 0) {
                    String rssiStr = response.substring(rssiIdx + 4);
                    rssiStr.trim();
                    rssi = rssiStr.toInt();
                }

                int snrIdx = response.indexOf("SNR");
                if (snrIdx >= 0) {
                    String snrStr = response.substring(snrIdx + 3);
                    snrStr.trim();
                    int semiIdx = snrStr.indexOf(';');
                    if (semiIdx > 0) snrStr = snrStr.substring(0, semiIdx);
                    snr = snrStr.toFloat();
                }

                if (decodePacket(hexData, rssi, snr)) {
                    packetCount++;
                    lastPacketTime = millis();
                    displayReceivedData();

                    // Send to Firebase if network is available
                    if (networkRegistered) {
                        sendGPSDataToFirebase();
                    }
                }
            }

            delay(100);
            sendAtCommand("AT+TEST=RXLRPKT", 1000);
            lastRxStart = millis();
        }
    }
}

bool decodePacket(String hexData, int rssi, float snr) {
    // Convert hex to ASCII
    String decoded = "";
    for (int i = 0; i < hexData.length(); i += 2) {
        if (i + 1 < hexData.length()) {
            String byteStr = hexData.substring(i, i + 2);
            char byte = (char)strtol(byteStr.c_str(), NULL, 16);
            decoded += byte;
        }
    }

    // Parse CSV: LAT,LON,ALT,SATS,TIME
    int commaPos[4];
    int commaCount = 0;

    for (int i = 0; i < decoded.length() && commaCount < 4; i++) {
        if (decoded.charAt(i) == ',') {
            commaPos[commaCount++] = i;
        }
    }

    if (commaCount != 4) {
        return false;
    }

    String latStr = decoded.substring(0, commaPos[0]);
    String lonStr = decoded.substring(commaPos[0] + 1, commaPos[1]);
    String altStr = decoded.substring(commaPos[1] + 1, commaPos[2]);
    String satsStr = decoded.substring(commaPos[2] + 1, commaPos[3]);
    String timeStr = decoded.substring(commaPos[3] + 1);

    lastReceived.latitude = latStr.toFloat();
    lastReceived.longitude = lonStr.toFloat();
    lastReceived.altitude = altStr.toFloat();
    lastReceived.satellites = satsStr.toInt();
    strncpy(lastReceived.time, timeStr.c_str(), sizeof(lastReceived.time) - 1);
    lastReceived.rssi = rssi;
    lastReceived.snr = snr;

    return true;
}

void displayReceivedData() {
    Serial.println("========================================");
    Serial.println("      GPS DATA FROM COLLAR");
    Serial.println("========================================");
    Serial.printf("Packet #:   %lu\n", packetCount);
    Serial.printf("Time (UTC): %s\n", lastReceived.time);
    Serial.println("----------------------------------------");
    Serial.printf("Latitude:   %.6f%c\n", abs(lastReceived.latitude),
                  lastReceived.latitude >= 0 ? 'N' : 'S');
    Serial.printf("Longitude:  %.6f%c\n", abs(lastReceived.longitude),
                  lastReceived.longitude >= 0 ? 'E' : 'W');
    Serial.printf("Altitude:   %.1f m\n", lastReceived.altitude);
    Serial.printf("Satellites: %d\n", lastReceived.satellites);
    Serial.println("----------------------------------------");
    Serial.printf("RSSI:       %d dBm\n", lastReceived.rssi);
    Serial.printf("SNR:        %.1f dB\n", lastReceived.snr);
    Serial.println("----------------------------------------");
    Serial.printf("Maps Link:\n");
    Serial.printf("https://maps.google.com?q=%.6f,%.6f\n",
                 lastReceived.latitude, lastReceived.longitude);
    Serial.println("========================================\n");
}

/* ==================== 4G MODEM FUNCTIONS ==================== */
String sendAtCommand4G(String command, const char *expected = "OK", uint16_t timeout = 2000) {
    Serial.println("[4G TX] " + command);
    SerialAT.println(command);
    unsigned long timeStart = millis();
    String response = "";
    while (millis() - timeStart < timeout) {
        while (SerialAT.available()) {
            char c = SerialAT.read();
            response += c;
        }
        if (response.indexOf(expected) != -1) break;
    }
    Serial.println("[4G RX] " + response);
    return response;
}

bool init4GModem() {
    // Check module
    String resp = sendAtCommand4G("AT");
    if (resp.indexOf("OK") == -1) {
        return false;
    }

    // Check SIM card
    String simResponse = sendAtCommand4G("AT+CPIN?");
    if (simResponse.indexOf("READY") == -1) {
        Serial.println("[4G] SIM card not ready!");
        return false;
    }

    // Set to 4G only mode
    sendAtCommand4G("AT+CNMP=38", "OK", 5000);

    // Configure APN
    sendAtCommand4G("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"");

    // Activate PDP context
    sendAtCommand4G("AT+CGACT=1,1", "OK", 15000);

    modem4GInitialized = true;
    return true;
}

bool waitForNetworkRegistration() {
    Serial.println("[4G] Waiting for network registration...");
    for (int i = 0; i < 30; i++) {
        String response = sendAtCommand4G("AT+CEREG?", "+CEREG:", 3000);
        if (response.indexOf("+CEREG: 0,1") != -1 || response.indexOf("+CEREG: 0,5") != -1) {
            Serial.println("[4G] Network registered!");
            return true;
        }
        delay(2000);
    }
    return false;
}

void sendGPSDataToFirebase() {
    if (!modem4GInitialized || !networkRegistered) {
        Serial.println("[Firebase] Modem not ready");
        return;
    }

    // Create JSON with collar GPS data
    String jsonData = "{";
    jsonData += "\"latitude\":" + String(lastReceived.latitude, 6) + ",";
    jsonData += "\"longitude\":" + String(lastReceived.longitude, 6) + ",";
    jsonData += "\"altitude\":" + String(lastReceived.altitude, 1) + ",";
    jsonData += "\"satellites\":" + String(lastReceived.satellites) + ",";
    jsonData += "\"rssi\":" + String(lastReceived.rssi) + ",";
    jsonData += "\"snr\":" + String(lastReceived.snr, 1) + ",";
    jsonData += "\"time\":\"" + String(lastReceived.time) + "\",";
    jsonData += "\"packet_number\":" + String(packetCount) + ",";
    jsonData += "\"timestamp\":" + String(millis());
    jsonData += "}";

    if (sendDataToFirebase(jsonData.c_str())) {
        Serial.println("[Firebase] Data sent successfully!");
    } else {
        Serial.println("[Firebase] Failed to send data");
    }
}

bool sendDataToFirebase(const char* jsonData) {
    // Initialize HTTP
    sendAtCommand4G("AT+HTTPTERM");
    delay(500);
    sendAtCommand4G("AT+HTTPINIT");
    delay(500);

    // Build Firebase URL with JSON and auth token
    String firebaseUrl = "https://" + String(FIREBASE_DATABASE_URL) + "/collar_gps.json?auth=" + String(FIREBASE_AUTH_TOKEN);

    // Set HTTP parameters
    String urlCmd = "AT+HTTPPARA=\"URL\",\"" + firebaseUrl + "\"";
    sendAtCommand4G(urlCmd);
    sendAtCommand4G("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

    // Send data
    int dataLength = strlen(jsonData);
    String httpDataCmd = "AT+HTTPDATA=" + String(dataLength) + ",10000";
    String dataResponse = sendAtCommand4G(httpDataCmd.c_str(), "DOWNLOAD", 5000);

    if (dataResponse.indexOf("DOWNLOAD") != -1) {
        delay(500);
        SerialAT.print(jsonData);
        Serial.println("[Firebase] JSON sent: " + String(jsonData));
        delay(2000);

        // Send POST request
        sendAtCommand4G("AT+HTTPACTION=1", "OK", 3000);
        delay(5000);

        // Read response
        String readResponse = sendAtCommand4G("AT+HTTPREAD", "+HTTPREAD:", 5000);

        // Check for success (200 or 201 status code)
        if (readResponse.indexOf("+HTTPACTION: 1,200") != -1 ||
            readResponse.indexOf("+HTTPACTION: 1,201") != -1) {
            sendAtCommand4G("AT+HTTPTERM");
            return true;
        }
    }

    sendAtCommand4G("AT+HTTPTERM");
    return false;
}

/* ==================== CAMERA FUNCTIONS ==================== */
bool ei_camera_init(void) {
    if (is_camera_initialised) return true;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    sensor_t * s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);
        s->set_brightness(s, 1);
        s->set_saturation(s, 0);
    }

    is_camera_initialised = true;
    return true;
}

void ei_camera_deinit(void) {
    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK) {
        Serial.println("Camera deinit failed");
        return;
    }
    is_camera_initialised = false;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;

    if (!is_camera_initialised) {
        Serial.println("ERR: Camera is not initialized");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return false;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
    esp_camera_fb_return(fb);

    if (!converted) {
        Serial.println("Conversion failed");
        return false;
    }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS) ||
        (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
            out_buf,
            EI_CAMERA_RAW_FRAME_BUFFER_COLS,
            EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
            out_buf,
            img_width,
            img_height);
    }

    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        // Swap BGR to RGB
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) +
                              (snapshot_buf[pixel_ix + 1] << 8) +
                              snapshot_buf[pixel_ix];
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }
    return 0;
}

/* ==================== UTILITY FUNCTIONS ==================== */
void updateState(SystemState newState) {
    if (currentState != newState) {
        currentState = newState;

        const char* stateNames[] = {"IDLE", "MOTION_DETECTED", "INFERENCING", "PROCESSING_LORA"};
        Serial.printf("[STATE] %s -> %s\n",
                     stateNames[currentState],
                     stateNames[newState]);
    }
}

void blinkLED(int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_GPIO_NUM, HIGH);
        delay(delayMs);
        digitalWrite(LED_GPIO_NUM, LOW);
        delay(delayMs);
    }
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif
