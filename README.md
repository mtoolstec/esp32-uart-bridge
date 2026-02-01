# ESP32-C3 UART Bridge for Proxmark3 X

A wireless UART bridge implementation for ESP32-C3 SuperMini, designed for seamless point-to-point serial communication over WiFi using TCP. This project is specifically configured for the **Proxmark3 X** expansion board.

## 🔗 Related Project

- [ESP32 Expansion Board for Proxmark3 X](https://shop.mtoolstec.com/product/esp32-expansion-board-for-proxmark3-x)

## 🚀 Key Features

- **Wireless Bridge**: Transparent UART data transmission via WiFi.
- **TCP Server**: Reliable TCP communication on port **18888**.
- **AP Mode Default**: Creates a WiFi hotspot automatically.
- **Low Latency**: Optimized for performance with Nagle's algorithm disabled.
- **Status Indication**: LED limits for connection and mode status.

## 📋 Hardware Configuration

### ESP32-C3 SuperMini
- **UART TX**: GPIO 0
- **UART RX**: GPIO 1
- **Status LED**: GPIO 8
- **Boot Button**: GPIO 9
- **Baud Rate**: 115200

## 📡 Network Settings

**Access Point (AP) Mode:**
- **SSID**: `Proxmark3X`
- **Password**: `mtoolstec`
- **Gateway IP**: `192.168.4.1`
- **TCP Port**: `18888`

## 🎯 Operation Guides

### 1. Default Operation (AP Mode)
1. Power on the device. The LED will blink slowly (2s interval) indicating it is waiting for a connection.
2. Connect your computer or mobile device to the WiFi network **Proxmark3X** using password **mtoolstec**.
3. Open a TCP client (e.g., Netcat, Proxmark3 client, or Serial Terminal) and connect to **192.168.4.1** on port **18888**.
4. Once connected, the LED will turn **Solid ON**. Data sent to the TCP socket is forwarded to the UART, and UART data is sent back to the TCP client.

### 2. Mode Switching
- Press and hold the **BOOT button** (GPIO 9) to toggle between AP Mode and Station Mode.
- *Note: The current firmware is primarily configured for AP Mode. Station Mode is set to connect to the same SSID ('Proxmark3X') by default.*

## 💡 LED Indicators

- **Solid ON**: Client connected (AP Mode) or WiFi connected (Station Mode).
- **Slow Blink (2s)**: AP Mode, waiting for client.
- **Fast Blink (200ms)**: Station Mode, disconnected or connecting.
- **Toggle Blink**: Occurs on mode switch.

## 🛠️ Build & Upload

1. Ensure **PlatformIO** is installed.
2. Select the **esp32c3supermini** environment.
3. Build and upload:
   ```bash
   pio run -e esp32c3supermini -t upload
   ```

## 📄 License

This project is open source.
