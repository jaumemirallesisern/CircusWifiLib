/*
  CircusWifiLib.h  (Version 2.0.0)

	Implements the circusofthings.com API in Arduino devices when connected by wifi through an external ESP8266 module.

  Created by Jaume Miralles Isern, October 23, 2018.
*/


#ifndef CircusWifiLib_h
#define CircusWifiLib_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DHT.h"

#define DEBUG_NO 0
#define DEBUG_YES 1
#define DEBUG_DEEP 2

#define FAIL_STOP 0
#define KEEP_ALIVE 1


class CircusWifiLib
{
  	public:
		CircusWifiLib(HardwareSerial &print, SoftwareSerial *Serial1, char *ssid, char *pass,int debug, int keepalive);
		void begin();
		void write(char *key, double value, char *token);
                double read(char *key, char *token);
                int checkTCP();
                int checkWIFI();
                int checkESP8266();
	private:
		HardwareSerial* printer;
		SoftwareSerial *_Serial1;
		char *_ssid;
                char *_pass;
		int _debug;
                int _keepalive;
		//int sendcommand(String scom, String dres, unsigned long timeout, int retries);
		int comp(char *whole,char *target);
		int sendcommand(char *scom, char *dres, unsigned long timeout, int retries);
		int senddata(int socket, char *data, int length, char *dresp, unsigned long timeout, int retries);
		int setESP();
		int connectWIFI();
		int connectServer();
                int count(char *text);
                char* parseServerResponse(char *r, char *label, int offset);
                char* sendQuery(char *q);
                void console(const __FlashStringHelper *message, int level);
                void console(char *message, int level);
                void console(char message, int level);
                void panicbutton();
};

#endif
