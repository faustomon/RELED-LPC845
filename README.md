ReLED
A Feature-Rich Desk Clock with Alarm, Timer, and Weather Display
Project Description
ReLED is a desktop clock that combines essential time management tools with advanced features. It includes alarms, timers (standard and Pomodoro), a stopwatch, and real-time weather information. The clock communicates with a computer via UART, where its functionalities are managed through a companion application built with Qt Creator. This repository focuses on the microcontroller (MCU) code written in C++ and C, developed with MCUXpresso.

The project utilizes protocols like I2C and UART for communication and includes peripheral drivers for various functionalities, such as GPIO, ADC, and timed peripherals.

Key Features
Core Functionalities:

Real-time clock display using an RTC DS3231.
Weather data display using an OLED SSD1306.
Alarm and timer management, including Pomodoro timers and a stopwatch.
Peripheral Management:

GPIO handling for input and output.
ADC for analog data reading.
Support for 7-segment displays, LCDs, and LEDs.
Communication:

Serial communication with a computer for data transmission and control.
Integration with a separate Qt Creator project (available in another repository).
Modules Overview
This repository includes the following modules, each focused on specific MCU functionalities:

01-pin: Pin configuration and setup.
02-gpio: Digital input/output handling.
03-timer: Timer management for various operations.
04-EntradasDigitales: Digital input module.
05-SalidasDigitales: Digital output module.
06-PerifericosTemporizados: Timed peripherals management.
07-leds: LED control.
08-display_de_segmentos: 7-segment display handling.
09-teclado: Keypad input.
10-lcd: LCD control.
11-intext: External interrupt handling.
12-adc: Analog-to-digital conversion.
13-USART: UART communication.
14-I2C: I2C communication.
15-SERIAL: Serial communication module.
16-RTC: RTC DS3231 interface.
17-OLED: OLED SSD1306 display control.
Technologies Used
Programming Languages: C++ and C.
Development Environment: MCUXpresso IDE.
Hardware Protocols: I2C and UART.

Getting Started

Contributions are welcome! Feel free to fork this repository and add new modules or enhance existing ones. Submit a pull request to share your improvements.

Contact Information
For questions or collaboration inquiries, contact:
Fausto Mongini
📧 faustomonginiwork@gmail.com
