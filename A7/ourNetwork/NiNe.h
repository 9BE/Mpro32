/*
 * NiNe.h
 *
 *  Created on: Sep 24, 2020
 *      Author: suhaimi
 */

#ifndef OURNETWORK_NINE_H_
#define OURNETWORK_NINE_H_

#include "Arduino.h"
#include "WebServer.h"
#include "ArduinoJson.h"
#include "DataHandler.h"

#define SAIZ_BRG_SPIFFS 7 //content lantern kene masuk kat spiff jugak. malas read from lantern.
#define SAIZ_BRG_MANDO 27

class NiNe {
private:
	static bool nine();
	String makeInit();
	String makeJson();
	void tambahRow(JsonArray root, String tab, String type, String e1, String e2);
	String getValue(String data, char separator, int index);
	String cariDanTukar(String data, char separator, String cari);
	void processSubmitJson(String json);
	bool spiffsKe(String e1, String e2);
	bool mandoKe(String e1, String e2);

	String _JSONSwitchMode = "All On,Pri-sec mode,A-B mode";
	const char * _ArraySwitchMode[3] = {"All On", "Pri-sec mode", "A-B mode"};


	String JSONTypeOfAton = "00- Default,01- Reference point,02- RACON,03- Fix structure off-shore,04- Spare for future use,05- Light without sector,06- Light with sector,07- Leading light front,08- Leading light rear,09- Beacon Cardinal N,10- Beacond Cardinal E,11- Beacon Cardinal S,12- Beacon Cardinal W,13- Beacon Port hand,14- Beacon Starboard hand,15- Beacon Preferred Channel port hand,16- Beacon Preferred Channel Starboard hand,17- Beacon Isolated danger,18- Beacon safe water,19- Beacon special mark,20- Cardinal Mark N,21- Cardinal Mark E,22- Cardinal Mark S,23-Cardinal Mark W,24- Port hand Mark,25- Starboard hand Mark,26- Preferred Channel Port hand,27- Preferred Channel Starboard hand,28- Isolated danger,29- Safe water,30- Special mark,31- Light Vessel/LANBY/Rigs";
	String JSONEPFS = "1- GPS,2- GLONASS,3- Combined GPS/GLONASS,4- Loran-C,5- Chayka,6- Integrated Navigation System,7- Surveyed,8- Galileo";
	String JSONCardinalLat = "N,S";
	String JSONCardinalLng = "E,W";
	String JSONMonitorRACON = "No,10Vin,Contact ON,Contact Fail";
	String JSONPosAcc = "0- Low,1- High";
	String JSONLanternMon = "Local,ASO";

	String brgSpiffs[SAIZ_BRG_SPIFFS] = {"DAC", "FI", "Dest. ID", "RACON Mon.", "LANTERN Mon.", "Detect current (A)", "Flash-Gap (s)"};
	String brgMando[SAIZ_BRG_MANDO] = {"Name", "MMSI", "Type", "Len A", "Len B", "Len C", "Len D", "OffPos. Thres.", "EPFS", "Pos.acc.", "Lat", "Card. Lat", "Lng", "Card. Lng.", "M6 A UTC h", "M6 A UTC m", "M6 A Int.", "M6 B UTC h", "M6 B UTC m", "M6 B Int.", "M21 A UTC h", "M21 A UTC m", "M21 A Int.", "M21 B UTC h", "M21 B UTC m", "M21 B Int."};
	String brgPosAcc[2] = {"0- Low", "1- High"};

	WebServer * _server = NULL;

	String _mID = "000000";
	DataHandler * _nData;
public:
	NiNe(WebServer * server, String mID);

	virtual ~NiNe();

	void setData(DataHandler* data) {
		_nData = data;
	}

	struct lampConfig {
		char adaBaru = 0;
		int switchMode = 1;
		int reportInterval = 0;
		double shortThres = 10;
		// Lantern 1
		double currentThresL1 = 0.8;
		int falseGuardTimeL1 = 10;
		int flashGapL1 = 30;
		// Lantern 2
		double currentThresL2 = 0.8;
		int falseGuardTimeL2 = 10;
		int flashGapL2 = 30;

	}LampConfig;



};

#endif /* OURNETWORK_NINE_H_ */
