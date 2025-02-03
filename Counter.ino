/**
 * Counter
 *
 * Version: 3.0
 *
 * Author: Aleksandr Kireev (https://bespredel.name)
 * Created: 12.04.2023
 * Updated: 12.04.2023
 */

#include <SPI.h>
#include <Ethernet.h>

// Ethernet
// deviceId - ОБЯЗАТЕЛЬНО ИЗМЕНИТЬ ДЛЯ НОВОЙ ПЛАТЫ. Можно использовать MAC адрес.
//          - MUST CHANGE FOR NEW BOARD. You can use the MAC address.
String deviceId = "DE-AD-BE-EF-FE-ED";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  //DE-AD-BE-EF-FE-ED
IPAddress ip(192, 168, 1, 10);                        //
EthernetServer server(80);

// HTTP request params
EthernetClient client;
char apiHost[20] = "192.168.1.20";
String apiUri = "/api";
String apiAppendParam = "&api_token=";

// Controls
int controlPin[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A4, A5 };

// Default state
int controlState[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };      // current state
int lastControlState[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // previous state
long controlCount[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };     // operation counter
int controlCountSize = sizeof(controlCount) / sizeof(controlCount[0]);
long allControlCount = 0;
long lastAllControlCount = 0;


void setup() {
  Serial.begin(9600);

  //pinMode(controlPin[0], INPUT_PULLUP);
  //pinMode(controlPin[1], INPUT_PULLUP);
  //pinMode(controlPin[2], INPUT_PULLUP);
  //pinMode(controlPin[3], INPUT_PULLUP);
  //pinMode(controlPin[4], INPUT_PULLUP);
  //pinMode(controlPin[5], INPUT_PULLUP);
  //pinMode(controlPin[6], INPUT_PULLUP);
  //pinMode(controlPin[7], INPUT_PULLUP);
  //pinMode(controlPin[8], INPUT_PULLUP);
  //pinMode(controlPin[9], INPUT_PULLUP);
  //pinMode(controlPin[10], INPUT_PULLUP);
  //pinMode(controlPin[11], INPUT_PULLUP);
  //pinMode(controlPin[12], INPUT_PULLUP);
  //pinMode(controlPin[13], INPUT_PULLUP);

  pinMode(controlPin[A0], INPUT_PULLUP);
  pinMode(controlPin[A1], INPUT_PULLUP);
  pinMode(controlPin[A2], INPUT_PULLUP);
  pinMode(controlPin[A3], INPUT_PULLUP);
  pinMode(controlPin[A4], INPUT_PULLUP);
  pinMode(controlPin[A5], INPUT_PULLUP);

  // Led notif
  pinMode(controlPin[8], OUTPUT);  // RED
  pinMode(controlPin[9], OUTPUT);  // GREEN

  // Ethernet
  initEthernet();

  ledStateCode(100, 8);
  ledStateCode(100, 9);
}


void loop() {
  // SCAN STATE
  //scanState(0);
  //scanState(1);
  //scanState(2);
  //scanState(3);
  //scanState(4);
  //scanState(5);
  //scanState(6);
  //scanState(7);
  //scanState(8);
  //scanState(9);
  //scanState(10);
  //scanState(11);
  //scanState(12);
  //scanState(12);
  //scanState(13);
  scanState(A0);
  scanState(A1);
  scanState(A2);
  scanState(A3);
  scanState(A4);
  //scanState(A5);

  // RESET STATE
  resetState(A5);

  // Check connect and reconnect
  if (linkStatus() != 1) {
    Serial.println("Reinit Ethernet...");
    ledStateCode(1, 8);
    initEthernet();
    ledStateCode(0, 8);
  }

  handleClient();

  //delay(50);
}

/**
 * Change state relay
 */
void scanState(int num) {
  if (checkState(num)) {
    ledStateCode(1, 9);
    ledStateCode(0, 9);
  }
}

/**
 * Check control state
 */
bool checkState(int num) {
  bool state = false;

  // Reading the state
  int reading = analogRead(controlPin[num]);
  controlState[num] = reading < 200;

  // If the state has changed
  if (controlState[num] != lastControlState[num] && controlState[num]) {
    // Increasing the local counter
    controlCount[num] = controlCount[num] + 1;
    allControlCount = allControlCount + 1;
    state = true;

    Serial.println("Ctrl [" + String(num) + "]: " + controlCount[num]);
  }

  lastControlState[num] = controlState[num];

  return state;
}

/**
 * Reset state
 */
void resetState(int num) {
  if (checkState(num)) {
    ledStateCode(1, 9);
    // Send reset comand
    sendData("/counter?c=reset&device_id=" + deviceId);

    resetCounter();

    delay(1000);
    ledStateCode(0, 9);
    Serial.println("Counters reset");
  }
}


/**
 * Reset state and counters
 */
void resetCounter() {
  // Reset counters
  memset(controlCount, 0, sizeof(controlCount));
  allControlCount = 0;
}

/**
 * Send status
 */
void sendData(String params) {
  if (client.connect(apiHost, 80)) {
    // Make a HTTP request:
    client.println("GET " + apiUri + params + apiAppendParam + " HTTP/1.1");
    client.println("Host: " + String(apiHost));
    client.println("Connection: close");
    client.println();
    //client.stop();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("Connection failed");
  }
}

/**
 * Server response
 */
void serverResponse(EthernetClient client) {
  // send a standard HTTP response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println();
  // print the current readings, in HTML format:
  client.print("{");
  client.print("\"deviceId\": \"" + String(deviceId) + "\"");
  client.print(",\"deviceIP\": \"" + String(ip) + "\"");
  client.print(",\"allControlCount\": " + String(allControlCount));

  client.print(",\"controlCount\": [");
  for (int i = 0; i < controlCountSize; i++) {
    client.print(String(controlCount[i]));
    if (i < controlCountSize - 1) {
      client.print(", ");
    }
  }
  client.print("]");

  client.print("}");
}

/**
 * Send status
 */
void handleClient() {
  EthernetClient client = server.available();

  if (client) {
    Serial.println("Got a client");

    bool currentLineIsBlank = true;
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && currentLineIsBlank) {
          serverResponse(client);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    if (request.indexOf("?") != -1) {  // check for the presence of GET parameters
      String params = request.substring(request.indexOf("?") + 1);
      if (params.indexOf("reset") != -1) {
        resetCounter();
      }
    }

    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
  }
}

/**
 * Init Ethernet connect
 */
void initEthernet() {
  //delay(1000);
  Serial.println("Initializing Ethernet");

  // If a connection with dynamic addressing has not been established, then
  // if (Ethernet.begin(mac) == 0) {
  //   Serial.println("Failed to configure Ethernet using DHCP");
  //   Ethernet.begin(mac, ip);
  //   if(Ethernet.localIP() == ip) {
  //     Serial.print("Success configure Ethernet using static IP: ");
  //     Serial.println(Ethernet.localIP());
  //   }
  // }
  // else {
  //   Serial.print("DHCP assigned IP ");
  //   Serial.println(Ethernet.localIP());
  // }

  Ethernet.begin(mac, ip);

  // Start the Ethernet connection and the server:
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found. Can't run without hardware. :(");
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  if (Ethernet.localIP() == ip) {
    Serial.print("Success configure Ethernet using static IP: ");
    Serial.println(Ethernet.localIP());
  }

  // Start the server
  server.begin();

  // Give Ethernet 1 second to initialize
  delay(1000);
}

/**
 * Check link status
 */
int linkStatus() {
  auto link = Ethernet.linkStatus();
  int statusLink = -1;
  //Serial.print("Link status: ");
  switch (link) {
    case Unknown:
      //Serial.println("Unknown");
      //statusLink = -1;
      break;

    case LinkOFF:
      //Serial.println("OFF");
      statusLink = 0;
      break;

    case LinkON:
      //Serial.println("ON");
      statusLink = 1;
      break;
  }

  return statusLink;
}

/**
 * Led status code
 */
void ledStateCode(int code, int pin) {
  switch (code) {
    case 0:
      analogWrite(pin, 0);
      break;

    case 1:
      analogWrite(pin, 255);
      break;

    case 100:
      analogWrite(pin, 255);
      delay(100);
      analogWrite(pin, 0);
      delay(100);
      analogWrite(pin, 255);
      delay(100);
      analogWrite(pin, 0);
      delay(100);
      analogWrite(pin, 255);
      delay(100);
      analogWrite(pin, 0);
      delay(100);
      analogWrite(pin, 255);
      delay(3000);
      analogWrite(pin, 0);
      break;
  }
}
