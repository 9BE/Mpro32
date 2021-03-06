/*
 * LocWiFi.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: annuar
 */

#include "LocWiFi.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>

LocWiFi	*iniWiFi;
TaskHandle_t loopLocWiFi= NULL;
WiFiMulti 	*wifiMulti;

LocWiFi::LocWiFi(int core, int loopDelay, int *lookVal) {
	iniWiFi = this;
	iniWiFi->_loopDelay = loopDelay;
	iniWiFi->_lookVal = lookVal;

	wifiMulti = new WiFiMulti;
	wifiMulti->APlistClean();

	xTaskCreatePinnedToCore(iniWiFi->loop, "loopLocWiFi", 3*1024, NULL, 1, &loopLocWiFi, core);
}

void LocWiFi::_openConnection(int as) {
	log_i("LocWiFi::_readSsid");
	LocSpiff *ls;
	bool staFlag=false;

	if ( (as == lw_wifi_sta) || (as == lw_wifi_apsta) ) {
		if (WiFi.isConnected()) {
			log_i("WiFi isConnected");
			return;
		}
	}

//	if (as == lw_wifi_ap) {
//		if (_dahAP) {
//			return;
//		}
//	}

	ls = new LocSpiff("LocWiFi::_openConnection");
	_sList.clear();
	_sList = ls->readCSV("/ssid.txt");
	delete ls;

	wifiMulti->APlistClean();

	log_i("INITIAL SSID=%s IP=%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	if( (as == lw_wifi_sta) || (as == lw_wifi_apsta) || (as == lw_wifi_ap) ){
		for(int x=0; x < _sList.size(); x++){
			auto data = _sList.at(x);

			if( ((data.at(0) == "apsta") || (data.at(0) == "sta")) && (as != lw_wifi_ap) ){
				log_i("STA");
				wifiMulti->addAP(data.at(1).c_str(), data.at(2).c_str());
				staFlag = true;
			}
			if( ((data.at(0) == "apsta") || (data.at(0) == "ap")) && (as != lw_wifi_sta) ){
				log_i("AP ...");
				if (!_dahAP) {
					board * Board;
					Board = new board;
					WiFi.softAP(data.at(1).c_str(), data.at(2).c_str(), Board->getChannel()); //Board->getChannel()
					delete Board;
					_dahAP = true;
					log_i("IP=%s", WiFi.softAPIP().toString().c_str());
				}

			}
		}
		if(staFlag){
			log_i("Start ...");
			wifiMulti->run();
			log_i("... End SSID=%s IP=%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
		}
	}

	log_i("Memory = %d", esp_get_free_heap_size());
}

void LocWiFi::loop(void* parameter) {
	//init


	while(true){

//		log_i("sini :: %d", *iniWiFi->_lookVal);
		switch (*iniWiFi->_lookVal) {
			case lw_wifi_ap:
				iniWiFi->_openConnection(lw_wifi_ap);
				*iniWiFi->_lookVal = 0;
				break;
			case lw_wifi_sta:
				if (!WiFi.isConnected()) {
					iniWiFi->_openConnection(lw_wifi_sta);
				}
				break;
			case lw_wifi_apsta:
				if (!WiFi.isConnected()) {
					iniWiFi->_openConnection(lw_wifi_apsta);
				}
				break;
			case lw_wifi_off:

				break;
			default:
				break;
		}

//		if (millis() > 60000) {
//			delete iniWiFi;
////			if (!WiFi.isConnected()) {
////				delete iniWiFi;
////			}
//
//		}


//		*iniWiFi->_lookVal = 0;
		delay(iniWiFi->_loopDelay);
	}
}


LocWiFi::~LocWiFi() {
	log_i("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& DESTRUCTING LocWiFi");
	vTaskDelete(NULL);

}

