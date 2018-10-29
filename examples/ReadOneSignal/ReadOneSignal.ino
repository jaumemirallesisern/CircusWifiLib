/*
  ReadOneSignal.ino
  
  This example code that shows how to read a signal using the circusofthings.com API through its CircusWifiLib-2.0.0 library for Arduino.
  
  This code will read a random signal existing at Circus.
  
  A software serial port is used, so the onboard serial port is used to monitor the process. You have 3 degrees for monitor: DEBUG_NO,DEBUG_YES and DEBUG_DEEP.
  
  There are no 3rd part libraries to use, beside SoftwareSerial.
  
  Created by Jaume Miralles Isern, October 26, 2018.
*/


#include <CircusWifiLib.h>

// ------------------------------------------------
// These are the CircusWifiLib related declarations
// ------------------------------------------------

int TXPinForWifiModule = 2;               // IO port in your arduino you will use as TX for serial communication with the wifi module
int RXPinForWifiModule = 3;               // IO port in your arduino you will use as RX for serial communication with the wifi module
char ssid[] = "JAZZTEL_rkf6";             // your wifi network SSID
char pass[] = "bthsgwkym7mf";             // your wifi network password
int wifiSerialBaudRate = 9600;            // Baud rate between your arduino and your wifi module. Did you check that your module actually uses this baud rate?
int debugSerialBaudRate = 9600;           // Baud rate between your arduino and your computer
char token[] = "eyJhbGciOiJIUzUxMiJ9.eyJqdGkiOiIxIiwic3ViIjoiSmF1bWUgTWlyYWxsZXMiLCJpc3MiOiJ1c2VyYXJlYS1qc3AtcGFnZSJ9.QUQDrrMAgbZZZrkNwQ4CnWWE2QCaTSMhz6ldboHBlSWot_ZmeCqdREfIYl9t424RXcIUlsvUVWKAN9ARD5cIFw";    // Your API token for Circus
char randomNumberKey[] = "23780";         // The key of the signal you have created at circusofthings.com

SoftwareSerial ss(RXPinForWifiModule,TXPinForWifiModule);
CircusWifiLib circus(Serial,&ss,ssid,pass,DEBUG_DEEP,KEEP_ALIVE);



void setup() {
  Serial.begin(debugSerialBaudRate);
  ss.begin(wifiSerialBaudRate);
  circus.begin();    
}

void loop() {
  delay(5000);
  double d = circus.read(randomNumberKey,token);
  Serial.print("Random number is: ");
  Serial.println(d);
}
