#ifdef ARDUINO_RP2040W
#include <Arduino.h>
#include <HardwareSerial.h>
const unsigned int apPort = 8888;
const unsigned int staPort = 8889;

const char *apIpAddress = "192.168.5.1";
const char *staIpAddress = "192.168.5.2";
const char *subnetMask = "255.255.255.0";
char *targetIpAddress =  "192.168.5.1";

#define UART_BAUD_RATE 115200
#define UART_TX_PIN 4     // UART TX 引脚
#define UART_RX_PIN 5     // UART RX 引脚

void setup()
{
    // Serial1.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    Serial.begin(115200);
    
}

void loop()
{
    delay(1000);
    Serial.println("ESP8285 AT Command Test");
}

// bool esp_sendCMD(const char *cmd, const char *ack, uint32_t timeout)
// {
//     espSerial.write(cmd);
//     espSerial.write("\r\n");
//     uint32_t start = millis();
//     while ((millis() - start) < timeout)
//     {
//         if (espSerial.available())
//         {
//             String response = espSerial.readString();
//             Serial.println(response);
//             if (response.indexOf(ack) >= 0)
//             {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// bool setApMode(const char *ssid, const char *pwd)
// {
//     // Set CW mode to 2 (AP mode)
//     if (!esp_sendCMD("AT+CWMODE_CUR=2", "OK", 2000))
//     {
//         Serial.println("设置 CW 模式失败");
//         return false;
//     }

//     // Set AP mode with SSID and password
//     char cmd[128];
//     snprintf(cmd, sizeof(cmd), "AT+CWSAP_CUR=\"%s\",\"%s\",1,3", ssid, pwd);
//     if (esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("设置为 AP 模式成功");
//         return true;
//     }
//     else
//     {
//         Serial.println("设置为 AP 模式失败");
//         return false;
//     }

//     // Set AP IP address
//     snprintf(cmd, sizeof(cmd), "AT+CIPAP_CUR=\"%s\",\"%s\",\"%s\"", apIpAddress, apIpAddress, subnetMask);
//     if (!esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("设置 AP IP 地址失败");
//         return false;
//     }
//     Serial.println("设置 AP IP 地址成功");
//     targetIpAddress = (char *)apIpAddress;

//     // CIPSTART set UDP with target IP, port and local port
//     snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d", targetIpAddress, apPort, staPort);
//     if (!esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("设置 UDP 连接失败");
//         return false;
//     }
//     Serial.println("设置 UDP 连接成功");
//     // Enable transparent transmission mode
//     if (!esp_sendCMD("AT+CIPMODE=1", "OK", 2000))
//     {
//         Serial.println("开启透传模式失败");
//         return false;
//     }
//     Serial.println("开启透传模式成功");
//     // Start sending data
//     if (!esp_sendCMD("AT+CIPSEND", ">", 2000))
//     {
//         Serial.println("准备发送数据失败");
//         return false;
//     }
//     Serial.println("准备发送数据成功");
//     return true;
// }

// bool setStaMode(const char *ssid, const char *pwd)
// {
//     // Set CW mode to 1 (STA mode)
//     if (!esp_sendCMD("AT+CWMODE_CUR=1", "OK", 2000))
//     {
//         Serial.println("设置 CW 模式失败");
//         return false;
//     }

//     // Connect to AP
//     char cmd[128];
//     snprintf(cmd, sizeof(cmd), "AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, pwd);
//     if (esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("连接到 AP 成功");
//         return true;
//     }
//     else
//     {
//         Serial.println("连接到 AP 失败");
//         return false;
//     }
//     // Set STA IP address
//     snprintf(cmd, sizeof(cmd), "AT+CIPSTA_CUR=\"%s\",\"%s\",\"%s\"", staIpAddress, apIpAddress, subnetMask);
//     if (!esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("设置 STA IP 地址失败");
//         return false;
//     }
//     Serial.println("设置 STA IP 地址成功");
//     targetIpAddress = (char *)staIpAddress;

//     // CIPSTART set UDP with target IP, port and local port
//     snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d", targetIpAddress, staPort, apPort);
//     if (!esp_sendCMD(cmd, "OK", 2000))
//     {
//         Serial.println("设置 UDP 连接失败");
//         return false;
//     }
//     Serial.println("设置 UDP 连接成功");
//     // Enable transparent transmission mode
//     if (!esp_sendCMD("AT+CIPMODE=1", "OK", 2000))
//     {
//         Serial.println("开启透传模式失败");
//         return false;
//     }
//     Serial.println("开启透传模式成功");
//     // Start sending data
//     if (!esp_sendCMD("AT+CIPSEND", ">", 2000))
//     {
//         Serial.println("准备发送数据失败");
//         return false;
//     }
//     Serial.println("准备发送数据成功");
//     return true;
// }

#endif