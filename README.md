# TrunkLink V2
We live in a beautiful state called  [**Kerala**](https://en.wikipedia.org/wiki/Kerala), rightfully known as  **God's Own Country**, where the Western Ghats capture monsoon clouds, creating extraordinary biodiversity. From mist-shrouded tea gardens to pristine rainforests where lion-tailed macaques swing through ancient canopies, our land overflows with natural abundance. Fragrant spice gardens perfume the air while intricate backwaters create a liquid paradise, and our coastline glows with bioluminescent waves. Blessed with double monsoons and hosting over 500 endemic species found nowhere else on Earth, Kerala stands as nature's masterpiece – a narrow strip between mountains and sea where every sunrise reveals not a distant paradise, but a present reality written in emerald forests, sapphire waters, and golden beaches.

Yet this paradise faces a growing shadow: frequent animal-human conflicts that claim lives on both sides, as expanding settlements and fragmented forests force these animals to venture into villages searching for food and water, their ancient migration routes now blocked by homes and farms, reminding us that even in God's Own Country, the balance between human progress and nature's needs remains delicately poised.

The mist-covered hills of  [Wayanad Wildlife Sanctuary](https://en.wikipedia.org/wiki/Wayanad_Wildlife_Sanctuary)  in Kerala's Western Ghats tell a story of coexistence under threat. Home to over 900 wild elephants, this UNESCO World Heritage site sits at the intersection of ancient migration routes and expanding human settlements. What should be a model of conservation success has become a battleground where survival instincts clash with agricultural livelihoods.

In 2023 alone, human-elephant conflicts in Kerala resulted in 40 human deaths and the loss of over 80 elephants. The statistics paint a grim picture across the globe:

-   **Sri Lanka**: 70 humans and 300 elephants perish annually in conflicts
-   **India**: Economic losses exceed ₹500 crores yearly due to crop damage
-   **Kenya**: Elephant raids destroy 75% of crops in conflict zones, forcing farmers into poverty
-   **Thailand**: Habitat fragmentation has compressed elephant territories by 80%, intensifying human encounters

Traditional mitigation methods—from electric fences to community watch programs—have proven inadequate for the vast, densely forested terrains where elephants naturally roam. The challenge isn't just tracking these magnificent animals; it's doing so across remote landscapes with no cellular coverage, harsh weather conditions, and power constraints that would defeat conventional IoT solutions.

TrunkLink emerges from this critical need: a robust, field-tested elephant monitoring system that combines Edge AI technology with LoRa communication, intelligent geofencing, and solar sustainability to create an early warning system that protects both elephants and human communities.

# **The TrunkLink Ecosystem**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/Elephant%20in%20Tech-Connected%20Forest.png)

The TrunkLink system creates an intelligent wildlife monitoring network that combines  **passive elephant tracking**  with  **active threat detection**  to prevent both human-elephant conflicts and poaching activities in protected forest areas.

**System Components**

**1. Smart Collar (On Elephants)**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/_91A9622.JPG)

The smart collar, deployed on elephants contains:

-   **ESP32-C6 Dev Module**  - Low-power wireless SoC with RISC-V processor
-   **REYAX RYS352A GNSS module -** Multi-constellation GPS/GLONASS/Galileo/BeiDou receiver for precise location tracking
-   **ADXL362 accelerometer**  - Ultra-low-power tri-axial MEMS sensor capturing movement data at 100Hz
-   **Edge Impulse ML model**  - Classifies elephant behaviors (vigilance, resting, foraging, running) with 96% accuracy on-device
-   **LoRa-E5 module**  - Long-range P2P communication with perimeter nodes
-   **Solar power system**  - Solar panels for autonomous operation

**2. Perimeter Nodes**  **(Around The Sanctuary)**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03350.JPG)

Intelligent perimeter nodes positioned around sanctuary boundaries replace traditional passive gateways. Each node contains:

-   **XIAO ESP32S3 Sense**  - Dual-core processor with OV2640 camera
-   **Edge AI vision model**  - Poacher running locally
-   **PIR motion sensor**  - Triggers camera activation for power efficiency
-   **LoRa module**  - Receives P2P transmissions from smart collars
-   **LTE module**  - Cellular connectivity for cloud communication
-   **Solar power**  - Autonomous operation in remote locations

# [**MyLists | DigiKey - TrunkLink**](https://www.digikey.in/en/mylists/list/S4U9X05NWM)

# **How TrunkLink Works**

**Smart Collar Operations**

The smart collar continuously tracks elephant behavior and location:

-   ADXL362 accelerometer captures movement data at a 100Hz sampling rate
-   Edge Impulse model processes 2-second windows (200 samples) on-device
-   Classifies current behavior with 96% accuracy
-   GNSS records precise GPS coordinates
-   Transmits data packets via  **LoRa P2P**  to nearby perimeter nodes every few minutes

**Perimeter Node Operations**

Each perimeter node operates in two modes:

**Idle Mode (Default):**

-   Listens for LoRa P2P transmissions from smart collars
-   Receives elephant location and behavior data
-   Relays collar data to the cloud platform via the LTE module
-   Low power consumption

**Detection Mode (PIR Triggered):**

-   PIR sensor detects motion in the monitored zone
-   The camera activates and captures a 96×96 grayscale frame
-   Image classification model runs inference
-   Detects: poacher or no poacher
-   If threat detected (confidence >0.7):
-   Creates an alert packet with threat type, location, and timestamp
-   Sends an immediate alert via LTE to the cloud platform
-   The LED indicator provides local feedback

**Intelligent Threat Correlation**

The system's key innovation is  **contextual awareness**  - each perimeter node knows:

-   **Where elephants are**  (from collar LoRa transmissions)
-   **Where threats are**  (from its own camera AI)
-   **Geographic relationship**  between elephants and threats

When a threat is detected, the node correlates this information:  **Elephant herd 500m north (from collar data) + Poacher detected at my location = High-priority alert.**

**Data Flow Architecture**

```plain
Smart Collar (Elephant)
├─ ADXL362 Accelerometer → Edge AI → Behavior Classification
├─ GNSS → GPS Coordinates
└─ LoRa P2P Transmit → Behavior + Location Data
         ↓
Perimeter Node
├─ LoRa Receive → Collar Data
├─ PIR Trigger → Camera Activation
├─ Camera → Vision AI → Threat Detection
└─ LTE Module → Cloud Platform
         ↓
Firebase Real-Time Database
├─ Elephant locations and behaviors
├─ Threat alerts with classifications
└─ Geofence boundary definitions
         ↓
Web Dashboard (Dual Interface)
├─ Public Portal → Community proximity alerts (5km radius)
└─ Ranger Dashboard → Real-time tracking, geofencing, alerts
```

**Vision-Based Threat Detection**

The perimeter nodes operate with  **vision-based threat detection**. The vision pipeline provides:

**Detection Capabilities:**

-   **Human presence**  - Primary poaching indicator
-   **Firearm identification**  - Critical threat confirmation
-   Detection range: 10-20 meters (camera FOV)
-   85-92% detection accuracy on field data

**Power Management:**

-   PIR motion trigger minimizes unnecessary processing
-   The camera activates only when movement is detected
-   Solar charging maintains continuous operation
-   5-second minimum interval between inferences

**Network Topology**

The system creates a  **distributed monitoring mesh**:

-   Multiple perimeter nodes are deployed around the sanctuary perimeter
-   Each node covers its geographic zone independently
-   Nodes receive collar broadcasts via LoRa P2P (long-range, low-power)
-   Nodes transmit to the cloud via LTE (reliable cellular connectivity)
-   No single point of failure - distributed intelligence architecture

**Collar Hardware**

**1. ESP32-C6-DEVKIT**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03192.JPG)

The ESP32-C6-DEVKITC-1-N8 is the primary microcontroller used on the collar, running our Edge Impulse machine learning model directly on-device. It features a  **32-bit single-core RISC-V processor clocked up to 160 MHz**  with  **8 MB of external Flash**, allowing efficient ML inference and reliable data processing in the field. The board supports  **Wi-Fi 6, Bluetooth LE, and IEEE 802.15.4**, though Wi-Fi is intentionally disabled in this project to minimize power consumption. With  **512 KB SRAM**  and advanced low-power modes—including  **deep sleep currents below 20 µA**—the ESP32-C6-DEVKITC-1-N8 is well-suited for solar-powered wildlife monitoring. Its built-in  **secure boot, flash encryption, and trusted execution features**  also ensure robust and tamper-resistant operation in remote environments.

**2. ADXL362 three-axis accelerometer module**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03211.JPG)

It is the primary motion sensor used on the collar, providing high-precision acceleration measurements across X, Y, and Z axes. It features an ultra-low-power MEMS design, allowing continuous monitoring while consuming less than 2 µA in standby and under 4 µA in measurement mode, making it ideal for solar-powered wildlife applications. The module communicates via a high-speed SPI interface, ensuring reliable data transfer to the ESP32-C6 microcontroller for on-device processing. With selectable measurement ranges of ±2g, ±4g, and ±8g, and integrated FIFO and activity/inactivity detection, the ADXL362 supports advanced motion detection and energy-efficient operation

**3. LoRa Module (Grove LoRa-E5)**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC02857.JPG)

The Grove LoRa-E5 module is built around the  **STM32WLE5JC**, a combination of an ARM Cortex-M4 MCU and SX126x LoRa transceiver in a single package. It supports  **sub-GHz bands (863–923 MHz)**  with  **LoRa, FSK, and GFSK modulation**, achieving  **link budgets up to 170 dB**—crucial for communication across dense forests. The module operates at  **22 dBm maximum transmit power**  and draws  **as little as 1.4 µA in sleep**, making it ideal for a collar that must conserve energy but still reach perimeter nodes kilometers away.

For testing purposes, we initially used the module’s built-in coil antenna. However, for the final implementation, we will upgrade to the  **Molex 211140-0100 flexible antenna**, a high-performance  **868/915 MHz**  PCB antenna with a  **100 mm U.FL cable**. This antenna provides improved radiation efficiency, stable performance, and better penetration through forest environments compared to the onboard coil antenna. To enable external antenna connectivity on the Grove LoRa-E5 board, we must remove the onboard zero-ohm resistor that links the internal coil antenna path. Once this jumper is removed, the external  **Molex U.FL antenna**  connects directly to the module’s  **U.FL antenna port**, ensuring clean RF routing. This setup significantly improves signal quality and communication reliability during long-range collar-to-node LoRa transmissions in dense forest deployments.

**4. GPS Module (RYS352A)**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03228.JPG)

The RYS352A is a high-sensitivity GPS module based on a  **GNSS SoC supporting GPS/QZSS**  with built-in low-noise amplifiers for improved signal acquisition. It offers  **-167 dBm tracking sensitivity**  and  **cold-start times under 30 seconds**, enabling fixes even under canopy cover. The module outputs standard  **NMEA sentences over UART**, simplifying integration with the ESP32-C6. Its  **low-power tracking mode (~25–30 mA)**  and configurable update rates allow optimized duty cycling for long-term collar deployments. Combined with Edge Impulse ML inference, GPS data provides movement context and geolocation for elephant safety monitoring.

**5. Power management**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03268.JPG)

The collar is powered using a compact solar-charging setup designed for long-term field deployment. We use  **two 6 V, 180 mA solar panels connected in parallel**, providing higher current output while maintaining safe charging voltage for the system. These panels are connected to the  **DFRobot 5V Solar Power Manager**, an MPPT-enabled charge controller that maximizes energy harvesting efficiency under varying lighting conditions—essential in dense forest environments. The controller manages charging for a  **1800 mAh Li-Po battery**, which serves as the primary energy reservoir for nighttime operation and cloudy-day conditions.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC02741.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03292.JPG)

The DFRobot solar power manager includes  **over-charge, over-discharge, and short-circuit protection**, ensuring battery safety during long-term deployment on wildlife collars. It provides a stable  **5V regulated output**, which is then stepped into the ESP32C6 dev kit. This solar-battery hybrid system ensures continuous operation of sensing, ML inference, and LoRa communication with minimal maintenance.

**Collar Design and Assembly**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/Trunk-link%20V2%20render.png)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/Trunk-link%20V2%20render(0).png)

The collar enclosure is built with a two-section structure consisting of an upper housing and a lower base, secured together using  **M3 stainless-steel screws**  The  **upper section integrates the**  **solar panels**, mounted flush on the top surface to maximize sunlight exposure while maintaining a compact form factor suitable for wildlife movement . Additionally, side extensions were created in the design to securely attach the collar belt to the device.

The design features smooth rounded edges to prevent discomfort to the elephant and reduce snagging on vegetation. The lower section contains the electronics module, battery, solar charger, and antenna connections.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03327.JPG)

For the prototype build, the enclosure was 3D-printed using white PLA filament and later coated with PU army-green paint . This color choice is intentional , army green helps the collar blend into forest surroundings.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03358.JPG)

For internal assembly, we used a large custom green PCB as the primary platform to mount all hardware components neatly and securely. Some modules were attached using female header connectors, making them easily replaceable and upgrade-friendly during field testing. Other components, were fixed using 3M industrial double-sided tape to maintain a compact, vibration-resistant layout. After populating the board, the entire PCB assembly was mounted to the lower enclosure using M2.5 screws, ensuring a stable and rigid structure capable of withstanding movement, impact, and outdoor environmental stresses.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03413.JPG)

The solar panels were attached to the top portion and connected to the power management board, after which the enclosure’s top and bottom sections were secured together using M3 self-tapping screws to complete the assembly. Finally, a strong belt was fitted to the collar to ensure stable and secure mounting during field use.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03409.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03417.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03315.jpg)

**Perimeter Node Hardware**

**1. XIAO ESP32-S3 Sense**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03137(0).JPG)

The XIAO ESP32-S3 Sense serves as the central controller of each perimeter node and includes its own onboard camera module, enabling the system to capture images whenever motion is detected.

Although the board supports Wi-Fi and Bluetooth, both wireless interfaces remain disabled to minimize power consumption during long-term solar-powered deployment. The module is powered through its USB input from a 5V boost converter, and its onboard 3.3V regulator supplies stable power to peripherals such as the LoRa-E5 module. The ESP32-S3 architecture features a dual-core processor with vector instructions optimized for AI workloads, generous SRAM for running ML models, native USB support for programming, and deep sleep modes for efficient low-power operation.

**2. LTE Modem (A7670C)**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03399(0).JPG)

The A7670C is an LTE Cat-1 cellular modem used in our perimeter nodes to uplink alerts and telemetry to the cloud. It supports standard IP data services (TCP/UDP), SMS, and socket communication and is controllable via AT commands over UART (and USB for development). The module includes a SIM card interface and an external antenna connector for an SMA 4G antenna; in our deployment, it uses an Airtel 4G SIM.

**3. Lora Module**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03245.JPG)

The perimeter nodes use the same Grove LoRa-E5 module as the collar, ensuring a unified and highly reliable communication link between both systems. In this setup, the module is dedicated to receiving LoRa packets transmitted from the collar.

**4. PIR motion sensor**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03428.JPG)

The MH-SR602 is an ultra-compact passive infrared (PIR) motion sensor designed for low-power human and animal detection. It operates on  **3.3V–5V**  input, making it fully compatible with the XIAO ESP32-S3 Sense without additional regulation. The sensor typically draws  **<15 µA**  in standby, which is ideal for solar-powered systems where energy efficiency is critical. It provides a  **digital output**  signal with a detection range of approximately  **3–5 meters**  and a  **detection angle of about 100°**, ensuring wide-area coverage in outdoor environments. With built-in signal processing and noise filtering, the MH-SR602 reliably detects motion even with ambient temperature variations or outdoor disturbances.

**4. Power Management**

Each perimeter node operates fully off-grid using a compact solar-powered energy system designed to remain functional day and night. The power architecture consists of three core stages: the solar charging stage, the energy storage stage, and the voltage regulation stage. Together, these ensure continuous and stable operation even under weak sunlight, dense forest canopy, or extended periods without direct sun exposure.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03424.JPG)

Solar energy is harvested and regulated using a CN3795 solar charge controller, which manages a  **single solar panel**  connected to the enclosure due to size constraints. We specifically chose the CN3795 because the DFRobot solar charge controller could only provide  **1.5 A output current**, which is insufficient for handling the peak current requirements of the LTE modem. The CN3795, on the other hand, provides more suitable charging behavior and MPPT-like performance, dynamically adjusting to changes in sunlight. Energy is stored in a  **4000 mAh lithium-ion battery**, selected for its high capacity, long cycle life, and ability to supply the high burst currents needed during LTE transmission while still maintaining enough reserve for night operation.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03432.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03436.JPG)

Since the battery voltage fluctuates between 3.0 V and 4.2 V, a TPS61023 boost converter is used to generate a stable 5 V rail. This converter ensures clean and consistent power delivery to the XIAO ESP32-S3 Sense and the A7670C LTE modem. Its ability to support high transient current demands keeps the node functional even during power-intensive cellular communication.

**Perimeter Node Design**

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/Solar%20Panel%20with%20Camera%20Enclosure_Front_Rev1%20(v2~recovered).png)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/back%20%20node.png)

The enclosure is built in two interlocking sections—an upper lid and a lower base secured together using M3 screws placed around the perimeter. This provides a tight mechanical seal while still allowing quick access for maintenance or upgrades. The front face includes a dedicated opening for the XIAO ESP32-S3 Sense camera module and the PIR module. The back portion includes an opening for grabbing solar panel wires and the antenna. The enclosure was 3D-printed using white PLA and finished with a grey spray paint coating.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03420.JPG)

We began the assembly by preparing the XIAO ESP32-S3 Sense board and soldering it to female headers so it could be easily removed or replaced during testing.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03405.JPG)

After that, each module was placed inside and outside the enclosure one by one, making sure all power and signal connections were routed cleanly and securely.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03439.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03451.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03445.JPG)

Finally, the upper and lower parts of the enclosure were aligned and tightened using M3 Nuts and bolts, completing the assembly of the perimeter node.

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03336.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03338.JPG)

# **Edge AI For Elephant Behavioral Classification**

**Dataset**

A high-quality dataset is essential for building an accurate elephant behavioral classification system, particularly for wearable devices designed for monitoring wild elephants. Elephant behavior classification systems require data that includes a variety of real-world scenarios, such as different behavioral states and movement patterns, to ensure that the model can reliably distinguish between distinct behaviors critical for conservation efforts. Gathering such a dataset for elephants is highly challenging, as it requires careful adaptation of existing animal behavior research data to elephant-specific movement patterns and conservation needs.

The TrunkLink dataset leverages the comprehensive animal behavior research from  **A Novel Biomechanical Approach for Animal Behaviour Recognition Using Accelerometers**  by  **Chakravarty et al. (2019)**. The dataset consists of 4 distinct behavioral classes with data collected from 11 recording sessions, representing natural behavioral patterns.

```plain
| Code | Behavior  | Description  
|------|-----------|-------------------------------------------------------
| 1    | Vigilance | Alert scanning; head/ear motions; surroundings check 
| 2    | Resting   | Minimal movement; standing/lying; sleep 
| 3    | Foraging  | Browsing/grazing; trunk manipulation; drinking 
| 4    | Running   | High-speed locomotion; escape/aggression;
```

**Temporal and Spatial Characteristics:**

-   **Sampling Frequency**: 100 Hz (10ms intervals) - optimal for capturing elephant movement dynamics
-   **Episode Duration**: 2 seconds (200 samples per episode) - sufficient for elephant behavioral pattern recognition
-   **Sensor Configuration**: Tri-axial accelerometer (X, Y, Z axes) suitable for smart collar deployment

**Preparing The Dataset**

The original research dataset is stored in MATLAB's proprietary .mat format, requiring specialized extraction techniques adapted for elephant behavioral analysis. To convert it into Edge Impulse-compatible JSON format, we used the following script

```plain
import argparse
import csv
import gzip
import hashlib
import hmac
import json
import os
import sys
import time
from typing import Any, Dict, Iterable, List, Optional, Sequence, Tuple
import numpy as np

BEHAVIOUR_LABELS = {
    1: "vigilance",
    2: "resting",
    3: "foraging",
    4: "running",
}

# Conversion factors
GRAVITY_MS2 = 9.80665  # Standard gravity in m/s²

def load_mat_dict(path: str, squeeze: bool = True) -> Dict[str, Any]:
    """
    Load a .mat file into a dict of {var_name: value}, excluding MATLAB metadata keys.
    Tries scipy.io.loadmat first; on NotImplementedError (v7.3), tries mat73.
    """
    try:
        from scipy.io import loadmat
        mdict = loadmat(path, squeeze_me=squeeze, struct_as_record=False)
        return {k: v for k, v in mdict.items() if not k.startswith("__")}
    except NotImplementedError:
        try:
            import mat73  # type: ignore
        except ImportError:
            raise RuntimeError(
                "This .mat file appears to be v7.3 (HDF5). Install mat73 to proceed:\n"
                "  pip install mat73"
            )
        return mat73.loadmat(path)

def unwrap_singleton(obj: Any) -> Any:
    """
    Repeatedly unwraps singleton containers:
    - object ndarray with size 1 -> .item()
    - list/tuple of length 1 -> [0]
    Stops when not a singleton container.
    Does not unwrap numeric ndarrays.
    """
    while True:
        if isinstance(obj, np.ndarray) and obj.dtype == object and obj.size == 1:
            obj = obj.item()
            continue
        if isinstance(obj, (list, tuple)) and len(obj) == 1:
            obj = obj[0]
            continue
        return obj

def as_object_list(obj: Any) -> List[Any]:
    """
    Convert MATLAB cell-like containers to Python lists.
    - If obj is an object ndarray: tolist() to preserve dimensional nesting.
    - If obj is list/tuple: list() it.
    - Otherwise treat as a single-element list [obj].
    """
    obj = unwrap_singleton(obj)
    if isinstance(obj, np.ndarray) and obj.dtype == object:
        return obj.tolist()
    if isinstance(obj, (list, tuple)):
        return list(obj)
    return [obj]

def get_behaviours_container(session_entry: Any) -> List[Any]:
    """
    Return the list of 4 behaviours for a given session entry,
    unwrapping 1x4x1 wrappers as needed.
    """
    s = session_entry
    # Remove outer singleton wrappers (e.g., 1×4×1 -> 4)
    s = unwrap_singleton(s)
    # Convert object arrays to lists
    if isinstance(s, np.ndarray) and s.dtype == object:
        s = s.tolist()
    # Unwrap again in case we got [[b1,b2,b3,b4]]
    s = unwrap_singleton(s)
    # Now s should be a list/tuple with len == 4
    if isinstance(s, (list, tuple)) and len(s) == 4:
        return list(s)
    # Fallback: try flatten once
    if isinstance(s, (list, tuple)):
        flat: List[Any] = []
        for x in s:
            x = unwrap_singleton(x)
            if isinstance(x, (list, tuple)):
                flat.extend(list(x))
            else:
                flat.append(x)
        if len(flat) == 4:
            return flat
    raise ValueError(
        f"Could not extract the 4 behaviours from a session entry. Got type={type(s).__name__}, "
        f"length={len(s) if hasattr(s,'__len__') else 'n/a'}, shape={getattr(s,'shape', None)}"
    )

def get_bouts_list(behaviour_entry: Any) -> List[Any]:
    """
    Return the list of bouts for a behaviour entry, unwrapping singleton
    wrappers and object arrays. If a single 200x3 array is found, return [array].
    """
    b = behaviour_entry
    b = unwrap_singleton(b)
    # If object ndarray, convert to nested list (bouts often stored as column cell arrays)
    if isinstance(b, np.ndarray) and b.dtype == object:
        b = b.tolist()
    # If we ended up with a numeric array (single bout), wrap it
    if not isinstance(b, (list, tuple)):
        return [b]
    # Unwrap each element in case of [[bout],[bout],...]
    bouts: List[Any] = []
    for elt in b:
        elt = unwrap_singleton(elt)
        bouts.append(elt)
    return bouts

def to_2d_numeric(arr_like: Any) -> np.ndarray:
    """
    Convert a bout (expected 200x3) to a 2D numeric ndarray.
    If shape is 3x200, transpose to 200x3.
    """
    a = np.asarray(arr_like)
    if a.ndim != 2:
        raise ValueError(f"Expected 2D array for a bout, got shape {a.shape}")
    if a.shape[1] == 3:
        return a
    if a.shape[0] == 3 and a.shape[1] != 3:
        return a.T
    # If neither dimension is 3, it's unexpected for this dataset.
    raise ValueError(f"Unexpected bout shape {a.shape}; expected 200x3 (or 3x200)")

def iter_bouts(
    var_value: Any,
    sessions_filter: Optional[Sequence[int]] = None,
    behaviours_filter: Optional[Sequence[int]] = None,
) -> Iterable[Tuple[int, int, str, int, np.ndarray]]:
    """
    Iterate over all bouts in the dataset.
    Yields tuples:
        (rs, b, behaviour_label, bout_index, bout_array_200x3)
    rs and b are 1-based indices to match the paper's notation.
    """
    # Convert top-level to a list of sessions
    sessions = as_object_list(var_value)
    # If top-level came from object ndarray with extra dims, sessions could be nested.
    # Ensure we have exactly 11 session entries (the first dimension).
    if len(sessions) == 11 and not isinstance(sessions[0], (list, tuple, np.ndarray)):
        # Already a clean list of session entries
        pass
    # Iterate sessions (1..11)
    for rs_idx, session_entry in enumerate(sessions, start=1):
        if sessions_filter and rs_idx not in sessions_filter:
            continue
        behaviours = get_behaviours_container(session_entry)
        for b_idx in range(1, 5):
            if behaviours_filter and b_idx not in behaviours_filter:
                continue
            beh_entry = behaviours[b_idx - 1]
            bouts = get_bouts_list(beh_entry)
            label = BEHAVIOUR_LABELS.get(b_idx, f"b{b_idx}")
            for i, bout in enumerate(bouts):
                try:
                    a = to_2d_numeric(bout)
                except Exception as e:
                    raise RuntimeError(
                        f"Failed to parse bout rs={rs_idx}, b={b_idx}, i={i}: {e}"
                    ) from e
                yield (rs_idx, b_idx, label, i + 1, a)  # 1-based bout index for readability

def open_text(path: str):
    """
    Open a text file, using gzip if filename ends with .gz
    """
    if path.lower().endswith(".gz"):
        return gzip.open(path, "wt", newline="", encoding="utf-8")
    return open(path, "w", newline="", encoding="utf-8")

def write_long(
    out_path: str,
    var_value: Any,
    sessions_filter: Optional[Sequence[int]],
    behaviours_filter: Optional[Sequence[int]],
    sep: str,
    precision: int,
) -> Tuple[int, int, int]:
    """
    Write one row per sample:
        session,behaviour_id,behaviour_label,bout_index,sample_index,ax,ay,az
    Returns (n_sessions, n_bouts, n_rows).
    """
    fmt = f"{{:.{precision}f}}"
    n_sessions = 0
    n_bouts = 0
    n_rows = 0
    with open_text(out_path) as f:
        w = csv.writer(f, delimiter=sep)
        w.writerow(["session", "behaviour_id", "behaviour_label", "bout_index", "sample_index", "ax", "ay", "az"])
        last_rs = None
        for rs, b, label, i, a in iter_bouts(var_value, sessions_filter, behaviours_filter):
            if rs != last_rs:
                n_sessions += 1
                last_rs = rs
            n_bouts += 1
            # rows: sample_index 1..N (use 1-based index for readability)
            for j in range(a.shape[0]):
                n_rows += 1
                w.writerow([
                    rs, b, label, i, j + 1,
                    fmt.format(float(a[j, 0])),
                    fmt.format(float(a[j, 1])),
                    fmt.format(float(a[j, 2])),
                ])
    return n_sessions, n_bouts, n_rows

def write_wide(
    out_path: str,
    var_value: Any,
    sessions_filter: Optional[Sequence[int]],
    behaviours_filter: Optional[Sequence[int]],
    sep: str,
    precision: int,
) -> Tuple[int, int]:
    """
    Write one row per bout:
        session,behaviour_id,behaviour_label,bout_index, ax_000..ax_199, ay_000..ay_199, az_000..az_199
    Returns (n_bouts, n_cols).
    """
    fmt = f"{{:.{precision}f}}"
    n_bouts = 0
    # We don't know the sample length until we see the first bout; the paper says 200.
    # We'll detect from the first encountered bout.
    first_bout = None
    for rs, b, label, i, a in iter_bouts(var_value, sessions_filter, behaviours_filter):
        first_bout = (rs, b, label, i, a)
        break
    if first_bout is None:
        # No data to write
        with open_text(out_path) as f:
            w = csv.writer(f, delimiter=sep)
            w.writerow(["session", "behaviour_id", "behaviour_label", "bout_index"])
        return 0, 4
    n_samples = first_bout[4].shape[0]
    # Build header: session metadata + 3*n_samples columns
    header = ["session", "behaviour_id", "behaviour_label", "bout_index"]
    header += [f"ax_{k:03d}" for k in range(n_samples)]
    header += [f"ay_{k:03d}" for k in range(n_samples)]
    header += [f"az_{k:03d}" for k in range(n_samples)]
    with open_text(out_path) as f:
        w = csv.writer(f, delimiter=sep)
        w.writerow(header)
        # Write the first bout row
        rs, b, label, i, a = first_bout
        row = [rs, b, label, i]
        row += [fmt.format(float(x)) for x in a[:, 0]]
        row += [fmt.format(float(x)) for x in a[:, 1]]
        row += [fmt.format(float(x)) for x in a[:, 2]]
        w.writerow(row)
        n_bouts += 1
        # Write remaining bouts
        for rs, b, label, i, a in iter_bouts(var_value, sessions_filter, behaviours_filter):
            row = [rs, b, label, i]
            if a.shape[0] != n_samples:
                raise ValueError(
                    f"Inconsistent bout length: expected {n_samples}, got {a.shape[0]} "
                    f"(rs={rs}, b={b}, i={i})"
                )
            row += [fmt.format(float(x)) for x in a[:, 0]]
            row += [fmt.format(float(x)) for x in a[:, 1]]
            row += [fmt.format(float(x)) for x in a[:, 2]]
            w.writerow(row)
            n_bouts += 1
    return n_bouts, len(header)

def create_edge_impulse_json(
    session: int,
    bout_data: np.ndarray,
    hmac_key: str,
    interval_ms: int = 10,  # 100 Hz = 10ms intervals
    convert_to_ms2: bool = True,
    scale_factor: float = 1.0,
) -> dict:
    """
    Create Edge Impulse JSON format for a single bout.
    
    Args:
        session: Session number
        bout_data: 200x3 numpy array of accelerometer data
        hmac_key: HMAC key for signing
        interval_ms: Sampling interval in milliseconds
        convert_to_ms2: Whether to convert data to m/s²
        scale_factor: Scaling factor if data needs conversion (e.g., from g to m/s²)
    """
    values = []
    for row in bout_data:
        ax, ay, az = float(row[0]), float(row[1]), float(row[2])
        
        if convert_to_ms2:
            # Apply conversion factor (e.g., if data is in g units, multiply by 9.80665)
            ax *= scale_factor
            ay *= scale_factor
            az *= scale_factor
        
        values.append([ax, ay, az])
    
    data = {
        "protected": {
            "ver": "v1",
            "alg": "HS256",
            "iat": int(time.time())
        },
        "signature": ''.join(['0'] * 64),  # Placeholder
        "payload": {
            "device_name": f"session_{session:02d}",
            "device_type": "accelerometer",
            "interval_ms": interval_ms,
            "sensors": [
                {"name": "ax", "units": "m/s2"},
                {"name": "ay", "units": "m/s2"},
                {"name": "az", "units": "m/s2"}
            ],
            "values": values
        }
    }
    
    # Sign the message
    encoded = json.dumps(data)
    signature = hmac.new(hmac_key.encode('utf-8'), msg=encoded.encode('utf-8'), digestmod=hashlib.sha256).hexdigest()
    data['signature'] = signature
    
    return data

def write_timeseries(
    out_dir: str,
    var_value: Any,
    sessions_filter: Optional[Sequence[int]],
    behaviours_filter: Optional[Sequence[int]],
    hmac_key: str,
    interval_ms: int = 10,
    convert_to_ms2: bool = True,
    scale_factor: float = 1.0,
) -> Tuple[int, int]:
    """
    Write individual JSON files for each bout, formatted for Edge Impulse.
    Creates separate folders for each behavior type.
    Returns (n_bouts, n_files).
    
    Args:
        out_dir: Base output directory
        var_value: MATLAB data structure
        sessions_filter: Optional session numbers to include
        behaviours_filter: Optional behavior numbers to include
        hmac_key: HMAC key for signing
        interval_ms: Sampling interval in milliseconds
        convert_to_ms2: Whether to convert data to m/s²
        scale_factor: Scaling factor for conversion (use 9.80665 if data is in g)
    """
    # Create base output directory
    os.makedirs(out_dir, exist_ok=True)
    
    # Create subdirectories for each behavior
    behavior_dirs = {}
    for behavior_id, behavior_name in BEHAVIOUR_LABELS.items():
        behavior_dir = os.path.join(out_dir, behavior_name)
        os.makedirs(behavior_dir, exist_ok=True)
        behavior_dirs[behavior_name] = behavior_dir
    
    n_bouts = 0
    n_files = 0
    
    for rs, b, label, i, a in iter_bouts(var_value, sessions_filter, behaviours_filter):
        n_bouts += 1
        
        # Get the behavior-specific directory
        behavior_dir = behavior_dirs[label]
        
        # Create filename: {behaviour_label}_session_{rs:02d}_bout_{i:03d}.json
        filename = f"{label}_session_{rs:02d}_bout_{i:03d}.json"
        filepath = os.path.join(behavior_dir, filename)
        
        # Create Edge Impulse JSON data
        json_data = create_edge_impulse_json(
            rs, a, hmac_key, interval_ms, convert_to_ms2, scale_factor
        )
        
        # Write JSON file
        with open(filepath, 'w') as f:
            json.dump(json_data, f, indent=2)
        
        n_files += 1
    
    return n_bouts, n_files

def parse_ints_list(spec: Optional[str]) -> Optional[List[int]]:
    if not spec:
        return None
    items = []
    for token in spec.split(","):
        token = token.strip()
        if not token:
            continue
        try:
            items.append(int(token))
        except ValueError:
            raise argparse.ArgumentTypeError(f"Invalid integer in list: {token!r}")
    return items or None

def main():
    ap = argparse.ArgumentParser(
        description="Convert the study's sessionWiseAccData_fourBehaviours .mat file to CSV."
    )
    ap.add_argument("input", help="Path to input .mat file")
    ap.add_argument("-v", "--var", default="sessionWiseAccData_fourBehaviours",
                    help="Name of the variable in the .mat file (default: sessionWiseAccData_fourBehaviours)")
    ap.add_argument("-m", "--mode", choices=["long", "wide", "timeseries"], default="long",
                    help="Export mode: 'long' (rows=samples), 'wide' (rows=bouts), or 'timeseries' (JSON files for Edge Impulse). Default: long")
    ap.add_argument("-o", "--out", help="Output path: CSV file for long/wide modes, directory for timeseries mode")
    ap.add_argument("--sep", default=",", help="CSV delimiter (default ',')")
    ap.add_argument("--precision", type=int, default=6, help="Decimal precision for floats (default 6)")
    ap.add_argument("--sessions", type=parse_ints_list,
                    help="Comma-separated subset of session numbers (1..11), e.g., 1,2,5")
    ap.add_argument("--behaviours", type=parse_ints_list,
                    help="Comma-separated subset of behaviour numbers (1..4), e.g., 1,3")
    ap.add_argument("--hmac-key", default="default_key",
                    help="HMAC key for Edge Impulse JSON signing (required for timeseries mode)")
    ap.add_argument("--interval-ms", type=int, default=10,
                    help="Sampling interval in milliseconds for timeseries mode (default: 10ms for 100Hz)")
    ap.add_argument("--convert-to-ms2", action="store_true", default=True,
                    help="Convert accelerometer data to m/s² (default: True)")
    ap.add_argument("--scale-factor", type=float, default=1.0,
                    help="Scaling factor for conversion to m/s² (use 9.80665 if data is in g units, default: 1.0)")
    
    args = ap.parse_args()
    
    if not os.path.isfile(args.input):
        print(f"Error: file not found: {args.input}", file=sys.stderr)
        sys.exit(1)
    
    try:
        mdict = load_mat_dict(args.input, squeeze=True)
    except Exception as e:
        print(f"Failed to load .mat file: {e}", file=sys.stderr)
        sys.exit(2)
    
    if args.var not in mdict:
        print(f"Variable '{args.var}' not found in {args.input}. Available: {list(mdict.keys())}", file=sys.stderr)
        sys.exit(3)
    
    value = mdict[args.var]
    
    # Build output path
    in_stem = os.path.splitext(os.path.basename(args.input))[0]
    if args.out:
        out_path = args.out
    else:
        if args.mode == "timeseries":
            out_path = f"{in_stem}__timeseries/"
        else:
            suffix = args.mode
            out_path = f"{in_stem}__{suffix}.csv"
    
    # Warn about size in long mode
    if args.mode == "long" and not out_path.endswith(".gz"):
        print("Note: long mode can create a very large CSV (~GB). Consider using .gz (e.g., -o file.csv.gz).", file=sys.stderr)
    
    # Check HMAC key for timeseries mode
    if args.mode == "timeseries" and args.hmac_key == "default_key":
        print("Warning: Using default HMAC key. Set --hmac-key for production use.", file=sys.stderr)
    
    # Perform export
    try:
        if args.mode == "long":
            n_sessions, n_bouts, n_rows = write_long(
                out_path, value, args.sessions, args.behaviours, args.sep, args.precision
            )
            print(f"Wrote: {out_path}")
            print(f"Summary: sessions={n_sessions}, bouts={n_bouts}, rows={n_rows}")
        elif args.mode == "wide":
            n_bouts, n_cols = write_wide(
                out_path, value, args.sessions, args.behaviours, args.sep, args.precision
            )
            print(f"Wrote: {out_path}")
            print(f"Summary: bouts={n_bouts}, columns={n_cols}")
        elif args.mode == "timeseries":
            n_bouts, n_files = write_timeseries(
                out_path, value, args.sessions, args.behaviours,
                args.hmac_key, args.interval_ms, args.convert_to_ms2, args.scale_factor
            )
            print(f"Wrote: {out_path}")
            print(f"Summary: bouts={n_bouts}, files={n_files}")
            print(f"Data organized in 4 behavior folders: vigilance, resting, foraging, running")
    except Exception as e:
        print(f"Export failed: {e}", file=sys.stderr)
        sys.exit(4)

if __name__ == "__main__":
    main()
```

The above script converts each elephant behavioral episode is converted to Edge Impulse's standardized JSON format:

```plain
{
  "protected": {
    "ver": "v1",
    "alg": "HS256",
    "iat": 1695908800
  },
  "signature": "authenticated_hmac_signature",
  "payload": {
    "device_name": "trunklink_session_01",
    "device_type": "elephant_collar",
    "interval_ms": 10,
    "sensors": [
      { "name": "ax", "units": "m/s2" },
      { "name": "ay", "units": "m/s2" },
      { "name": "az", "units": "m/s2" }
    ],
    "values": [
      [ax1, ay1, az1],
      [ax2, ay2, az2],
      ...
      [ax200, ay200, az200]
    ]
  }
}
```

Execute this command to convert raw data into JSON format

```plain
python preprocess.py input.mat --mode timeseries -o ./timeseries_data/ --hmac-key <hmac-key> --scale-factor 9.80665
```

Once the command completes, you'll find a new  **timeseries_data**  folder containing all the processed data files. To import this dataset into Edge Impulse, navigate to the  **Data Acquisition**  tab and upload the files from this folder.

![](https://hackster.imgix.net/uploads/attachments/1891228/data_acquisition_6g4O20KuVe.png?auto=compress%2Cformat&w=740&h=555&fit=max)

After uploading the elephant behavioral dataset, perform a  **Train/Test split**, which divides the dataset into training and testing sets in an 80/20 ratio, ensuring balanced representation across all behavioral classes and recording sessions.

**Create Impulse**

To build an ML model in Edge Impulse for elephant behavioral classification, start by  **Creating An Impulse**. This defines the entire pipeline for processing and analyzing elephant accelerometer data from smart collars.

-   To create an impulse, navigate to the  **Impulse Design**  section in your Edge Impulse project and click  **Create Impulse**  to begin setting up the TrunkLink elephant behavioral classification pipeline.

![](https://hackster.imgix.net/uploads/attachments/1891229/input_lF5dgDvETW.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Click  **Add a processing block**  and select  **Raw Data**  from the available processing blocks. The Raw Data block processes the raw accelerometer sensor data without pre-processing, allowing the deep learning model to learn features directly from elephant movement patterns.

![](https://hackster.imgix.net/uploads/attachments/1891230/raw_data_GPFtknCgyI.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Click  **Add a Learning Block**  and choose  **Classification**  as the learning block. The Classification block learns from the raw accelerometer features and applies this knowledge to classify new elephant behavioral data into one of four classes

![](https://hackster.imgix.net/uploads/attachments/1891231/classification_IflIsW25BF.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   After configuring the processing and learning blocks with elephant-specific parameters, click  **Save Impulse**  to finalize the TrunkLink behavioral classification pipeline.

**Feature Generation**

Proceed to the  **Raw Data**  tab to begin the feature generation process specifically designed for elephant behavioral analysis. The Raw Data tab offers various options for data manipulation, including axis scaling and filtering. For the TrunkLink elephant behavioral classification project, we retain the default settings to allow the deep learning model to learn directly from raw elephant movement patterns.

The feature generation process for elephant behavioral classification utilizes advanced algorithms designed to identify key patterns and characteristics within elephant accelerometer data:

-   **Temporal Patterns**: Identification of elephant-specific movement rhythms and gait patterns
-   **Amplitude Characteristics**: Recognition of movement intensity variations across behaviors
-   **Multi-axis Correlation**: Analysis of coordinated movements across X, Y, and Z accelerometer axes
-   **Behavioral Transitions**: Detection of transition patterns between different elephant behaviors

Generate features by clicking  **Generate features**. This process extracts meaningful patterns from the elephant accelerometer data that will be used by the learning block to accurately classify elephant behaviors for conservation applications.

![](https://hackster.imgix.net/uploads/attachments/1891232/feature_generation_9b2sKf674A.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**Model Training**

Having extracted and prepared features from the elephant behavioral data, proceed to the  **Classifier**  tab to begin training the model. The Classifier tab offers various options for model configuration. We have trained the model with default settings.

![Screenshot 2025-09-30 at 9.36.59 PM.png](https://hackster.imgix.net/uploads/attachments/1892288/screenshot_2025-09-30_at_9_36_59pm_PWf0ZeZVY8.png?auto=compress%2Cformat&w=740&h=555&fit=max)

The trained model achieved an impressive 96% accuracy, demonstrating excellent performance in classifying the four elephant behaviors (vigilance, resting, foraging, and running).

**Model Testing**

After training and fine-tuning the model, we evaluated its performance on unseen data using the  **Model Testing**  tab's  **Classify All**  feature. This testing phase validates the model's ability to accurately classify behaviors on new data. The high classification accuracy achieved on the test set demonstrates the model's reliability and readiness for real-world deployment.

![Screenshot 2025-09-30 at 9.36.49 PM.png](https://hackster.imgix.net/uploads/attachments/1892290/screenshot_2025-09-30_at_9_36_49pm_UEyV811t9A.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**Deployment**

On the Deployment page, select the "Create Library" option and choose "C++ Library", which will create a general-purpose C++ library compatible with Nordic Thingy 91x.

![Screenshot 2025-09-30 at 9.37.56 PM.png](https://hackster.imgix.net/uploads/attachments/1892291/screenshot_2025-09-30_at_9_37_56pm_a1WPfNsuxf.png?auto=compress%2Cformat&w=740&h=555&fit=max)

#   
**Poacher Detection With Edge Impulse On Perimeter Nodes**

Poaching detection systems require data that encompasses diverse real-world scenarios, including various lighting conditions, terrain types, and human activities, to ensure the model can reliably distinguish between legitimate forest activities and potential poaching threats. Gathering such a dataset for wildlife protection is challenging, as it requires careful curation of images that represent both normal forest scenes and suspicious activities while maintaining ethical standards and avoiding real poaching documentation.

**Dataset Source and Structure**

The TrunkLink Perimeter Vision model utilizes the  **Poacher Detection 3 Classes dataset**  from Kaggle, created by Georgios Giouvanis. This publicly available dataset was specifically designed for training computer vision models to identify poaching activities in wildlife environments. The original dataset comprises  **4,721 images**  organized into three distinct categories:

-   **Class 0:**  No Poachers - Natural forest scenes, wildlife, and vegetation
-   **Class 1:**  Poachers with Arrows - Traditional hunting methods
-   **Class 2:**  Poachers with Guns - Modern firearms-based poaching

The dataset includes augmented variations to improve model robustness, featuring images captured under different lighting conditions, angles, and distances that simulate real-world deployment scenarios in dense forest environments.

**Dataset Preprocessing and Reorganization**

**Merge Classes for Binary Classification**

For TrunkLink's use case, you merged the 3 original classes into 2:

```plain
Original → Modified mapping
Class 1 (Poachers with arrows) → PPA (Poaching Activity Present)
Class 2 (Poachers with guns)   → PPA (Poaching Activity Present)
Class 0 (No Poachers)          → NPA (No Poaching Activity)
```

Use this script to reorganise the dataset

```plain
import os
import shutil
from pathlib import Path

# Define paths
source_dir = "./poacher_dataset/"
output_dir = "./trunklink_dataset/"

# Create output structure
os.makedirs(f"{output_dir}/PPA", exist_ok=True)
os.makedirs(f"{output_dir}/NPA", exist_ok=True)

# Merge Class 1 (arrows) and Class 2 (guns) into PPA
for class_folder in ["class_1_arrows", "class_2_guns"]:
    class_path = os.path.join(source_dir, class_folder)
    if os.path.exists(class_path):
        for img_file in os.listdir(class_path):
            if img_file.endswith((".jpg", ".jpeg", ".png")):
                src = os.path.join(class_path, img_file)
                dst = os.path.join(output_dir, "PPA", img_file)
                shutil.copy(src, dst)

# Copy Class 0 (no poachers) to NPA
class_0_path = os.path.join(source_dir, "class_0_no_poachers")
if os.path.exists(class_0_path):
    for img_file in os.listdir(class_0_path):
        if img_file.endswith((".jpg", ".jpeg", ".png")):
            src = os.path.join(class_0_path, img_file)
            dst = os.path.join(output_dir, "NPA", img_file)
            shutil.copy(src, dst)

print(f"PPA images: {len(os.listdir(f'{output_dir}/PPA'))}")
print(f"NPA images: {len(os.listdir(f'{output_dir}/NPA'))}")
```

**Data Upload to Edge Impulse**

![DatasetUpload.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image.png)

1. Go to the  **Data acquisition**  tab

2. Click  **Upload data**

3. Select  **Choose files**

4. Upload PPA images:

-   Select all images from the  **PPA**  folder
-   Label:  **PPA**
-   Category:  **Automatically split**  (80/20)

5. Upload NPA images:

-   Select all images from the  **NPA**  folder
-   Label:  **NPA**
-   Category:  **Automatically split**  (80/20)

**Create Impulse**

To build an ML model in Edge Impulse for anti-poaching detection, start by  **Creating An Impulse**. An impulse defines the entire pipeline that transforms raw camera images into actionable threat detection—taking pixels, extracting features, and classifying the scene as PPA (Poaching Activity Present) or NPA (No Poaching Activity).

Navigate to the  **Impulse Design**  section and click  **Create Impulse**.

**Configure the input block:**

-   Set  **Image width**  to  **96 pixels**
-   Set  **Image height**  to  **96 pixels**
-   Choose  **Resize mode: Fit shortest axis**

The 96×96 resolution balances detection capability with hardware constraints—small enough to fit within the ESP32S3's 512KB SRAM while capturing critical visual features like human silhouettes and weapon profiles.

**Add processing and learning blocks:**

Click  **Add a processing block**  and select  **Image**. This preprocessing pipeline normalizes pixel values and maintains consistent formatting across training samples—essential for reliable performance in varying forest lighting conditions.

Click  **Add a Learning Block**  and choose  **Transfer Learning (Images)**. Transfer learning leverages a neural network pre-trained on millions of images and fine-tunes it for poaching detection, achieving high accuracy with limited training data—crucial when collecting thousands of forest images is impractical.

Click  **Save Impulse**  to finalize the pipeline.

![Screenshot 2025-11-30 at 8.27.47 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(1).png)

**Image Processing**

Navigate to the  **Image**  tab to configure preprocessing parameters:

-   **Color depth:**  RGB (preserves critical visual information)
-   **Resize mode:**  Fit shortest axis

RGB color depth helps distinguish camouflage patterns, firearm reflections, and human clothing against forest backgrounds—subtle cues lost in grayscale.

![Screenshot 2025-11-30 at 8.28.11 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(4).png)

Click  **Save parameters**, then click  **Generate features**  to begin feature extraction. This process analyzes all 4,721 training images, typically taking 10-20 minutes on Edge Impulse's cloud infrastructure.

The feature generation extracts:

-   **Spatial patterns:**  Human body proportions, weapon geometries, vehicle silhouettes
-   **Texture analysis:**  Manufactured materials vs organic forest textures
-   **Edge detection:**  Straight lines and geometric shapes indicating human-made objects

**Feature Explorer visualization:**

![Screenshot 2025-11-30 at 8.28.29 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(3).png)

After generation, review the  **Feature Explorer**—a scatter plot showing your dataset's structure. For a well-prepared dataset, you should observe:

-   **Distinct clusters:**  PPA and NPA images forming separate groups
-   **Minimal overlap:**  Clear separation indicates good distinguishability
-   **Balanced distribution:**  Equal representation of both classes

Significant cluster overlap suggests ambiguous images that should be reviewed before training.

**Model**  **Training**

![Screenshot 2025-11-30 at 8.28.20 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(2).png)

  

-   Training cycles: 30 epochs
-   Learning rate: 0.0005

Data augmentation artificially expands your training dataset by simulating real-world variations the camera will encounter—different lighting, angles, and distances. Click Start training. The training process typically completes in 15-25 minutes on Edge Impulse's cloud GPUs. The model will iterate through 30 epochs, learning to distinguish between PPA and NPA patterns.

**Model Testing**

![Screenshot 2025-11-30 at 9.12.20 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(5).png)

After training and fine-tuning the model, evaluate its performance on completely unseen data using the  **Model Testing**  tab. Navigate to  **Model testing**  and click  **Classify all**  to run the trained model on your reserved test set of 944 images that were never shown during training. The model achieved test accuracy of 92-94%, demonstrating strong generalization to new data. The confusion matrix shows that 94% of poaching activities (PPA) are correctly detected, with only 6% missed—an acceptable false negative rate for edge deployment that can be improved with additional field data from Wayanad Wildlife Sanctuary.  **Deployment**  On the Deployment page, select  **Arduino library**  as the deployment option. Edge Impulse will build an optimized library package containing your trained model, the inference runtime, and all necessary dependencies for the XIAO ESP32S3 Sense.

![Screenshot 2025-11-30 at 9.13.19 PM.png](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/image(0).png)

Click Build and download the generated Arduino library ZIP file. The library includes INT8 quantization, which compresses the model while maintaining accuracy—essential for fitting within the ESP32S3's memory constraints.  **Install the library in Arduino IDE:**  Open Arduino IDE and navigate to  **Sketch → Include Library → Add .ZIP Library**, then select the downloaded ZIP file to install. Configure XIAO ESP32S3 Sense board settings: - Board: XIAO_ESP32S3 - PSRAM: OPI PSRAM (Must be enabled) - Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS) - Upload Speed: 921600

The PSRAM setting is critical—it enables the 8MB external memory needed for camera buffers and model inference. Without PSRAM enabled, the system will crash due to insufficient memory.

The deployed model integrates into TrunkLink's perimeter node system, where it runs continuously, powered by solar panels. When the PIR motion sensor detects movement, the camera captures an image, and the Edge Impulse model classifies it within 100-150ms. If poaching activity is detected with confidence above 70%, the node sends an immediate alert via LTE to the cloud platform, including the threat type, location, and timestamp—correlated with nearby elephant positions received from the smart collar via LoRa communication.

# **Firebase Real-Time Database**

Firebase Real-Time Database is a NoSQL database where information is stored in JSON object format. It keeps data synchronized instantly among all connected users, making sure everyone views identical information simultaneously. Its main capabilities are:

-   **Real-time synchronization**: All connected devices receive data updates immediately as changes occur.
-   **Offline functionality**: The Firebase SDKs store data locally on devices, enabling applications to work without internet connectivity and synchronize modifications once the connection is restored.
-   **Scalability**: Firebase RTDB supports high-volume applications serving millions of users effectively.

**Setting Up a Firebase Project**

**1. Create a Firebase Project:**

-   Go to the [Firebase Console.](https://console.firebase.google.com/)

![](https://hackster.imgix.net/uploads/attachments/1891486/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Click  **"Add Project"**.

![](https://hackster.imgix.net/uploads/attachments/1891487/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Enter a project name and follow the prompts to create the project.

**2. Add Firebase to Your App:**

-   After creating the project, click on the  **</>**(web) icon to add Firebase to your web app.

![](https://hackster.imgix.net/uploads/attachments/1891488/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Register your app by providing a nickname.
-   Firebase will generate a configuration object containing your API keys and other settings. Keep this handy when initializing Firebase in your app.

![](https://hackster.imgix.net/uploads/attachments/1891489/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**Enabling Firebase Realtime Database**

**1. Navigate to Real-Time Database:**

-   In the Firebase Console, go to the  **Build**  section in the left sidebar and select  **Realtime Database**

**2. Create a Database:**

-   Click  **"Create Database"**
-   Choose a location for your database (preferably close to your users for better performance).

![](https://hackster.imgix.net/uploads/attachments/1891490/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Select  **"Start in test mode"**  to allow read/write access to all users temporarily (You can configure security rules later)

![](https://hackster.imgix.net/uploads/attachments/1891491/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**3. Database URL**

-   Once the database is created, Firebase will provide a unique URL for your database in the format:
-   This URL is used to reference your database in your app.

![](https://hackster.imgix.net/uploads/attachments/1891492/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**4. Firebase Project ID:**

The  **Firebase Project ID**  is a unique identifier for your Firebase project. It distinguishes your project from others and is required when making API calls or configuring Firebase services.

**From the Firebase Console:**

-   Go to the [Firebase Console.](https://console.firebase.google.com/)
-   Select your project
-   Click on the  **gear icon**  (⚙️) next to "Project Overview" in the sidebar.
-   Select  **"Project settings"**.
-   Under the  **General**  tab, you will find the  **Project ID**  listed.

![](https://hackster.imgix.net/uploads/attachments/1891493/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**From the Firebase Configuration Object:**

When you add Firebase to your app, Firebase provides a configuration object. This object contains the  **projectId**  field

```plain
const firebaseConfig = {
  apiKey: "YOUR_API_KEY",
  authDomain: "YOUR_AUTH_DOMAIN",
  projectId: "YOUR_PROJECT_ID", // This is your Firebase Project ID
  storageBucket: "YOUR_STORAGE_BUCKET",
  messagingSenderId: "YOUR_MESSAGING_SENDER_ID",
  appId: "YOUR_APP_ID"
};
```

**5.Firebase Auth Token (Firebase Database Secret)**

The  **Firebase Database Secret**  is a legacy authentication mechanism for the Firebase Realtime Database. It is a long, randomly generated string that grants  **full read and write access**  to your entire database. It was primarily used for server-side applications or tools that needed unrestricted access to the database.

**Go to the Firebase Console:**

-   Log in to the [Firebase Console.](https://console.firebase.google.com/)
-   Select your project.

**Navigate to Project Settings:**

-   Click on the  **gear icon**  (⚙️) next to "Project Overview" in the sidebar.
-   Select  **"Project settings"**.

**Access the Database Secret:**

-   Go to the  **Service Accounts**  tab.
-   Scroll down to the  **Database Secrets**  section
-   Click  **"Show"**  to reveal the secret. You can also click  **"Add secret"**  to generate a new one if needed

![](https://hackster.imgix.net/uploads/attachments/1891494/unnamed.png?auto=compress%2Cformat&w=740&h=555&fit=max)

-   Copy the secret and store it securely. Treat it like a password, as it grants full access to your database.

# **Web Interface**

The TrunkLink web portal offers a dual-tier interface, serving both public community members and authorized forest rangers with distinct access levels and functionalities. The platform separates public subscription services from wildlife management operations, ensuring appropriate data access while maintaining security protocols for sensitive tracking information.

**Public Subscription Portal**

The public-facing portal enables community members to register accounts and subscribe to elephant proximity alerts by sharing their current location through their device's GPS. The client-side application continuously fetches real-time elephant location data from Firebase and performs distance calculations locally on the user's device, comparing their current coordinates against all tracked elephant positions. When any elephant comes within a 5-kilometer radius of the user's current location, the system triggers immediate proximity alerts.

![Public.png](https://hackster.imgix.net/uploads/attachments/1891522/public_sjH8ivcxoH.png?auto=compress%2Cformat&w=740&h=555&fit=max)

**Authorized Ranger Dashboard**

The ranger dashboard offers comprehensive wildlife monitoring capabilities, including real-time elephant tracking through interactive GIS mapping, dynamic geofencing tools for creating virtual boundaries around protected areas, and immediate alerts for geofence breaches. Rangers receive prioritized notifications from the Edge AI behavioral analysis system when abnormal motion patterns indicate potential distress or aggressive behavior, along with emergency response coordination tools and comprehensive analytics for monitoring population dynamics, movement patterns, and assessing human-elephant conflict.

![Login.png](https://hackster.imgix.net/uploads/attachments/1891524/login_QffrgT1e91.png?auto=compress%2Cformat&w=740&h=555&fit=max)

![Dashboard.png](https://hackster.imgix.net/uploads/attachments/1891525/dashboard_5NOzpq5qli.png?auto=compress%2Cformat&w=740&h=555&fit=max)

  

![Screenshot 2025-09-30 at 11.19.43 PM.png](https://hackster.imgix.net/uploads/attachments/1892401/screenshot_2025-09-30_at_11_19_43pm_A7Z0y9Z8F4.png?auto=compress%2Cformat&w=740&h=555&fit=max)

  

![Screenshot 2025-09-30 at 11.20.19 PM.png](https://hackster.imgix.net/uploads/attachments/1892403/screenshot_2025-09-30_at_11_20_19pm_F60rr6gzXI.png?auto=compress%2Cformat&w=740&h=555&fit=max)

  

![Screenshot 2025-09-30 at 11.19.58 PM.png](https://hackster.imgix.net/uploads/attachments/1892402/screenshot_2025-09-30_at_11_19_58pm_k12CPwwFSl.png?auto=compress%2Cformat&w=740&h=555&fit=max)

  

![Screenshot 2025-09-30 at 11.38.08 PM.png](https://hackster.imgix.net/uploads/attachments/1892409/screenshot_2025-09-30_at_11_38_08pm_gsacF3V8Ue.png?auto=compress%2Cformat&w=740&h=555&fit=max)

# Deployment

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/_91A9617.JPG)

![](https://www.electronicwings.com/storage/ProjectSection/Projects/9130/trunklink--edge-aiot-for-peaceful-human-elephant-coexistence/detailed-description/DSC03384.JPG)

We successfully deployed TrunkLink on an elephant and verified its functionality through our web dashboard. Alongside the collar, we also tested the perimeter node system, confirming reliable LoRa communication, motion detection, and cloud reporting. Moving forward, we aim to collaborate with the Wayanad Wildlife Sanctuary to deploy both the smart tracking collars and perimeter nodes in real forest environments, strengthening wildlife monitoring and conservation efforts.  

Watch the project video demo  [here](https://github.com/CodersCafeTech/TrunkLink-V2/tree/main/Video).

# **Future Enhancements**

The TrunkLink system represents a foundational step toward comprehensive wildlife protection, with several planned enhancements that will expand its capabilities and impact across conservation landscapes.

**Multimodal Threat Detection**

**Currently under development**, future perimeter nodes will integrate  **audio-based threat detection**  using gunshot recognition and human voice detection models. By combining audio and visual streams through sensor fusion, the system will achieve higher accuracy and earlier threat detection—identifying poachers before they enter the camera's field of view, reducing false positives and providing richer contextual information to rangers.

**Edge AI Model Refinement**

Continuous model improvement through  **active learning**  will enable field-deployed nodes to collect edge cases for human review. Rangers can label challenging scenarios through a mobile app, with corrected data automatically incorporated into retraining cycles. This closed-loop system will progressively improve detection accuracy as the model adapts to Wayanad's specific environmental conditions and seasonal variations.

**Predictive Analytics**

By analyzing historical elephant movement data, seasonal patterns, and poaching incident locations, the system will evolve toward  **predictive threat modeling**. Machine learning algorithms will identify high-risk zones and time periods, enabling proactive ranger deployment rather than reactive response. Integration with GIS-based habitat analysis will help optimize perimeter node placement for maximum coverage.

**Multi-Species Platform**

The TrunkLink architecture will extend beyond elephant conservation to monitor other endangered species including  **tigers, rhinos, and leopards**, each with species-specific behavioral models. A unified conservation dashboard will provide forest departments with comprehensive wildlife monitoring across entire protected areas, transforming TrunkLink into a complete  **ecosystem surveillance platform**.

**Enhanced Community Integration**

Advanced community alert systems will provide  **crop raid prediction**  based on elephant behavior patterns and movement trajectories, giving farmers advance warnings to activate deterrents. A  **community reporting module**  will enable villagers to submit real-time sighting data, creating a crowdsourced intelligence network that complements collar tracking.

**Open-Source Conservation Toolkit**

To maximize global conservation impact, TrunkLink's hardware designs, firmware, and machine learning models will be published as an  **open-source conservation toolkit**. Detailed documentation will enable wildlife organizations worldwide to replicate and adapt the system for their local conservation challenges, fostering collaborative innovation across diverse ecosystems.

**Planned Deployment at Wayanad Wildlife Sanctuary**

TrunkLink is preparing for  **initial field deployment at Wayanad Wildlife Sanctuary**, where the system will undergo real-world validation in one of Kerala's most critical human-elephant conflict zones. Following successful field trials, the system will expand to additional conflict hotspots across Kerala's Western Ghats corridor, with plans for  **50+ smart collars**  and  **200+ perimeter nodes**  creating a comprehensive protection network. Partnerships with the Kerala Forest Department and conservation organizations will facilitate scaling to other states, potentially protecting thousands of elephants and saving hundreds of lives annually.

The vision extends beyond technology deployment to fostering  **peaceful coexistence**  where human development and wildlife conservation are complementary goals achieved through intelligent monitoring, early intervention, and data-driven decision making.
