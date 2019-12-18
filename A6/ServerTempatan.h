/*
 * ServerTempatan.h
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#ifndef SERVERTEMPATAN_H_
#define SERVERTEMPATAN_H_

#include "Arduino.h"
#include <WebServer.h>
#include <WiFi.h>
#include "JsonHandler.h"
#include "timing.h"
#include <ArduinoOTA.h>
#include "Mando.h"
#include "Lantern.h"
#include "LReader.h"
#include "pages.h"
#include <ESPmDNS.h>
#include "esp_task_wdt.h"

class ServerTempatan {
public:
	ServerTempatan(const BaseType_t xCoreID, uint64_t loopDelay, Mando * mando, Lantern * lantern, LReader * lanternReader);
	ServerTempatan(const BaseType_t xCoreID, uint64_t loopDelay, Mando * mando, Lantern * lantern, LReader * lanternReader, timing * tim);
	virtual ~ServerTempatan();

	const String& getAlert() const {
		return alert;
	}

	void setAlert(const String& alert) {
		this->alert = alert;
	}

	void setAppCommPort(const String& appCommPort = "Searching") {
		AppCommPort = appCommPort;
	}

	void setBinFile(const String& binFile = "") {
		this->binFile = binFile;
	}

	void setJsonSpiffs(const String& jsonSpiffs = "") {
		JSONSpiffs = jsonSpiffs;
	}

private:
	void setup();
	static void loop(void * lantern);
	static bool nine();
	String makeInit();
	String makeJson();
	static void StatusViaWiFi();

	String alert;
	String AppCommPort = "Searching";
	String binFile = "";
	uint64_t _loopDelay;
	Mando * _oMando;
	Lantern * _oLantern;
	LReader * _oLanReader;
	timing * _oTiming;

	String JSONTypeOfAton = "00- Default,01- Reference point,02- RACON,03- Fix structure off-shore,04- Spare for future use,05- Light without sector,06- Light with sector,07- Leading light front,08- Leading light rear,09- Beacon Cardinal N,10- Beacond Cardinal E,11- Beacon Cardinal S,12- Beacon Cardinal W,13- Beacon Port hand,14- Beacon Starboard hand,15- Beacon Preferred Channel port hand,16- Beacon Preferred Channel Starboard hand,17- Beacon Isolated danger,18- Beacon safe water,19- Beacon special mark,20- Cardinal Mark N,21- Cardinal Mark E,22- Cardinal Mark S,23-Cardinal Mark W,24- Port hand Mark,25- Starboard hand Mark,26- Preferred Channel Port hand,27- Preferred Channel Starboard hand,28- Isolated danger,29- Safe water,30- Special mark,31- Light Vessel/LANBY/Rigs";
	String JSONEPFS = "1- GPS,2- GLONASS,3- Combined GPS/GLONASS,4- Loran-C,5- Chayka,6- Integrated Navigation System,7- Surveyed,8- Galileo";
	String JSONCardinalLat = "N,S";
	String JSONCardinalLng = "E,W";
	String JSONFormat = "GF-SC35,A126-SC35,GF-LR-BUOY,GF-LR-BEACON,GF-LR-LIGHTHOUSE";  //1. GF/SC-35 = Bouy Malaysia. Can use Lantern Reader or SC-35; 2. A126 = Bouy International. 3. SPAR-K2 = Beacon Malaysia
	String JSONMonitorPrimary = "Yes,No";
	String JSONMonitorSecondary = "Yes,No";
	String JSONCalibrate = "Auto,No,Manual";
	String JSONMonitorRACON = "No,10Vin,Contact ON,Contact Fail";
	String JSONPosAcc = "0- Low,1- High";
	String JSONUseLDR = "No,Yes";
	String JSONLightMode = "Fix,AI";
	String JSONRebootMode = "No,1Min every 3Days,1Min every 5Min,1Min every 1Day";

	String JSONSpiffs = "";

};

#endif /* SERVERTEMPATAN_H_ */
