/*
 * NdirectOTA.cpp
 *
 *  Created on: Nov 18, 2020
 *      Author: suhaimi
 */

#include <NdirectOTA.h>

TaskHandle_t directOtaTask = NULL;

N_directOTA::N_directOTA(uint32_t core, uint32_t loopDelay) {
	this->_loopDelay = loopDelay;
	this->_server = new WebServer(80);

	xTaskCreatePinnedToCore(this->loop, "directOtaTask", 6000, this, 1, &directOtaTask, core);
}

N_directOTA::~N_directOTA() {
	// TODO Auto-generated destructor stub
}

void N_directOTA::loop(void* param) {
	N_directOTA *ini = (N_directOTA*) param;

	MDNS.begin("auto");

	ini->_server->on("/stat", WebServer::THandlerFunction([ini]() {
		ini->statusViaWiFi(ini);
	}));

	ini->_server->on("/reboot", HTTP_GET, WebServer::THandlerFunction([ini]() {
		ini->_server->sendHeader("Connection", "close");
		ini->_server->send(200, "text/html", "done");
		delay(1000);
		ESP.restart();
		}));

////	ini->_server->on("/clearfs", WebServer::THandlerFunction([]() {
////		iniDirectOTA->_clearFile();
////	}));
//
	ini->_server->on("/ota", HTTP_GET, WebServer::THandlerFunction([ini]() {
		ini->_server->sendHeader("Connection", "close");
		ini->_server->send(200, "text/html", ota);}));

////	ini->_server->on("/jquery", HTTP_GET, WebServer::THandlerFunction([]() {
////		iniDirectOTA->_server->sendHeader("Connection", "close");
////		iniDirectOTA->_server->send(200, "text/javascript", jquery);
////	}));
//
//
//
	ini->_server->on("/update", HTTP_POST, WebServer::THandlerFunction([ini]() {
		ini->_server->sendHeader("Connection", "close");
		ini->_server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		delay(1000);
		ESP.restart();}),
		WebServer::THandlerFunction([ini]() {
			HTTPUpload& upload = ini->_server->upload();
			if (upload.status == UPLOAD_FILE_START) {
				log_i("Update: %s\n", upload.filename.c_str());
				if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_FLASH,2,1)) { //start with max available size
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_WRITE) {
				/* flashing firmware to ESP*/
				if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_END) {
				if (Update.end(true)) { //true to set the size to the current progress
//					Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
				}
				else {
//					Update.printError(Serial);
				}
			}
		}));


	ini->_server->begin(80);

	while (1) {
		ini->_server->handleClient();
		delay(ini->_loopDelay);
	}
}

String N_directOTA::getMAC() {
	uint8_t mac[6];
	WiFi.macAddress(mac);

	String macID = "";


	uint8_t num;
	for(int i=0; i < 5; i++){
		num = mac[i];
		if(num < 16) macID += "0";
		macID += String(mac[i], HEX) + ":";
	}
	macID += String(mac[5], HEX);



	macID.toUpperCase();

	return macID;
}

void N_directOTA::statusViaWiFi(N_directOTA* ini) {
	String msg="";

//	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());

	long rssi = WiFi.RSSI();

	msg  = "MAC = " + ini->getMAC() + "</br>";
	msg += "IP = " + WiFi.localIP().toString() + "</br>";
	msg += "Live = " + String(millis()/1000) + "</br>";
//	msg += "Time = " + timenow + "</br>";
	msg += "RSSI = " + String(rssi) + "</br>";
	msg += "<hr>";

//	msg += iniDirectOTA->siniLocMando->dapatMandoText;


//	iniDirectOTA->siniLocMando->dapatMandoText = "";

	ini->_server->sendHeader("Connection", "close");
	ini->_server->send(200, "text/html", msg);

	delay(50);
}
