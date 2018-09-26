/*
  CircusWifiLib.cpp  (Version 1.0.0)

	Implements the circusofthings.com API in Arduino devices when connected by wifi through an external ESP8266 module.

  Created by Jaume Miralles Isern, September 25, 2018.
*/


#include "CircusWifiLib.h"

CircusWifiLib::CircusWifiLib(HardwareSerial &print, SoftwareSerial *Serial1, WiFiEspClient *client, int wifiSerialBaudRate, char *ssid, char *pass, char *token)
{
	printer = &print;	
	_Serial1 = Serial1;       
	_client=client;
	_ssid=ssid;
	_pass=pass;
	_token=token;
	status = WL_IDLE_STATUS;
}

void CircusWifiLib::begin()
{
	printer->print("[CircusWifiLib] "); 
	printer->println("Starting CircusWifiLib library"); 
	if(connectToWifi(_ssid,_pass)) {connectToCircus();}	
}

boolean CircusWifiLib::wifiStatus() {
	if(status != WL_CONNECTED) {return true;} else {return false;}
}

boolean CircusWifiLib::circusServerStatus() {
	return(_client->connected());
}

boolean CircusWifiLib::connectToWifi(char *ssid, char *pass)
{
	while (status != WL_CONNECTED) {	
		WiFi.init(_Serial1);
		if (WiFi.status() == WL_NO_SHIELD) {
			printer->print("[CircusWifiLib] "); 
			printer->println("WiFi shield not present");    
			return false;
		}
		while ( status != WL_CONNECTED) {
			printer->print("[CircusWifiLib] "); 
			printer->print("Attempting to connect to WPA SSID: ");
			printer->println(ssid);    
			status = WiFi.begin(ssid, pass);
		}  
		printWifiStatus();
	} 
	return true;
}

void CircusWifiLib::printWifiStatus()
{
	printer->print("[CircusWifiLib] "); 
	Serial.print("");
	Serial.print("SSID: ");
	Serial.print(WiFi.SSID());
	IPAddress ip = WiFi.localIP();
	Serial.print(", ");
	Serial.print("IP Address: ");
	Serial.print(ip);
	long rssi = WiFi.RSSI()/10;
	Serial.print(", ");
	Serial.print("Signal strength (RSSI):");
	Serial.print(rssi);
	Serial.print(" dBm");
}

boolean CircusWifiLib::connectToCircus()
{
	while (!_client->connected()) {
		char server[] = "circusofthings.com";
		// connect to server
		printer->println();
		if (_client->connectSSL(server, 443)) {
			printer->print("[CircusWifiLib] "); 			
			printer->println("Connected!");    			
			_client->println();
			return true;
		}
	} 
	return true; // already connected
}

boolean CircusWifiLib::disconnectFromCircus()
{
  	printer->println();
  	printer->print("[CircusWifiLib] "); 
	printer->println("Disconnecting from Circus...");
  	_client->stop();
}

JsonObject& CircusWifiLib::getJsonResponse()
{
	String json;
	int pick = 0;
	while (_client->available()) { 
		char c = _client->read();
		if (c=='{') {pick = 1;}
		if (pick) {json += c;}
		if (c=='}') {pick = 0;}		
	}
	printer->print("[Circus-Server] "); 
	printer->println(json);
	StaticJsonBuffer<100> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	return root;
}

double CircusWifiLib::read(char *key)
{
	if(connectToWifi(_ssid,_pass)) {connectToCircus();}
	char query[200];
	strcpy(query,"GET /ReadValue?Key=");
	strcat(query,key);
	strcat(query,"&Token=");
	strcat(query,_token);
	strcat(query," HTTP/1.1");
	_client->println(query); // Make a HTTP request
	_client->println("Host: www.circusofthings.com");
	_client->println();
	JsonObject& root = getJsonResponse();
	const double value = root["Value"];
  	const char* m = root["Message"];
  	const String k = root["Key"];
	printer->print("[CircusWifiLib] "); 
	printer->println(value);
	delay(1000);
	return(value);
}

void CircusWifiLib::write(char *key, double value)
{
	if(connectToWifi(_ssid,_pass)) {connectToCircus();}
	char query[200];
	strcpy(query,"GET /WriteValue?Key=");
	strcat(query,key);
	strcat(query,"&Value=");
	char bufValue[15]; 
	dtostrf(value,1,4,bufValue);
	strcat(query,bufValue);
	strcat(query,"&Token=");
	strcat(query,_token);
	strcat(query," HTTP/1.1");
	_client->setTimeout(8000);
	_client->println(query); // Make a HTTP request
	_client->println("Host: www.circusofthings.com");
	_client->println();
	printer->print("[CircusWifiLib] "); 
	printer->println(query);
	JsonObject& root = getJsonResponse();
  	const char* m = root["Message"];
  	const String k = root["Key"];
	delay(1000);
}
