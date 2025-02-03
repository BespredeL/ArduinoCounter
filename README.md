# Counter Project

## Description

This project implements a counter system using an Arduino board with an Ethernet shield. The counter increments based on inputs from pins, and the state is transmitted over the network to a server via HTTP requests. The system supports resetting the counter via HTTP, and it can track the state of up to 20 inputs. It is designed to handle control operations through both physical input (e.g., buttons or sensors) and remote commands.

---

## Features

- **Ethernet Connectivity**: Uses an Ethernet shield to connect the Arduino to a local network.
- **Control Inputs**: Tracks up to 20 control pins (A0 to A5 in this version).
- **State Handling**: Tracks the state of each control input and counts its activations.
- **Reset Functionality**: Allows resetting the counters through a web request.
- **Web Interface**: Provides a basic web API to get the current counter status and reset the counters remotely.
- **LED Notifications**: Uses LEDs for indicating connection and state changes.

---

## Hardware Requirements

- **Arduino Board** (e.g., Arduino Uno)
- **Ethernet Shield** (compatible with Arduino)
- **LEDs** for visual notifications
- **Buttons or sensors** connected to pins A0 to A5 (or more if modified)
- **Ethernet Cable** (for wired network connection)

---

## Software Requirements

- **Arduino IDE** (with installed libraries for `SPI` and `Ethernet`)
- Basic understanding of Arduino programming and networking

---

## Setup

### 1. Modify Device Settings

The device ID is required for the board to communicate with your server. Change it to something unique (you can use a MAC address).

```cpp
String deviceId = "DE-AD-BE-EF-FE-ED";  // Update this
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Update if needed
```

Additionally, adjust the static IP address and server API settings:

```cpp
IPAddress ip(192, 168, 1, 10); // Set your desired static IP
char apiHost[20] = "192.168.1.20";  // Update to the address of your API server
String apiUri = "/api";  // Endpoint for API
String apiAppendParam = "&api_token=";  // Add API token if needed
```

### 2. Wiring

- **Control Pins**: Connect buttons or sensors to the pins A0 to A5. The code handles these pins as input pins with pull-up resistors.
- **LED Pins**: Pins 8 and 9 are used to control the notification LEDs (Red and Green).

### 3. Upload the Code

Upload the code to your Arduino board via the Arduino IDE. Once uploaded, the system will initialize the Ethernet connection and start listening for control inputs.

---

## Web API

### Get Counter Status

Send a `GET` request to the Arduino's IP to receive the current status:

```
GET /api?deviceId=DE-AD-BE-EF-FE-ED
```

Response (JSON format):

```json
{
  "deviceId": "DE-AD-BE-EF-FE-ED",
  "deviceIP": "192.168.1.10",
  "allControlCount": 25,
  "controlCount": [ 5, 0, 8, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]
}
```

### Reset Counter

You can reset the counters via the following URL:

```
GET /api?reset&device_id=DE-AD-BE-EF-FE-ED
```

This will reset all counters and provide a confirmation message.

---

## LED Indicators

- **Red LED (Pin 8)**:
  - Blinks when the Ethernet connection is being initialized or reconnected.
- **Green LED (Pin 9)**:
  - Turns on when a button is pressed or a sensor is activated, signaling state change.

---

## Troubleshooting

1. **Ethernet Not Connecting**:
   - Ensure the Ethernet shield is correctly connected and the cable is functional.
   - Double-check the static IP address and ensure it does not conflict with other devices on your network.
   
2. **Control Inputs Not Detecting**:
   - Check the wiring for buttons or sensors.
   - Verify that the pins are configured correctly in the code.

3. **No Response from API**:
   - Ensure the server at `apiHost` is up and reachable.
   - Verify the API endpoint is correct and configured on the server.

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Let me know if you need any adjustments or more details in the `README`!
