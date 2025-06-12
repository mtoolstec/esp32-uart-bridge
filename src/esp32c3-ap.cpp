#ifdef ARDUINO_ESP32C3
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>

const char *apSSID = "PN532Killer-AP";
const char *staSSID = "PN532Killer-AP";
const char *staPassword = "";

#define BOOT_BUTTON_PIN 9 // BOOT button pin for ESP32-C3
#define LED_PIN 12        // Onboard LED pin for ESP32-C3 (adjust according to your board)
#define LED_DATA_PIN 13   // LED data pin
#define DEBOUNCE_DELAY 50 // Debounce delay (ms)
#define UART_TX_PIN 2     // UART TX pin
#define UART_RX_PIN 3     // UART RX pin
#define UART_BAUD_RATE 115200

unsigned long bootTime;
bool apMode = true;
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long lastLedToggle = 0;
bool ledState = false;
bool ledShouldBlinkFast = false;
bool ledShouldBeOn = false;

unsigned long lastDataBlinkTime = 0;   // Last data LED blink time
unsigned long dataBlinkInterval = 100; // Data LED blink interval, initial 100ms

WiFiUDP udp;                       // Create a WiFiUDP object
const unsigned int udpPort = 18888; // UDP communication port

const unsigned long testInterval = 2000;

IPAddress targetIp; // Store the IP address of the Station device

void setup()
{
  Serial.begin(115200);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // BOOT button input pull-up
  pinMode(LED_PIN, OUTPUT);               // LED pin output
  pinMode(LED_DATA_PIN, OUTPUT);          // Initialize data LED pin
  digitalWrite(LED_DATA_PIN, LOW);        // Ensure LED is initially off

  bootTime = millis();

  checkBootButton();

  Serial1.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  if (apMode)
  {
    startAPMode();
    udp.begin(udpPort);
    Serial.println("UDP server started, waiting for client message...");
  }
  else
  {
    startSTAMode();
  }
}

void loop()
{
  // static unsigned long lastLogTime = 0;

  // if (millis() - lastLogTime >= 60000)
  // {
  //   printNetworkStatus();
  //   lastLogTime = millis();
  // }

  checkBootButton();

  // If in STA mode and not connected, try to reconnect
  if (!apMode && !targetIp)
  {
    connectToAP();
  }

  // If in AP mode and a device is connected, record the first connected device's IP address
  if (apMode && !targetIp)
  {
    getStationIp();
  }

  // Update LED status
  updateLedStatus();

  // Update data LED status (ensure continuous calling to handle blink logic)
  // updateDataLedStatus(false);

  // Handle UART data passthrough
  handleUARTToUDP(); // Handle UART to UDP data transmission
  handleUDPToUART(); // Handle UDP to UART data transmission
}

void ledBlinkOnce()
{
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

void ledAlwaysOn()
{
  digitalWrite(LED_PIN, HIGH);
}

void ledAlwaysOff()
{
  digitalWrite(LED_PIN, LOW);
}

void updateLedStatus()
{
  // LED always on if a device is connected or STA mode is connected
  if (apMode)
  {
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    if (stationList.num > 0)
    {
      ledAlwaysOn();
      return;
    }
  }
  else if (WiFi.status() == WL_CONNECTED)
  {
    ledAlwaysOn();
    return;
  }

  // LED fast blink when STA mode is not connected
  if (!apMode && WiFi.status() != WL_CONNECTED)
  {
    unsigned long interval = 200; // Fast blink period 200ms
    if (millis() - lastLedToggle >= interval)
    {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      lastLedToggle = millis();
    }
    return;
  }

  // Set blink period according to current mode
  unsigned long interval = apMode ? 2000 : 1000; // AP mode 2s period, STA mode 1s period
  if (millis() - lastLedToggle >= interval)
  {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    lastLedToggle = millis();
  }
}

void checkBootButton()
{
  bool currentState = digitalRead(BOOT_BUTTON_PIN);

  // Debounce handling
  if (currentState != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
  {
    if (currentState == LOW && !buttonPressed)
    {
      buttonPressed = true;
      toggleMode();
    }
    else if (currentState == HIGH)
    {
      buttonPressed = false;
    }
  }

  lastButtonState = currentState;
}

void toggleMode()
{
  Serial.println("\nBOOT button pressed, switching mode...");

  if (apMode)
  {
    WiFi.softAPdisconnect(true);
    apMode = false;
    startSTAMode();
  }
  else
  {
    WiFi.disconnect();
    apMode = true;
    startAPMode();
    udp.begin(udpPort); // Listen on port in AP mode
    Serial.println("UDP server started, waiting for client message...");
  }
  lastLedToggle = millis();
}

void startAPMode()
{
  Serial.println("\n[Mode] AP Hotspot Mode");

  // Set maximum connected devices to 1
  WiFi.softAP(apSSID, nullptr, 1);

  Serial.print("AP Name: ");
  Serial.println(apSSID);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void startSTAMode()
{
  Serial.println("\n[Mode] Station Mode");
  WiFi.mode(WIFI_STA);
  connectToAP();
}

void connectToAP()
{
  Serial.println("Trying to connect to AP...");

  WiFi.begin(staSSID, staPassword);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    targetIp = WiFi.gatewayIP();
  }
  else
  {
    Serial.println("\nConnection failed, please check if the AP is available");
  }
}

void printNetworkStatus()
{
  unsigned long uptime = (millis() - bootTime) / 1000;
  Serial.print("\n[Status] Uptime: ");
  Serial.print(uptime);
  Serial.println(" seconds");

  if (apMode)
  {
    Serial.println("Current mode: AP (Hotspot)");
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);

    if (stationList.num > 0)
    {
      Serial.print("Connected devices: ");
      Serial.println(stationList.num);

      for (int i = 0; i < stationList.num; i++)
      {
        wifi_sta_info_t station = stationList.sta[i];
        Serial.print("Device ");
        Serial.print(i + 1);
        Serial.print(" MAC: ");
        for (int j = 0; j < 6; j++)
        {
          if (station.mac[j] < 16)
            Serial.print("0");
          Serial.print(station.mac[j], HEX);
          if (j < 5)
            Serial.print(":");
        }
        Serial.println();

        // Get device IP address
        tcpip_adapter_sta_list_t ipList;
        tcpip_adapter_get_sta_list(&stationList, &ipList);
        Serial.print("Device ");
        Serial.print(i + 1);
        Serial.print(" IP: ");
        Serial.print((ipList.sta[i].ip.addr >> 0) & 0xFF);
        Serial.print(".");
        Serial.print((ipList.sta[i].ip.addr >> 8) & 0xFF);
        Serial.print(".");
        Serial.print((ipList.sta[i].ip.addr >> 16) & 0xFF);
        Serial.print(".");
        Serial.println((ipList.sta[i].ip.addr >> 24) & 0xFF);
      }
    }
    else
    {
      Serial.println("No devices connected");
    }
  }
  else
  {
    Serial.println("Current mode: STA (Client)");
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("Connected to: ");
      Serial.println(WiFi.SSID());
      Serial.print("Signal strength: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    }
    else
    {
      Serial.println("Not connected to any AP");
    }
  }
}

void printConnectedDevices()
{
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList);

  for (int i = 0; i < stationList.num; i++)
  {
    wifi_sta_info_t station = stationList.sta[i];
    Serial.print("New device connected - MAC: ");
    for (int j = 0; j < 6; j++)
    {
      if (station.mac[j] < 16)
        Serial.print("0");
      Serial.print(station.mac[j], HEX);
      if (j < 5)
        Serial.print(":");
    }
    Serial.println();

    // Get device IP address
    tcpip_adapter_sta_list_t ipList;
    tcpip_adapter_get_sta_list(&stationList, &ipList);
    Serial.print("Device IP: ");
    Serial.print((ipList.sta[i].ip.addr >> 0) & 0xFF);
    Serial.print(".");
    Serial.print((ipList.sta[i].ip.addr >> 8) & 0xFF);
    Serial.print(".");
    Serial.print((ipList.sta[i].ip.addr >> 16) & 0xFF);
    Serial.print(".");
    Serial.println((ipList.sta[i].ip.addr >> 24) & 0xFF);
  }
}

void updateDataLedStatus(bool dataTransferred)
{
  // Only update LED blink status when data is transferred
  if (dataTransferred && millis() - lastDataBlinkTime >= dataBlinkInterval)
  {
    digitalWrite(LED_DATA_PIN, !digitalRead(LED_DATA_PIN)); // Toggle LED state
    lastDataBlinkTime = millis();
  }
  else if (!dataTransferred)
  {
    digitalWrite(LED_DATA_PIN, LOW); // Turn off LED when no data is transferred
  }
}

void handleUARTToUDP()
{
  static char buffer[256]; // Buffer array for storing data received from UART
  static int length = 0;   // Current data length in buffer

  // Read as much as possible from UART_RX_PIN
  while (Serial1.available() && length < sizeof(buffer))
  {
    buffer[length++] = Serial1.read();
  }

  // If there is data and target IP exists, send immediately
  if (length > 0 && targetIp)
  {
    udp.beginPacket(targetIp, udpPort);
    udp.write((uint8_t *)buffer, length);
    udp.endPacket();

    unsigned long timestamp = millis(); // Get current timestamp
    if (apMode)
    {
      Serial.printf("[%lu ms] [AP] UART: ", timestamp);
    }
    else
    {
      Serial.printf("[%lu ms] [STA] UART: ", timestamp);
    }
    for (int i = 0; i < length; i++)
    {
      Serial.printf("%02X", buffer[i]);
    }
    Serial.println();

    length = 0; // Clear buffer
  }
}

void handleUDPToUART()
{
  static uint8_t buffer[256]; // Buffer for storing UDP data
  int length = udp.parsePacket(); // Get UDP packet length

  if (length > 0)
  {
    // Limit read length to prevent overflow
    length = udp.read(buffer, sizeof(buffer));

    // Batch write to UART
    Serial1.write(buffer, length);
  }
}

void getStationIp()
{
  wifi_sta_list_t stationList;
  tcpip_adapter_sta_list_t ipList;

  esp_wifi_ap_get_sta_list(&stationList);
  tcpip_adapter_get_sta_list(&stationList, &ipList);

  if (stationList.num > 0)
  {
    if (ipList.sta[0].ip.addr != 0)
    {
      targetIp = IPAddress(ipList.sta[0].ip.addr);
      Serial.print("Station IP: ");
      Serial.println(targetIp);
    }
  }
}

#endif