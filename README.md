# UART to UDP data transfer 
using Arduino Uno and Atwinc 1500

## Project details
Arduino listens for UART input data in the form of 16 bytes followed by a termination character. When correct input is provided the Arduino will send this data via SPI to Atwinc 1500 to be transferred to remote client via UDP packets.

## Setup
Wiring configuration is based on the [Adafruit Wiring Page](https://learn.adafruit.com/adafruit-atwinc1500-wifi-module-breakout/wiring-and-test).

## How to use
Connect to `atmelwifi`, use password: `1234567890`
To initiate UDP connection, open terminal and type the following:
```
nc -u 192.168.1.1 80
```
Once connection is established, send the first byte by typing any one character and pressing enter.

