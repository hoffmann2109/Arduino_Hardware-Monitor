# Arduino Hardware Monitor

This project displays your PC's hardware temperatures (like CPU and NVMe) on a small OLED display connected to an Arduino Mega 2560. It uses a C program on the PC to read sensor data and sends it over serial to the Arduino, which then displays the information.

## Overview

The project consists of two main components:

1.  **PC Backend**: (`PC/Backend/main.c`) A C program that runs on a Linux PC. It utilizes the `libsensors` library to access hardware temperature sensors. It reads the temperatures and sends formatted strings (e.g., "Package id 0: 45.50") over the serial port connected to the Arduino.
2.  **Arduino Frontend**: (`Arduino/Frontend/Display/src/main.cpp`) An Arduino sketch designed for the Arduino Mega 2560. It listens for incoming data on the serial port, parses the temperature labels and values, and displays them on an attached SSD1306 I2C OLED display using the U8g2 library.

## Hardware Requirements

* **PC**: A computer running Linux with `libsensors` installed.
* **Arduino**: Arduino Mega 2560 Rev3 (or compatible).
* **Display**: SSD1306 OLED Display (128x64 pixels) with an I2C interface.
* **USB Cable**: To connect the Arduino to the PC for programming and serial communication.
* **Wiring**: Jumper wires to connect the display to the Arduino's I2C pins (SDA, SCL, VCC, GND).

## Software Requirements & Dependencies

* **PC (Linux)**:
    * `gcc` compiler (or compatible C compiler).
    * `cmake` build system.
    * `libsensors` library (development package usually named `lm-sensors-devel`, `libsensors4-dev`, `libsensors-dev` or similar, depending on your Linux distribution).
* **Arduino**:
    * **PlatformIO IDE**: Recommended for compiling and uploading the Arduino code. It can be used as a [VS Code extension](https://platformio.org/platformio-ide).
    * **U8g2 Library**: By olikraus. PlatformIO will typically install this automatically based on the `platformio.ini` file.

## Setup and Usage

### 1. Arduino Frontend (`Arduino/Frontend/Display/`)

1.  **Connect Display**: Wire the SSD1306 display to the Arduino Mega's I2C pins
2.  **Open Project**: Open the `Arduino/Frontend/Display/` folder in VS Code with the PlatformIO extension installed.
3.  **Build & Upload**: Connect the Arduino Mega to your PC via USB. Use the PlatformIO interface (usually buttons in the status bar) to build and upload the `src/main.cpp` sketch to the Arduino.

### 2. PC Backend (`PC/Backend/`)

1.  **Install Dependencies**: Make sure you have `gcc`, `cmake`, and the `libsensors` development library installed on your Linux system. (e.g., on Fedora: `sudo dnf install gcc cmake lm_sensors-devel`, on Debian/Ubuntu: `sudo apt install build-essential cmake libsensors-dev`).
2.  **Identify Serial Port**: Check the name of the serial port your Arduino is connected to. It's often `/dev/ttyACM0` or `/dev/ttyUSB0`. If it's different, you'll need to modify the `serial` variable in `main.c`.
3.  **Compile**:
    * Navigate to the `PC/Backend/` directory in your terminal.
    * Create a build directory: `mkdir build && cd build`
    * Run CMake: `cmake ..`
    * Compile the code: `make`
4.  **Run**:
    * Ensure the Arduino is connected and running the frontend sketch.
    * Execute the compiled backend program from the `build` directory: `./Backend`
    * You might need appropriate permissions to access the serial port (e.g., add your user to the `dialout` group or run with `sudo`).

## How It Works

1.  The PC backend program initializes `libsensors` to access hardware monitoring chips.
2.  It opens a serial connection to the Arduino, configured for 115200 baud, 8N1.
3.  In a loop, it iterates through detected hardware chips and their features/subfeatures (sensors).
4.  For each readable temperature input (`SENSORS_SUBFEATURE_TEMP_INPUT`), it retrieves the value and the sensor's label.
5.  It formats the label and temperature value into a string (e.g., "Label: 12.34\n") and writes it to the serial port.
6.  The Arduino frontend continuously reads lines from the serial port.
7.  It parses each line to extract the label and the floating-point temperature value, storing them in arrays but skipping lines containing "Core".
8.  Periodically (every 3 seconds), it clears the OLED display buffer, draws the stored labels and temperatures using the U8g2 library, and sends the buffer to the display.
