#ifdef ARDUINO_ESP32C3SUPERMINI
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include "esp_system.h"
#include <esp_netif.h>

const char *apSSID = "Proxmark3X";
const char *staSSID = "Proxmark3X";
const char *staPassword = "mtoolstec";

#define BOOT_BUTTON_PIN 9 // BOOT button pin for ESP32-C3
#define LED_PIN 8        // Onboard LED pin for ESP32-C3 (adjust according to your board)
#define DEBOUNCE_DELAY 50 // Debounce delay (ms)
#define UART_TX_PIN 0     // UART TX pin
#define UART_RX_PIN 1     // UART RX pin
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
const unsigned int udpPort = 19999; // UDP communication port
const unsigned int tcpPort = 18888; // TCP communication port
WiFiClient tcpClient;
WiFiServer tcpServer;

IPAddress targetIp; // Store the IP address of the Station device

void setup()
{
  setCpuFrequencyMhz(80); 
  Serial.begin(115200);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // BOOT button input pull-up
  pinMode(LED_PIN, OUTPUT);               // LED pin output

  bootTime = millis();

  Serial1.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  if (apMode)
  {
    startAPMode();
    Serial.println("TCP server started, waiting for client connection...");
    tcpServer.begin(tcpPort);
  }
  else
  {
    startStationMode();
  }

  esp_wifi_set_max_tx_power(5);
}

void loop()
{
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

  if (apMode) {
    if (!tcpClient || !tcpClient.connected()) {
      tcpClient = tcpServer.accept();
    }
  }

  updateLedStatus();

  if(apMode){
    handleTcpToUart();
    handelUartToTcp();
  }
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
    startStationMode();
  }
  else
  {
    WiFi.disconnect();
    apMode = true;
    startAPMode();
    udp.begin(udpPort);
    Serial.println("UDP server started, waiting for client message...");
  }
  lastLedToggle = millis();
}

void startAPMode()
{
  Serial.println("\n[Mode] AP Hotspot Mode");

  WiFi.softAP(apSSID, staPassword, 6);
  Serial.print("AP Name: ");
  Serial.println(apSSID);
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.softAPIP());
}

void startStationMode()
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
  static uint8_t buffer[256]; 
  int length = udp.parsePacket();

  if (length > 0)
  {
    length = udp.read(buffer, sizeof(buffer));

    Serial1.write(buffer, length);
  }
}

void getStationIp()
{
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList);

  if (stationList.num > 0)
  {
    wifi_sta_info_t station = stationList.sta[0];
    Serial.print("Station MAC: ");
    for (int j = 0; j < 6; j++)
    {
      if (station.mac[j] < 16) Serial.print("0");
      Serial.print(station.mac[j], HEX);
      if (j < 5) Serial.print(":");
    }
    Serial.println();
    // Cannot get IP directly, if you need IP, let STA report actively
  }
}

void handelUartToTcp()
{
  static char buffer[256];
  static int length = 0;

  while (Serial1.available() && length < sizeof(buffer))
  {
    buffer[length++] = Serial1.read();
  }

  // If there is data and target IP exists, send immediately
  if (length > 0 && targetIp)
  {
    tcpClient.write((uint8_t *)buffer, length);
    tcpClient.flush();

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

void handleTcpToUart()
{
  static uint8_t buffer[256]; // Buffer for storing TCP data
  int length = tcpClient.available(); // Get TCP packet length

  if (length > 0)
  {
    // Limit read length to prevent overflow
    length = tcpClient.read(buffer, sizeof(buffer));

    // Batch write to UART
    Serial1.write(buffer, length);
  }
}


#endif