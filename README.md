# pn532-lib
PN532 NFC Library for Raspberry Pi, STM32, Arduino

![image](http://www.waveshare.net/photo/accBoard/PN532-NFC-HAT/PN532-NFC-HAT-3.jpg)

# How to use?

## For Raspberry Pi

### Using SPI
1.  Set I0I1:
-   I0 --> LOW
-   I1 --> HIGH
2.  Hardware Connection:
-   SCK --> SCK
-   MISO --> MISO
-   MOSI/SDA/TX --> MOSI
-   NSS/SCL/RX --> D4 (GPIO 4 of BCM pinout)
Optional: 
-   RSTPDN --> D20 (for hardware reset)
3.  Modify the PN532 Init lines for SPI in `example/raspberrypi/rpi_get_uid.c`:
```
PN532_SPI_Init(&pn532);
//PN532_I2C_Init(&pn532);
//PN532_UART_Init(&pn532);
```
4.  Compile with make:
```
cd examples/raspberrypi && make
```
5.  Run the demo with:
```
./rpi_get_uid.exe
```

### Using I2C
1.  Set I0I1:
-   I0 --> HIGH
-   I1 --> LOW
2.  Hardware Connection:
-   MOSI/SDA/TX --> SDA
-   NSS/SCL/RX --> SCL
Optional: 
-   P32 --> D16 (for hardware request)
-   RSTPDN --> D20 (for hardware reset)
3.  Modify the PN532 Init lines for I2C in `example/raspberrypi/rpi_get_uid.c`:
```
//PN532_SPI_Init(&pn532);
PN532_I2C_Init(&pn532);
//PN532_UART_Init(&pn532);
```
4.  Compile with make:
```
cd examples/raspberrypi && make
```
5.  Run the demo with:
```
./rpi_get_uid.exe
```

### Using UART
1.  Set I0I1:
-   I0 --> LOW
-   I1 --> LOW
2.  Hardware Connection:
-   MOSI/SDA/TX --> RX
-   NSS/SCL/RX --> TX
Optional: 
-   RSTPDN --> D20 (for hardware reset)
3.  Modify the PN532 Init lines for UART in `example/raspberrypi/rpi_get_uid.c`:
```
//PN532_SPI_Init(&pn532);
//PN532_I2C_Init(&pn532);
PN532_UART_Init(&pn532);
```
4.  Compile with make:
```
cd examples/raspberrypi && make
```
5.  Run the demo with:
```
./rpi_get_uid.exe
```

## For Arduino UNO
Please first copy the files pn532.c, pn532.h, pn532_uno.cpp and pn532_uno.h to the libraries directory,
which can be set up by Arduino IDE --> preference.

Then you should also connect the serial port from your UNO to a PC for Serial Monitor. 
-   Baudrate: 115200.

### Using SPI
1.  Set I0I1:
-   I0 --> LOW
-   I1 --> HIGH
2.  Hardware Connection:
-   SCK --> Arduino D13
-   MISO --> ArduinoD12
-   MOSI --> Arduino D11
-   NSS --> Arduino D4

3.  Open examples\arduino\uno_get_uid\ uno_get_uid.ino with Arduino IDE then change the PN532 Init lines for SPI:
```
PN532_SPI_Init(&pn532);
//PN532_I2C_Init(&pn532);
```
4.  Compile and upload the demo to your Arduino UNO board:
```
examples\arduino\uno_get_uid\ uno_get_uid.ino
```

### Using I2C
1.  Set I0I1:
-   I0 --> HIGH
-   I1 --> LOW
2.  Hardware Connection:
- 	SCL --> ArduinoA5
- 	SDA --> ArduinoA4
3.  Open examples\arduino\uno_get_uid\uno_get_uid.ino with Arduino IDE then change the PN532 Init lines for I2C:
```
//PN532_SPI_Init(&pn532);
PN532_I2C_Init(&pn532);
```
4.  Compile and upload the demo to your Arduino UNO board:
```
examples\arduino\uno_get_uid\uno_get_uid.ino
```

## For STM32
The demos are build on STM32F103CBT6, but you can port them with STM32CubeMX.

1.  Set I0I1:
-   I0 --> LOW
-   I1 --> HIGH
2.  Hardware connection:
-   SCK --> PA5
-   MISO --> PA6
-   MOSI --> PA7
-   NSS --> PA4
-   PA9 --> RX
-   PA10 --> TX
3.  Extract: pn532-lib\examples\stm32\stm32.7z
4.  Open the project MDK-ARM\pn532_stm32.uvprojx with Keil V5
5.  Build and download the project to your STM32 board.
