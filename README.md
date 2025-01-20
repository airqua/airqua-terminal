# AirQua Terminal firmware

## About
Firmware that allows [Wio Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html) devices to send data to the [AirQua](https://airqua.uk/) server.

## Installation
### Using AirQua Terminal Flasher (recommended)
Guide is available in [airqua-terminal-flasher](https://github.com/airqua/airqua-terminal-flasher) repository

### Manual (advanced)
#### Flash RTL8720D firmware
You only need to do this once - after you purchase the device
1. In command prompt clone the flasher repository and navigate into the resulting directory
   ```shell
   git clone https://github.com/Seeed-Studio/ambd_flash_tool
   cd ambd_flash_tool
   ```
2. Erase the factory firmware

   **Windows:**
   ```shell
   .\ambd_flash_tool.exe erase
   ```
   **Mac/Linux:**
   ```shell
   python3 ambd_flash_tool.py erase
   ```

3. Flash the latest firmware

   **Windows:**
   ```shell
   .\ambd_flash_tool.exe flash
   ```
   **Mac/Linux:**
   ```shell
   python3 ambd_flash_tool.py flash
   ```

#### Flash AirQua firmware
1. Clone this repository using git
   ```shell
   git clone https://github.com/airqua/airqua-terminal
   ```
2. Download Arduino IDE from [the official website](https://www.arduino.cc/en/software)
3. Go to **Preferences** -> **Settings** -> **Additional boards manager URLs** and add the following URL:
    ```
    https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
    ```
4. Connect your Wio Terminal, select **Seeeduino Wio Terminal** in **Boards** dropdown
5. When prompted, agree to automatic installation of **Seeed SAMD Boards** package
6. Copy everything from `libraries` folder into `%YOUR_SKETCHBOOK_LOCATION%/libraries`, where `YOUR_SKETCHBOOK_LOCATION`
   is Arduino's path, that can be checked in Settings
7. Open `WioTerminal.ino` using Arduino IDE
8. Insert your token and device id into appropriate variables
9. Click **Upload**