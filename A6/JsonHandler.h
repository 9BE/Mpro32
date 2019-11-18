/*
 * JsonHandler.h
 *
 *  Created on: Jun 24, 2019
 *      Author: suhaimi
 */

#ifndef JSONHANDLER_H_
#define JSONHANDLER_H_

#include "ArduinoJson.h"
#include "Arduino.h"
#include "LocSpiff.h"
#include "Mando.h"

#define SAIZ_BRG_SPIFFS 15 //content lantern kene masuk kat spiff jugak. malas read from lantern.
#define SAIZ_BRG_MANDO 27


class JsonHandler {
public:
	JsonHandler();
	JsonHandler(Mando * mando);
	JsonHandler(Mando * mando, const char * siapaPanggil);
	virtual ~JsonHandler();
	void tambahRow(JsonArray root, String tab, String type, String e1, String e2);
	String getValue(String data, char separator, int index);
	void processSubmitJSON(String json);
	String cariDanTukar(String data, char separator, String cari);
	void processAdTech(String json);
	DynamicJsonDocument getInit();
	String checkConfigValue(String data, bool freshSetup);
	void extractConfData();
	bool simpanConfData();
	bool spiffsKe(String e1, String e2);
	bool mandoKe(String e1, String e2);

	String mID;
	String mName;
	String mGroup;

	Mando * _oMando;

	String brgSpiffs[SAIZ_BRG_SPIFFS] = {"Beat", "Light Detect Method", "Reboot Mode", "Format", "DAC", "FI", "Dest. ID", "UTC Offset", "Sec. Mon.", "RACON Mon.", "Lantern_Lost", "Use LDR", "Calibrate", "Calib. Prim. (M)", "Calib. Sec. (M)"};
	String brgMando[SAIZ_BRG_MANDO] = {"Name", "MMSI", "Type", "Len A", "Len B", "Len C", "Len D", "OffPos. Thres.", "EPFS", "Pos.acc.", "Lat", "Card. Lat", "Lng", "Card. Lng.", "M6 A UTC h", "M6 A UTC m", "M6 A Int.", "M6 B UTC h", "M6 B UTC m", "M6 B Int.", "M21 A UTC h", "M21 A UTC m", "M21 A Int.", "M21 B UTC h", "M21 B UTC m", "M21 B Int."};

};


#endif /* JSONHANDLER_H_ */
