/*
  CircusWifiLib.cpp  (Version 2.0.0)
	Implements the circusofthings.com API in Arduino devices when connected by wifi through an external ESP8266 module.
  Created by Jaume Miralles Isern, October 23, 2018.
*/


#include "CircusWifiLib.h"

CircusWifiLib::CircusWifiLib(HardwareSerial &print, SoftwareSerial *Serial1, char *ssid, char *pass, int debug, int keepalive)
{
	printer = &print;	
	_Serial1 = Serial1; 
        _ssid = ssid;
        _pass = pass;
	_debug=debug;	
        _keepalive = keepalive;
}

void CircusWifiLib::console(char message, int level) {
    if(level<=_debug) {
        printer->write(message);
    }
}

void CircusWifiLib::console(char *message, int level) {
    if(level<=_debug) {
        printer->print(message);
    }
}

void CircusWifiLib::console(const __FlashStringHelper *message, int level) {
    if(level<=_debug) {
        printer->print(message);
    }
}

void CircusWifiLib::panicbutton() {
    if(checkWIFI()==0){
        console(F("[CircusWifiLib] Wifi ok, restablishing TCP\n"),2);
        if(connectServer()==0){} else {panicbutton();}
    } else {
        console(F("[CircusWifiLib] Wifi not ready either\n"),2);
        if(_keepalive) {
            console('\n',1);
            console(F("[CircusWifiLib] Persistent restart\n"),1);
            char rOK[] = "OK\r\n";
            char closecommand[] = "AT+CIPCLOSE\r\n";
            sendcommand(closecommand,rOK,1000L,1);
            char releaseAPcommand[] = "AT+CWQAP\r\n";
            sendcommand(releaseAPcommand,rOK,1000L,1);
            begin();
        } else {
            console(F("[CircusWifiLib] Device stopped\n"),1);
        }
    }    
}


int CircusWifiLib::sendcommand(char *scom, char *dresp, unsigned long timeout, int retries) {
    char rresp[200];
    for( int i = 0; i < sizeof(rresp);  ++i )
        rresp[i] = (char)0;
    for(int i=0;i<retries;i++){
        _Serial1->print(scom);
        console(scom,2);
        delay(50);
        unsigned long startTime = millis();
        int j=0;		
        while (millis()-startTime < timeout) {	
            while (_Serial1->available()) {
                char c = _Serial1->read();
                if(c!='\0') {rresp[j] = c;} else {rresp[j] = ' ';} // para más inri los huecos son \0
                console(rresp[j],2);
                j++;
                if (strstr(rresp,dresp)) {return 0;}				
                if(j>200){console(F("overflow\n"),2); return -1;}				
            }
        }
        printer->println();
    }
    return -1;
}

int CircusWifiLib::checkTCP() {
    char rOK[] = "OK\r\n";
    char sendOK[] = "SEND OK\r\n";
    char sdcommand[] = "AT+CIPSEND=1\r\n";
    if(sendcommand(sdcommand,rOK,1000L,1)==0) {
        char content[] = "W\r\n";
        return(sendcommand(content,sendOK,1000L,1));
    } else {
        return -1;
    }
}

int CircusWifiLib::checkWIFI() {
    char gotIP[] = "STATUS:2\r\n";
    char disconnected[] = "STATUS:4\r\n";
    char statuscommand[] = "AT+CIPSTATUS\r\n";
    int r1 = sendcommand(statuscommand,gotIP,5000L,1);
    int r2 = sendcommand(statuscommand,disconnected,5000L,1);
    if(r1==0 || r2==0) {return 0;} else {return -1;}
}

int CircusWifiLib::checkESP8266() {
    char rOK[] = "OK\r\n";
    char atcommand[] = "AT\r\n";
    return(sendcommand(atcommand,rOK,5000L,5));
}

int CircusWifiLib::setESP() {
	char rOK[] = "OK\r\n";
	char ready[] = "ready\r\n";        
        
        // checkings
        char atcommand[] = "AT\r\n";
	if(sendcommand(atcommand,rOK,5000L,5) == 0) {// Anyone there? 
		console('.',1);
	} else {
		return -1;
	}
        char versioncommand[] = "AT+GMR\r\n";
        if(sendcommand(versioncommand,rOK,5000L,5) == 0) { // Get version		
		if(!_debug) {printer->print(".");}
	} else {
		return -1;
	}
	
        // settings
        char releaseAPcommand[] = "AT+CWQAP\r\n";
        sendcommand(releaseAPcommand,rOK,1000L,1);
        char echocommand[] = "ATE0\r\n";
	if(sendcommand(echocommand,rOK,5000L,2) == 0) { // Modem echo off
		console('.',1);
	} else {
		return -1;
	}
	char cwmcommand[] = "AT+CWMODE=1\r\n";
	if(sendcommand(cwmcommand,rOK,5000L,2) == 0) { // Station mode
		console('.',1);
	} else {
		return -1;
	}	
	char muxcommand[] = "AT+CIPMUX=0\r\n";
	if(sendcommand(muxcommand,rOK,5000L,2) == 0) { // Enable multiple connections
		console('.',1);
	} else {
            return -1;           
	}
	char infocommand[] = "AT+CIPDINFO=1\r\n";
	if(sendcommand(infocommand,rOK,5000L,5) == 0) { // Get remote ip and port (+ipd)
		console('.',1);
	} else {
		return -1;
	}
	char autocommand[] = "AT+CWAUTOCONN=0\r\n";
	if(sendcommand(autocommand,rOK,5000L,5) == 0) { // Autoconnect on power
		console('.',1);
	} else {
		return -1;
	}
	char dhcpcommand[] = "AT+CWDHCP=1,1\r\n";
	if(sendcommand(dhcpcommand,rOK,5000L,5) == 0) { // Enable DHCP in station mode
		console('.',1);
	} else {
		return -1;
	}
        char rstcommand[] = "AT+RST\r\n";
	if(sendcommand(rstcommand,ready,5000L,5) == 0) {// Reset 
		console('.',1);
	} else {
		return -1;
	} 
        
	return 0;
}

int CircusWifiLib::connectWIFI() {
	char rOK[] = "OK\r\n";        
        char command[50];
        sprintf_P(command, PSTR("AT+CWJAP_CUR=\"%s\",\"%s\"\r\n"),_ssid,_pass);        
	if(sendcommand(command,rOK,7000L,1) == 0) { // Get connection
		console('.',1);
	} else {
		return -1;
	}
        
        char conwcommand[] = "AT+CWJAP?\r\n";
	if(sendcommand(conwcommand,rOK,2000L,1) == 0) { // Check connection
		console('.',1);
	} else {
		return -1;
	}
        
        char cifcommand[] = "AT+CIFSR\r\n";
	if(sendcommand(cifcommand,rOK,2000L,1) == 0) { // Check connection
		console('.',1);
	} else {
		return -1;
	}

	return 0;
}

int CircusWifiLib::connectServer() {
	char rOK[] = "OK\r\n";
        char already[] = "ALREADY CONNECTED\r\n";
	char mustclose[] = "must close ssl link";
	char csizecommand[] = "AT+CIPSSLSIZE=4096\r\n";
	char closecommand[] = "AT+CIPCLOSE\r\n";
        if(sendcommand(csizecommand,rOK,5000L,1) == 0) { // ?
            console('.',1);
	} else {
            //if(sendcommand(csizecommand,mustclose,5000L,1) == 0) {
                //sendcommand(closecommand,rOK,1000L,1);
                //connectServer();
            //}
            return -1;
	}
        char startcommand[] = "AT+CIPSTART=\"SSL\",\"circusofthings.com\",443\r\n";
	if(sendcommand(startcommand,rOK,15000L,1) == 0) { // Get status
		console('.',1);
	} else {
            if(sendcommand(startcommand,already,2000L,1) == 0) {
                return 0;
            }
            return -1;
	}
        
	return 0;
}


void CircusWifiLib::begin()
{        
    console('\n',1);
    console(F("[CircusWifiLib] Setting ESP8266 parameters"),1);	
    console('\n',1);
    if(setESP()==0) {
        console('\n',1);
        console(F("[CircusWifiLib] Module Ok!"),1);
        delay(500);
        console('\n',1);
        console(F("[CircusWifiLib] Reaching the network"),1);	
        console('\n',1);
        if(connectWIFI()==0) {
            console('\n',1);
            console(F("[CircusWifiLib] Network Ok"),1);
            delay(500);
            console('\n',1);
            console(F("[CircusWifiLib] Reaching the server"),1);	
            console('\n',1);
            if(connectServer()>-1) {
                console('\n',1);
                console(F("[CircusWifiLib] Server Ok"),1);
            } else {
                console('\n',1);
                console(F("[CircusWifiLib] Server error\n"),1); 
                panicbutton();
            }
            delay(500);
        } else {
            console('\n',1);
            console(F("[CircusWifiLib] Network error\n"),1);
            panicbutton(); 
        }
    } else {
        console('\n',1);
        console(F("[CircusWifiLib] Module error\n"),1);
        panicbutton(); 
    }
}

int CircusWifiLib::count(char *text) {
    for(int i=0;i<300;i++){
        if(text[i]=='\0'){
            return(i);
        }
    }
    return -1;
}

char* CircusWifiLib::parseServerResponse(char *r, char *label, int offset) {
    int labelsize = count(label);
    char *ini = strstr(r,label) + labelsize + offset;
    static char content[100];
    int i=0;
    while (*ini!='\0'){ // may truncated occur
        if(*ini!='\"')
            content[i]=*ini;
        else {
            content[i]='\0';
            return content;
        }
        i++; ini++;
    }
    return (char)0;
}

char* CircusWifiLib::sendQuery(char *q) {
    char listen[] = ">";
    char sendOK[] = "SEND OK";
    int c = count(q) + 30 + 33 + 2; 
    char cmdBuf[20];
    sprintf_P(cmdBuf, PSTR("AT+CIPSEND=%d\r\n"), c);
    if(sendcommand(cmdBuf,listen,1000L,1)==0) { 
        delay(0);
        console(q,2); _Serial1->print(q);
        console(F("Host: www.circusofthings.com\r\n"),2); _Serial1->print(F("Host: www.circusofthings.com\r\n")); // +30
        console(F("User-agent: CircusWifiLib-1.1.0\r\n"),2); _Serial1->print(F("User-agent: CircusWifiLib-1.1.0\r\n")); // +33
        console(F("\r\n"),2); _Serial1->print("\r\n"); // +2
        delay(0); // 80 bytes buffer    https://www.esp8266.com/viewtopic.php?f=8&t=3111
        unsigned long startTime = millis();
        int j=0;
        int pick=0;
        static char r[100];
        while (millis()-startTime < 2000) {
            while (_Serial1->available()) {
                char c = _Serial1->read();
                if(_debug>1) {printer->print(c);}
                if (c=='{') {pick=1;}
                if(pick){r[j]=c;j++;}
                if (c=='}') {r[j]='\0';pick=0;}                        
            }
        }
        return r;
    } else {
        char rOK[] = "OK\r\n";
        console(F("AT+CIPSEND error\n"),2);
        char closecommand[] = "AT+CIPCLOSE\r\n";
        sendcommand(closecommand,rOK,1000L,1);
        return (char)0;
    }
}

void CircusWifiLib::write(char *key, double value, char *token) {
    
    char bufValue[15]; 
    dtostrf(value,1,4,bufValue);
    char q[250];
    sprintf_P(q, PSTR("GET /WriteValue?Key=%s&Value=%s&Token=%s HTTP/1.1\r\n\0"), key, bufValue, token);

    console('\n',1);
    console(F("[CircusWifiLib] Write  "),1);
    console(bufValue,1);console(F(" --> "),1);console(key,1);console('\n',1);
    char *serverresponse = sendQuery(q);

    if (serverresponse!=(char)0) {
        char labelk[] = "Key";
        char *key = parseServerResponse(serverresponse, labelk,3);
        if(_debug==2) {console('\n',1);}
        console(F("[CircusOfThings.com] "),1);
        console(key,1);
        char labelm[] = "Message";
        char *message = parseServerResponse(serverresponse, labelm,3);
        console(F(" - "),1);
        console(message,1);
    } else {
        console(F("[CircusWifiLib] Can't stablish TCP connection\n"),2);
        panicbutton();
    }
}

double CircusWifiLib::read(char *key, char *token) {
    
    char q[250];
    sprintf_P(q, PSTR("GET /ReadValue?Key=%s&Token=%s HTTP/1.1\r\n\0"), key, token);

    console('\n',1);
    console(F("[CircusWifiLib] Read from --> "),1);
    console(key,1);console('\n',1);
    char *serverresponse = sendQuery(q);

    if (serverresponse!=(char)0) {
        char labelk[] = "Key";
        char *key = parseServerResponse(serverresponse, labelk, 3);
        if(_debug==2) {console('\n',1);}
        console(F("[CircusOfThings.com] "),1);
        console(key,1);
        char labelm[] = "Message";
        char *message = parseServerResponse(serverresponse, labelm, 3);
        console(F(" - "),1);
        console(message,1);
        char labelv[] = "Value";
        console('\n',1);
        char *value = parseServerResponse(serverresponse, labelv, 2);   
        console(F("[CircusOfThings.com] "),1);
        console(value,1);
        console('\n',1);
        return(atof(value));
    } else {
        console(F("[CircusWifiLib] Can't stablish TCP connection\n"),2);
        panicbutton();
    }
}

