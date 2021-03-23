/*
 * LocWireless.h
 *
 *  Created on: Sep 19, 2020
 *      Author: annuar
 */

#ifndef LOCAL_LOCWIRELESS_H_
#define LOCAL_LOCWIRELESS_H_

#include "Arduino.h"
//#include <LocSpiff.h>
#include "esp_task_wdt.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include "ArduinoJson.h"
//#include <ManSpiff.h>
#include "LocLittleFS.h"
#include "ArduinoJson.h"

#define newWiFi true

class LocWireless {
private:
	std::vector<std::vector<String>> _sList;
	int 	_loopDelay = 10000;
	wifi_mode_t		connectionAs = WIFI_MODE_NULL;
	String _apSSID;
	String _apPW;
public:
	LocWireless(int core, int loopDelay);
	static void connect(wifi_mode_t as);
	static void loop(void * parameter);
	virtual ~LocWireless();
	void WiFiSSID();
	void WiFiUse();

};

#endif /* LOCAL_LOCWIRELESS_H_ */
