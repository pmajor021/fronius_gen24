# fronius_gen24
LCD display for Fronius Gen24 and SmartMeter

Supported Product

Lilygo T-Display-S3


PlatformIO Quick Start (Recommended)

Install Visual Studio Code and Python

Search for the PlatformIO plugin in the VisualStudioCode extension and install it.

After the installation is complete, you need to restart VisualStudioCode

After restarting VisualStudioCode, select File in the upper left corner of VisualStudioCode -> Open Folder -> select the T-Display-S3 directory

Wait for the installation of third-party dependent libraries to complete

Click on the platformio.ini file, and in the platformio column

Uncomment one of the lines default_envs = xxxx to make sure only one line works

Click the (✔) symbol in the lower left corner to compile

Connect the board to the computer USB

Click (→) to upload firmware

Click (plug symbol) to monitor serial output


Arduino IDE Manual installation

Install Arduino IDE

Download fronius_gen24, move to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\)

Copy all folders in lib folder to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)

Open ArduinoIDE ,Tools ，Make your selection according to the table below

Arduino IDE Setting	Value

Board	ESP32S3 Dev Module

Port	Your port

USB CDC On Boot	Enable

CPU Frequency	240MHZ(WiFi)

Core Debug Level	None

USB DFU On Boot	Disable

Erase All Flash Before Sketch Upload	Disable

Events Run On	Core1

Flash Mode	QIO 80MHZ

Flash Size	16MB(128Mb)

Arduino Runs On	Core1

USB Firmware MSC On Boot	Disable

Partition Scheme	16M Flash(3M APP/9.9MB FATFS)

PSRAM	OPI PSRAM

Upload Mode	UART0/Hardware CDC

Upload Speed	921600

USB Mode	CDC and JTAG

Click upload , Wait for compilation and writing to complete
