![GitHub release (with filter)](https://img.shields.io/github/v/release/pmajor021/fronius_gen24?color=%23008000)

# fronius_gen24
Information display for Fronius Gen24 Inverter and SmartMeter

![fronius_gen24 on Lilygo T-Display-3](https://github.com/user-attachments/assets/ace65480-dd41-408c-989c-845e8e2832ae)

GR - Grid (red - from grid, green - to grid), PV - Power from inverter (Batt. + Solar) / Power from solar, LD - Load, 
B - Battery SoC, L1-3 - Power on each phase (red - from grid, green - to grid)

## Supported Product

Lilygo T-Display-S3
[https://www.lilygo.cc/products/t-display-s3?variant=42589373268149](https://lilygo.cc/products/t-display-s3?variant=42585826590901)

## PlatformIO Quick Start (Recommended)
1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
2. Search for the `PlatformIO` plugin in the `VisualStudioCode` extension and install it.
3. After the installation is complete, you need to restart `VisualStudioCode`
4. After restarting `VisualStudioCode`, select `File` in the upper left corner of `VisualStudioCode` -> `Open Folder` -> select the `fronius_gen24` directory
5. Wait for the installation of third-party dependent libraries to complete
6. Click the (✔) symbol in the lower left corner to compile
7. Connect the board to the computer USB
8. Click (→) to upload firmware
9. Click (plug symbol) to monitor serial output


## Arduino IDE Manual installation

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Open the Arduino IDE.
3. Click `File` menu -> `Preferences` -> `Additional Development` -> `Additional Board Manager URLs` -> Enter the URL in the input box: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` and click OK.
4. Click `Tools` menu -> `Board: "...."` -> `Boards Manager...`, install the `esp32` package by `Espressif Systems` **Please pay attention to use version 2.0.17**
5. Download `fronius_gen24` , move to Arduino folder (e.g. C:\Users\YourName\Documents\Arduino\)
6. Copy all folders in [lib folder](./lib/) to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)
7. Enter the downloaded `software` directory
8. Open ArduinoIDE ,`Tools` ，Make your selection according to the table below
    | Arduino IDE Setting                  | Value                             |
    | ------------------------------------ | --------------------------------- |
    | Board                                | **ESP32S3 Dev Module**            |
    | Port                                 | Your port                         |
    | USB CDC On Boot                      | Enable                            |
    | CPU Frequency                        | 240MHZ(WiFi)                      |
    | Core Debug Level                     | None                              |
    | USB DFU On Boot                      | Disable                           |
    | Erase All Flash Before Sketch Upload | Disable                           |
    | Events Run On                        | Core1                             |
    | Flash Mode                           | QIO 80MHZ                         |
    | Flash Size                           | **16MB(128Mb)**                   |
    | Arduino Runs On                      | Core1                             |
    | USB Firmware MSC On Boot             | Disable                           |
    | Partition Scheme                     | **16M Flash(3M APP/9.9MB FATFS)** |
    | PSRAM                                | **OPI PSRAM**                     |
    | Upload Mode                          | **UART0/Hardware CDC**            |
    | Upload Speed                         | 115200                            |
    | USB Mode                             | **CDC and JTAG**                  |
    * The options in bold are required, others are selected according to actual conditions.

9. Click `upload` , Wait for compilation and writing to complete

