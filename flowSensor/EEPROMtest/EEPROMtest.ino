#include <M95M01.h>

#include <FlashAsEEPROM.h>
#include <FlashStorage.h>
#include <RTCZero.h>
#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiMDNSResponder.h>
#include <WiFiSSLClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

/*
  EEPROM M95320

  Pins:
    HOLD = D6
    CS = D5
    Q (MISO) = 22
    SCK = 24
    D (MOSI) = 23

  Inspired by the BarometricPressureSensor example. 
*/

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

//Sensor's memory register addresses:
const byte READ = 0b11111100;     // FIND EEPROM'S read command
const byte WRITE = 0b00000010;   // FIND EEPROMS'S write command

// pins used for the connection with the sensor
// the other you need are controlled by the SPI library):
const int chipSelectPin = 5;

void setup() {
  Serial.begin(9600);

  // start the SPI library:
  SPI.begin();

  pinMode(chipSelectPin, OUTPUT);

  //Configure SCP1000 for low noise configuration:
  //writeRegister(0x02, 0x2D);
  //writeRegister(0x01, 0x03);
  //writeRegister(0x03, 0x02);
  
  // give the sensor time to set up:
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:

}

//Sends a write command to EEPROM

void writeRegister(byte thisRegister, byte thisValue) {

  //Q = data output MISO; pin = 23
}