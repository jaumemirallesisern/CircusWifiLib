/*
  CircusWifiLib.h  (Version 1.0.0)

	Implements the circusofthings.com API in Arduino devices when connected by wifi through an external ESP8266 module.

  Created by Jaume Miralles Isern, September 25, 2018.
*/


#ifndef CircusWifiLib_h
#define CircusWifiLib_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "WiFiEsp.h"
#include "ArduinoJson.h"
#include "DHT.h"

class CircusWifiLib
{
  	public:
		CircusWifiLib(HardwareSerial &print, SoftwareSerial *Serial1, WiFiEspClient *client, int wifiSerialBaudRate, char *ssid, char *pass, char *token);
		void begin();		
		double read(char *key);
		void write(char *key, double value);
		boolean wifiStatus();
		boolean circusServerStatus();
	private:
		HardwareSerial* printer;
		SoftwareSerial *_Serial1;
		WiFiEspClient *_client;		
		char *_ssid;
		char *_pass;
		char *_token;
		int status;
		boolean connectToWifi(char *ssid, char *pass);
		boolean connectToCircus();
		boolean disconnectFromCircus();
		void printWifiStatus();
		JsonObject& getJsonResponse();
};

#endif
