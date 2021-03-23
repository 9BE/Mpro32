/*
 * LocWireless.cpp
 *
 *  Created on: Sep 19, 2020
 *      Author: annuar
 */

#include <LocWireless.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>

LocWireless *iniWireless;
TaskHandle_t handlerWireless= NULL;
WiFiMulti 	*wifiMulti;


void LocWireless::connect(wifi_mode_t as) {
	iniWireless->connectionAs = as;
}

void LocWireless::loop(void *parameter) {

	log_i("SONi");
	esp_task_wdt_init(iniWireless->_loopDelay+2, true);
	esp_task_wdt_add(handlerWireless);

	int previousConnection = iniWireless->connectionAs;
	int counter;

	while(true){
//		log_i("IP=%s", WiFi.softAPIP().toString().c_str());
//		log_i("SSID=%s IP=%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
//		log_i("wifi power get is %d", WiFi.getTxPower());

		if(WiFi.getMode() != iniWireless->connectionAs){
			log_i("SINIIII ::: %d", iniWireless->connectionAs);
			if(WiFi.mode(iniWireless->connectionAs)){
//				WiFi.setTxPower(WIFI_POWER_7dBm);
//				log_i("after set wifi power get is %d", WiFi.getTxPower());
				if((WiFi.getMode() == WIFI_MODE_APSTA) || (WiFi.getMode() == WIFI_MODE_STA)){
					wifiMulti->run();
					WiFi.setAutoReconnect(true);
					WiFi.persistent(true);

				}

//				if((WiFi.getMode() == WIFI_MODE_APSTA) || (WiFi.getMode() == WIFI_MODE_AP)){
//					WiFi.softAP(iniWireless->_apSSID.c_str(), iniWireless->_apPW.c_str());
////					wifiMulti->run();
////					WiFi.setAutoReconnect(true);
////					WiFi.persistent(true);
//
//				}
			}
		}
//
		if((WiFi.getMode() == WIFI_MODE_APSTA) || (WiFi.getMode() == WIFI_MODE_STA)){
			if(WiFi.localIP().toString().equals("0.0.0.0")){
				wifiMulti->run();
			}
		}

		counter = 0;
		while(true){
			esp_task_wdt_reset();

			if(counter >= iniWireless->_loopDelay){
				break;
			}
			if(previousConnection != iniWireless->connectionAs){
				previousConnection = iniWireless->connectionAs;
				break;
			}
			counter += 100;
			delay(100);
		}
	}
}

//========================================================
LocWireless::LocWireless(int core, int loopDelay) {
	iniWireless = this;
	iniWireless->_loopDelay = loopDelay;

	wifiMulti = new WiFiMulti;
	wifiMulti->~WiFiMulti();	//optional

	// check if ssid dah ada atau tak
	bool data = false;
//	ManSpiff *ms = new ManSpiff();
//	String baca = ms->readFile("/ssid");

	WiFi.setTxPower(WIFI_POWER_2dBm);
	log_i("wifi power get is %d", WiFi.getTxPower());

	LocLittleFS *lfs = new LocLittleFS("LocWireless");
	String baca = lfs->readFile("/ssid");

	if(baca.length() > 20){
		data = !newWiFi;
	}
//	delete ms;
	delete lfs;

	if(data == false){
		iniWireless->WiFiSSID();
	}



	iniWireless->WiFiUse();


	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);


	xTaskCreatePinnedToCore(iniWireless->loop, "loop", 4000, NULL, 1, &handlerWireless, core);
}

void LocWireless::WiFiUse() {
//	ManSpiff	*ms;
	String temp;


//	ms = new ManSpiff();
//	temp = ms->readFile("/ssid");

	LocLittleFS *lfs = new LocLittleFS("LocWireless::WiFiUse()");

	DynamicJsonDocument jsonBasic(1024);

	temp = lfs->readFile("/ssid");



	deserializeJson(jsonBasic, temp);

//	serializeJson(jsonBasic["data"], Serial);



	JsonArray data = jsonBasic["data"];

	Serial.println();

	for(JsonVariant p : data){
//		log_i("dalam %s", ms->getValue(p.as<String>().c_str(), ',' ,0));

		if(lfs->getValue(p.as<String>().c_str(), ',' ,0).equals("sta")){
			log_i("STA");
			wifiMulti->addAP(
					lfs->getValue(p.as<String>().c_str(), ',' ,1).c_str(),
					lfs->getValue(p.as<String>().c_str(), ',' ,2).c_str()
					);
		}
		else if(lfs->getValue(p.as<String>().c_str(), ',' ,0).equals("ap")){
			log_i("AP");
//			iniWireless->_apSSID = lfs->getValue(p.as<String>().c_str(), ',' ,1);
//			iniWireless->_apPW = lfs->getValue(p.as<String>().c_str(), ',' ,2);
			WiFi.softAP(
					lfs->getValue(p.as<String>().c_str(), ',' ,1).c_str(),
					lfs->getValue(p.as<String>().c_str(), ',' ,2).c_str()
					);

		}


	}

	delete lfs;



}

void LocWireless::WiFiSSID() {
//	ManSpiff	*ms;
//
//	ms = new ManSpiff();

	LocLittleFS *lfs = new LocLittleFS("LocWireless::WiFiUse()");

	DynamicJsonDocument jsonBasic(3000);

	JsonArray rows = jsonBasic.createNestedArray("data");

	rows.add("sta,ideapad,sawabatik1");
	rows.add("sta,AndroidAP,sawabatik");
//	rows.add("sta,GF-Technical-2.4Ghz,gr33nf1nd3r");
//	rows.add("sta,GreenFinderIOT,0xadezcsw1");
//	rows.add("ap,TestZippy,123qweasd");
	rows.add("ap,GreenFinderIOT,0xadezcsw1");

	String data;
	serializeJson(jsonBasic, data);
	lfs->saveFile("/ssid", data.c_str());

//	ms->listDir("/", 4);






}

LocWireless::~LocWireless() {
//	log_i("LocWireless::~LocWireless(A)");
	WiFi.disconnect(true,true);
	vTaskDelete(handlerWireless);

//	log_i("LocWireless::~LocWireless(B)");

}



//	LocSpiff *ls;
//	ls = new LocSpiff("LocWiFi::_openConnection");
//	_sList.clear();
//	_sList = ls->readCSV("/ssid.txt");
//	delete ls;
//
//	for(int x=0; x < _sList.size(); x++){
//		auto data = _sList.at(x);
//
//		if(data.at(0) == "sta"){
//			log_i("Make STA");
//			wifiMulti->addAP(data.at(1).c_str(), data.at(2).c_str());
//		}
//		if( data.at(0) == "ap"){
//			log_i("Make AP");
//			WiFi.softAP(data.at(1).c_str(), data.at(2).c_str());
//			log_i("IP=%s", WiFi.softAPIP().toString().c_str());
//		}
//	}
