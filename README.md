# UART Point-to-Point Communication Project

A wireless UART bridge implementation for ESP32-C3 microcontrollers that enables point-to-point serial communication over WiFi networks.

## 🚀 Features

- **Dual Mode Operation**: Supports both Access Point (AP) and Station (STA) modes
- **UART Bridge**: Seamless bidirectional data transmission between UART and WiFi
- **Multiple Protocols**: UDP and TCP communication support
- **Mode Switching**: Runtime mode switching via boot button
- **Visual Feedback**: LED indicators for connection status and data transmission
- **Multi-Board Support**: Compatible with different ESP32-C3 variants
- **Low Power**: Optimized for power efficiency with configurable CPU frequency

## 📋 Supported Hardware

- **ESP32-C3**: Standard ESP32-C3 development boards
- **ESP32-C3 SuperMini**: Compact ESP32-C3 boards with USB-C
- **ESP8285**: (Work in progress) ESP8285 boards with AT command support

## 🔧 Hardware Configuration

### ESP32-C3 Standard
- **UART TX**: GPIO 2
- **UART RX**: GPIO 3
- **Status LED**: GPIO 12
- **Data LED**: GPIO 13
- **Boot Button**: GPIO 9
- **Baud Rate**: 115200

### ESP32-C3 SuperMini
- **UART TX**: GPIO 0
- **UART RX**: GPIO 1
- **Status LED**: GPIO 8
- **Boot Button**: GPIO 9
- **Baud Rate**: 115200

## 📡 Network Configuration

### Default WiFi Settings

**ESP32-C3 Standard:**
- **AP SSID**: `PN532Killer-AP`
- **Station SSID**: `PN532Killer-AP`
- **Password**: (None)
- **UDP Port**: 18888

**ESP32-C3 SuperMini:**
- **AP SSID**: `Proxmark3X`
- **Station SSID**: `Proxmark3X`
- **Password**: `mtoolstec`
- **UDP Port**: 19999
- **TCP Port**: 18888

## 🎯 Operation Modes

### Access Point (AP) Mode
- Creates a WiFi hotspot
- Waits for client connections
- Forwards UART data to connected clients via UDP/TCP
- LED Status: Solid ON when client connected, 2-second blink when no clients

### Station (STA) Mode
- Connects to existing WiFi network
- Communicates with gateway/AP device
- Forwards UART data via UDP
- LED Status: Solid ON when connected, fast blink (200ms) when disconnected

## 🔄 Mode Switching

Press and hold the **BOOT button** to switch between AP and STA modes:
- Short press toggles between modes
- Debounce protection prevents accidental switching
- Visual confirmation via LED patterns

## 💡 LED Indicators

### Status LED Patterns
- **Solid ON**: Device connected (AP mode with client or STA mode connected)
- **Slow Blink (2s)**: AP mode, no clients connected
- **Medium Blink (1s)**: STA mode, attempting connection
- **Fast Blink (200ms)**: STA mode, connection failed

### Data LED (ESP32-C3 Standard only)
- **Blink**: Data transmission activity
- **OFF**: No data transmission

## 🛠️ Installation & Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) IDE or extension
- ESP32-C3 development board
- USB cable for programming

### Build & Upload

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd uart-p2p
   ```

2. **Configure target board in platformio.ini:**
   - For ESP32-C3: Use `[env:esp32c3]`
   - For ESP32-C3 SuperMini: Use `[env:esp32c3supermini]`

3. **Build and upload:**
   ```bash
   pio run -t upload
   ```

4. **Monitor serial output:**
   ```bash
   pio device monitor
   ```

## 📊 Data Flow

```
UART Device ←→ ESP32-C3 ←→ WiFi ←→ ESP32-C3 ←→ UART Device
```

1. **UART to WiFi**: Data received on UART is packetized and sent via UDP/TCP
2. **WiFi to UART**: Network packets are received and forwarded to UART
3. **Bidirectional**: Full-duplex communication in both directions
4. **Real-time**: Minimal latency for time-sensitive applications

## 🔧 Configuration

### Customizing WiFi Settings
Edit the following constants in the source files:

```cpp
// ESP32-C3 Standard (esp32c3-ap.cpp)
const char *apSSID = "PN532Killer-AP";
const char *staSSID = "PN532Killer-AP";
const char *staPassword = "";

// ESP32-C3 SuperMini (esp32c3supermini.ino)
const char *apSSID = "Proxmark3X";
const char *staSSID = "Proxmark3X";
const char *staPassword = "mtoolstec";
```

### UART Configuration
```cpp
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 2  // Adjust for your board
#define UART_RX_PIN 3  // Adjust for your board
```

## 🧪 Testing & Debugging

### Serial Monitor Output
The device provides detailed logging including:
- Mode transitions
- WiFi connection status
- Data transmission logs with timestamps
- Error messages and diagnostics

### Example Output
```
[Mode] AP Hotspot Mode
AP Name: PN532Killer-AP
AP IP: 192.168.4.1
UDP server started, waiting for client message...
Station IP: 192.168.4.2
[1234 ms] [AP] UART: 48656C6C6F
```

## 🔗 Use Cases

- **IoT Device Communication**: Connect microcontrollers wirelessly
- **Serial Device Extension**: Extend UART range beyond cable limitations
- **Data Logging**: Wireless data collection from remote sensors
- **Debugging**: Remote debugging of embedded systems
- **Industrial Automation**: Wireless communication in industrial environments

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is open source. Please check the license file for details.

## 🔍 Troubleshooting

### Common Issues

**WiFi Connection Failed:**
- Verify SSID and password settings
- Check WiFi network availability
- Ensure ESP32-C3 is within range

**No Data Transmission:**
- Verify UART wiring and baud rate
- Check LED indicators for connection status
- Monitor serial output for error messages

**Mode Switch Not Working:**
- Ensure proper button debouncing
- Check GPIO pin configuration
- Verify button wiring

### Debug Tips
- Use serial monitor to view real-time status
- Check LED patterns for connection status
- Verify network configuration with ping tests
- Test UART connectivity with loopback

## 📞 Support

For issues, questions, or contributions, please:
- Check the troubleshooting section above
- Review existing issues in the repository
- Create a new issue with detailed description and logs