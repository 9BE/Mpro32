/*
 * JsonHandler.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: suhaimi
 */

#include "JsonHandler.h"

//extern SpiffsHandler *spiffsHandler;
//extern ais Ais;
//extern board Board;
String JSONHopperCount = "0,1,2";
String JSONSonar = "Installed,Not installed";

JsonHandler::JsonHandler() {
	// TODO Auto-generated constructor stub

}

JsonHandler::~JsonHandler() {
	// TODO Auto-generated destructor stub
//	log_i("------------------------------------------------------ musnah @ %s", siapaPanggil);
}

JsonHandler::JsonHandler(Mando* mando) {
	_oMando = mando;
}

JsonHandler::JsonHandler(Mando* mando, const char* siapaPanggil) {
	_oMando = mando;
	log_i("------------------------------------------------------ bina @ %s", siapaPanggil);
}

String JsonHandler::checkConfigValue(String data, bool freshSetup) {
	String res = "OK";
	DynamicJsonDocument jDoc(2048);
	DeserializationError jerr = deserializeJson(jDoc, data);
	bool err = false;

	if (freshSetup)	err = true;

	if (jerr.code() == DeserializationError::Ok && !err) {
		int x = 0;
		if (jDoc["Beat"].isNull() || !strcmp(jDoc["Beat"], ""))									x++;
		if (jDoc["Reboot Mode"].isNull() || !strcmp(jDoc["Reboot Mode"], ""))					x++;
		if (jDoc["Light Detect Method"].isNull() || !strcmp(jDoc["Light Detect Method"], ""))	x++;
		if (jDoc["Format"].isNull() || !strcmp(jDoc["Format"], ""))								x++;
		if (jDoc["DAC"].isNull() || !strcmp(jDoc["DAC"], ""))									x++;
		if (jDoc["FI"].isNull() || !strcmp(jDoc["FI"], ""))										x++;
		if (jDoc["Dest. ID"].isNull() || !strcmp(jDoc["Dest. ID"], ""))							x++;
		if (jDoc["UTC Offset"].isNull() || !strcmp(jDoc["UTC Offset"], ""))						x++;
		if (jDoc["Sec. Mon."].isNull() || !strcmp(jDoc["Sec. Mon."], ""))						x++;
		if (jDoc["Lantern_Lost"].isNull() || !strcmp(jDoc["Lantern_Lost"], ""))					x++;
		if (jDoc["Use LDR"].isNull() || !strcmp(jDoc["Use LDR"], ""))							x++;
		if (jDoc["Calibrate"].isNull() || !strcmp(jDoc["Calibrate"], ""))						x++;
		if (jDoc["Calib. Prim. (M)"].isNull() || !strcmp(jDoc["Calib. Prim. (M)"], ""))			x++;
		if (jDoc["Calib. Sec. (M)"].isNull() || !strcmp(jDoc["Calib. Sec. (M)"], ""))			x++;
		if (jDoc["RACON Mon."].isNull() || !strcmp(jDoc["RACON Mon."], ""))						x++;

		log_i("x ====== %d", x);
		if (x>=1) {
			err = true;
		}
	}
	else {
		if (jerr) log_e("DeserializationError :: %s", jerr.c_str());
		else if (freshSetup) log_i("freshSetup = true");
		err = true;
	}

	if (err) {
		jDoc.clear();
		res="";
		jDoc["Beat"] = "0";
		jDoc["Reboot Mode"] = "1Min every 5Min";
		jDoc["Light Detect Method"] = "Fix";
		jDoc["Format"] = "GF-LR-BUOY";
		jDoc["DAC"] = "533";
		jDoc["FI"] = "1";
		jDoc["Dest. ID"] = "701";
		jDoc["UTC Offset"] = "8";
		jDoc["Sec. Mon."] = "No";
		jDoc["RACON Mon."] = "No";
		jDoc["Lantern_Lost"] = "0";
		jDoc["Use LDR"] = "No";
		jDoc["Calibrate"] = "No";
		jDoc["Calib. Prim. (M)"] = "30.0000";
		jDoc["Calib. Sec. (M)"] = "30.0000";
		serializeJson(jDoc, res);
	}
	else {
		serializeJsonPretty(jDoc, Serial);
	}

	jDoc.clear();

	return res;
}

void JsonHandler::extractConfData() {
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("JsonHandler::extractConfData()");

	String jData = locSpiff->readFile("/config");
	log_i("jData sebelum extractConfData ===== %s", jData.c_str());
	DynamicJsonDocument jDoc(4096);
	DeserializationError jerr = deserializeJson(jDoc, jData);

	log_e("extractConfData jerr :: %s", jerr.c_str());

//	ConfData = {jDoc["Beat"], jDoc["Reboot Mode"], jDoc["Light Detect Method"], jDoc["Format"], jDoc["DAC"], jDoc["FI"], jDoc["Dest. ID"],jDoc["UTC Offset"],
//			jDoc["Sec. Mon."], jDoc["RACON Mon."],jDoc["Lantern_Lost"],jDoc["Use LDR"],jDoc["Calibrate"],jDoc["Calib. Prim. (M)"],jDoc["Calib. Sec. (M)"]};

//	const char* niha;
//	niha = jDoc["Beat"];
//	log_i("ConfData.Beat -------------------------------------------------------- niha :: %s", niha);
//	ConfData.Beat = niha;
//	log_i("ConfData.Beat -------------------------------------------------------- jerr :: %s", ConfData.Beat);
//	niha = jDoc["Reboot Mode"];
//	ConfData.Reboot_Mode = niha;
//	niha = jDoc["Light Detect Method"];
//	ConfData.Light_Detect_Method = niha;
//	niha = jDoc["Format"];
//	ConfData.Format = niha;
//	niha = jDoc["DAC"];
//	ConfData.DAC = niha;
//	niha = jDoc["FI"];
//	ConfData.FI = niha;
//	niha = jDoc["Dest. ID"];
//	ConfData.Dest_ID = niha;
//	niha = jDoc["UTC Offset"];
//	ConfData.UTC_Offset = niha;
//	niha = jDoc["Sec. Mon."];
//	ConfData.Sec_Mon = niha;
//	niha = jDoc["RACON Mon."];
//	ConfData.RACON_Mon = niha;
//	niha = jDoc["Lantern_Lost"];
//	ConfData.Lantern_Lost = niha;
//	niha = jDoc["Use LDR"];
//	ConfData.Use_LDR = niha;
//	niha = jDoc["Calibrate"];
//	ConfData.Calibrate = niha;
//	niha = jDoc["Calib. Prim. (M)"];
//	ConfData.Calib_Prim_M = niha;
//	niha = jDoc["Calib. Sec. (M)"];
//	ConfData.Calib_Sec_M = niha;

//	log_e("extractConfData jerr :: %s", jerr.c_str());
//	const char* niha = jDoc["Beat"];
//	log_i("ConfData.Beat -------------------------------------------------------- niha :: %s", niha);
//	ConfData.Beat = jDoc["Beat"].as<String>();
//	log_i("ConfData.Beat -------------------------------------------------------- jerr :: %s", jDoc["Beat"]);
//	ConfData.Reboot_Mode = jDoc["Reboot Mode"].as<String>();
//	ConfData.Light_Detect_Method = jDoc["Light Detect Method"].as<String>();
//	ConfData.Format = jDoc["Format"].as<String>();
//	ConfData.DAC = jDoc["DAC"].as<String>();
//	ConfData.FI = jDoc["FI"].as<String>();
//	ConfData.Dest_ID = jDoc["Dest. ID"].as<String>();
//	ConfData.UTC_Offset = jDoc["UTC Offset"].as<String>();
//	ConfData.Sec_Mon = jDoc["Sec. Mon."].as<String>();
//	ConfData.RACON_Mon = jDoc["RACON Mon."].as<String>();
//	ConfData.Lantern_Lost = jDoc["Lantern_Lost"].as<String>();
//	ConfData.Use_LDR = jDoc["Use LDR"].as<String>();
//	ConfData.Calibrate = jDoc["Calibrate"].as<String>();
//	ConfData.Calib_Prim_M = jDoc["Calib. Prim. (M)"].as<String>();
//	ConfData.Calib_Sec_M = jDoc["Calib. Sec. (M)"].as<String>();

	const char* niha = jDoc["Beat"];
	log_i("ConfData.Beat -------------------------------------------------------- niha :: %s", niha);
	_oMando->SpiffsData.Beat = jDoc["Beat"].as<String>();
//	log_i("_oMando->ConfData.Beat -------------------------------------------------------- jerr :: %s", _oMando->SpiffsData.Beat.c_str());
	_oMando->SpiffsData.Reboot_Mode = jDoc["Reboot Mode"].as<String>();
	_oMando->SpiffsData.Light_Detect_Method = jDoc["Light Detect Method"].as<String>();
	_oMando->SpiffsData.Format = jDoc["Format"].as<String>();
	_oMando->SpiffsData.DAC = jDoc["DAC"].as<String>();
	_oMando->SpiffsData.FI = jDoc["FI"].as<String>();
	_oMando->SpiffsData.Dest_ID = jDoc["Dest. ID"].as<String>();
	_oMando->SpiffsData.UTC_Offset = jDoc["UTC Offset"].as<String>();
	_oMando->SpiffsData.Sec_Mon = jDoc["Sec. Mon."].as<String>();
	_oMando->SpiffsData.RACON_Mon = jDoc["RACON Mon."].as<String>();
	_oMando->SpiffsData.Lantern_Lost = jDoc["Lantern_Lost"].as<String>();
	_oMando->SpiffsData.Use_LDR = jDoc["Use LDR"].as<String>();
	_oMando->SpiffsData.Calibrate = jDoc["Calibrate"].as<String>();
	_oMando->SpiffsData.Calib_Prim_M = jDoc["Calib. Prim. (M)"].as<String>();
	_oMando->SpiffsData.Calib_Sec_M = jDoc["Calib. Sec. (M)"].as<String>();


	jDoc.clear();

	delete locSpiff;

}

bool JsonHandler::simpanConfData() {
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("JsonHandler::simpanConfData()");

	String jData = locSpiff->readFile("/config.json");
	log_i("jData sebelum simpanConfData ===== %s", jData.c_str());
	DynamicJsonDocument jDoc(2048);
	DeserializationError jerr = deserializeJson(jDoc, jData);

	log_e("simpanConfData jerr :: %s", jerr.c_str());

	if (jerr.code() == DeserializationError::Ok) {
		int x = 0;
		if (strcmp(jDoc["Beat"], _oMando->SpiffsData.Beat.c_str())) {
			x++;
			jDoc["Beat"] = _oMando->SpiffsData.Beat;
		}
		if (strcmp(jDoc["Reboot Mode"], _oMando->SpiffsData.Reboot_Mode.c_str())) {
			x++;
			jDoc["Reboot Mode"] = _oMando->SpiffsData.Reboot_Mode.c_str();
		}
		if (strcmp(jDoc["Light Detect Method"], _oMando->SpiffsData.Light_Detect_Method.c_str())) {
			x++;
			jDoc["Light Detect Method"] = _oMando->SpiffsData.Light_Detect_Method.c_str();
		}
		if (strcmp(jDoc["Format"], _oMando->SpiffsData.Format.c_str())) {
			x++;
			jDoc["Format"] = _oMando->SpiffsData.Format.c_str();
		}
		if (strcmp(jDoc["DAC"], _oMando->SpiffsData.DAC.c_str())) {
			x++;
			jDoc["DAC"] = _oMando->SpiffsData.DAC.c_str();
		}
		if (strcmp(jDoc["FI"], _oMando->SpiffsData.FI.c_str())) {
			x++;
			jDoc["FI"] = _oMando->SpiffsData.FI.c_str();
		}
		if (strcmp(jDoc["Dest. ID"], _oMando->SpiffsData.Dest_ID.c_str())) {
			x++;
			jDoc["Dest. ID"] = _oMando->SpiffsData.Dest_ID.c_str();
		}
		if (strcmp(jDoc["UTC Offset"], _oMando->SpiffsData.UTC_Offset.c_str())) {
			x++;
			jDoc["UTC Offset"] = _oMando->SpiffsData.UTC_Offset.c_str();
		}
		if (strcmp(jDoc["Sec. Mon."], _oMando->SpiffsData.Sec_Mon.c_str())) {
			x++;
			jDoc["Sec. Mon."] = _oMando->SpiffsData.Sec_Mon.c_str();
		}
		if (strcmp(jDoc["Lantern_Lost"], _oMando->SpiffsData.RACON_Mon.c_str())) {
			x++;
			jDoc["RACON Mon."] = _oMando->SpiffsData.RACON_Mon.c_str();
		}
		if (strcmp(jDoc["Use LDR"], _oMando->SpiffsData.Lantern_Lost.c_str())) {
			x++;
			jDoc["Lantern_Lost"] = _oMando->SpiffsData.Lantern_Lost.c_str();
		}
		if (strcmp(jDoc["Calibrate"], _oMando->SpiffsData.Use_LDR.c_str())) {
			x++;
			jDoc["Use LDR"] = _oMando->SpiffsData.Use_LDR.c_str();
		}
		if (strcmp(jDoc["Calib. Prim. (M)"], _oMando->SpiffsData.Calibrate.c_str())) {
			x++;
			jDoc["Calibrate"] = _oMando->SpiffsData.Calibrate.c_str();
		}
		if (strcmp(jDoc["Calib. Sec. (M)"], "")) {
			x++;
			jDoc["Calib. Prim. (M)"] = _oMando->SpiffsData.Calib_Prim_M.c_str();
		}
		if (strcmp(jDoc["RACON Mon."], "")) {
			x++;
			jDoc["Calib. Sec. (M)"] = _oMando->SpiffsData.Calib_Prim_M.c_str();
		}

		log_i("x ====== %d", x);
		if (x>=1) {
			jData = "";
			serializeJson(jDoc, jData);

			log_i("jData lepas simpanConfData ===== %s", jData.c_str());

			if (locSpiff->writeFile("/config.json", jData.c_str())) {
				log_i("simpanConfData setel");
			}
		}
	}else {
		jDoc.clear();
		jDoc["Beat"] = _oMando->SpiffsData.Beat;
		jDoc["Reboot Mode"] = _oMando->SpiffsData.Reboot_Mode.c_str();
		jDoc["Light Detect Method"] = _oMando->SpiffsData.Light_Detect_Method.c_str();
		jDoc["Format"] = _oMando->SpiffsData.Format.c_str();
		jDoc["DAC"] = _oMando->SpiffsData.DAC.c_str();
		jDoc["FI"] = _oMando->SpiffsData.FI.c_str();
		jDoc["Dest. ID"] = _oMando->SpiffsData.Dest_ID.c_str();
		jDoc["UTC Offset"] = _oMando->SpiffsData.UTC_Offset.c_str();
		jDoc["Sec. Mon."] = _oMando->SpiffsData.Sec_Mon.c_str();
		jDoc["RACON Mon."] = _oMando->SpiffsData.RACON_Mon.c_str();
		jDoc["Lantern_Lost"] = _oMando->SpiffsData.Lantern_Lost.c_str();
		jDoc["Use LDR"] = _oMando->SpiffsData.Use_LDR.c_str();
		jDoc["Calibrate"] = _oMando->SpiffsData.Calibrate.c_str();
		jDoc["Calib. Prim. (M)"] = _oMando->SpiffsData.Calib_Prim_M.c_str();
		jDoc["Calib. Sec. (M)"] = _oMando->SpiffsData.Calib_Prim_M.c_str();

		jData = "";
		serializeJson(jDoc, jData);

		log_i("jData lepas simpanConfData ===== %s", jData.c_str());

		if (locSpiff->writeFile("/config.json", jData.c_str())) {
			log_i("simpanConfData setel");
		}
	}





	jDoc.clear();

//	delay(100);

	delete locSpiff;

	return true;
}
//jDoc["i"] = "1";
//	jDoc["n"] = "UNKNOWN";
//	jDoc["j"] = "d";
//	jDoc["c"] = "I4N2IP.png";
//	jDoc["s"] = "";
//	jDoc["m"] = "A00000";
//	jDoc["e"] = "10800000";
//
//	JsonArray g = jDoc.createNestedArray("g");
//	g.add("ABCDEF");
////	g.add("G1OFJV");

//void JsonHandler::processAdTech(String json)
//{
//	const char* e1;
//	const char* m="";
//	const char* gr;
//	String g="";
//	JsonArray groups;
//
//	bool ada = false;
//	DynamicJsonDocument jDoc(2048);
//	deserializeJson(jDoc, json);
//	JsonArray rows = jDoc["rows"];
//
//
//	for (JsonObject aRow : rows) {
//		e1 = aRow["element"][0]["e1"][0];
//
//		if (!strcmp(e1,"Machine ID")) {
//			m = aRow["element"][1]["e2"][0];
//			ada = true;
//		}
//		else if (!strcmp(e1, "Group")) {
//			groups = aRow["element"][1]["e2"];
//			ada = true;
//		}
//	}
//
//	if (ada) {
//		hantu("ada");
////		jDoc.clear();
////		String initJson = spiffsHandler->readFromSpiffs("init");
////		deserializeJson(jDoc, initJson);
//		if (m && m[0]) {
//			mID = m;
//			spiffsHandler->writeToSpiffs("m", m);
//			hantu("m==" +mID);
//		}
//
//		if (groups.size() > 0) {
//			for (int i=0; i<groups.size(); i++) {
//				gr = groups[i];
//				g += gr;
//				if (groups.size() > 1 && i < groups.size()-1) g += ",";
//			}
//			mGroup = g;
//			spiffsHandler->writeToSpiffs("g", g);
//			hantu("g==" + mGroup);
//		}
//
//	}
//
//
//}
//
String JsonHandler::cariDanTukar(String data, char separator, String cari)
{
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;
	int ioff = -1;
	bool ada = false;
	String temp = "";

	for (int i = 0; i <= maxIndex; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
			ioff = data.substring(strIndex[0], strIndex[1]).indexOf(cari);
			if (ioff >= 0) {
				ada = true;
				break;
			}
		}
	}

	if (ada) {
		temp = data.substring(strIndex[0], strIndex[1]);
		data.remove(strIndex[1] > maxIndex ? strIndex[0] - 1 : strIndex[0] , temp.length() + 1 );
		temp += ",";
		temp += data;
	}

	return ada ? temp : data;
}
//
void JsonHandler::processSubmitJSON(String json)
{
	DynamicJsonDocument jsonObjSubmit(15*1024);
	DeserializationError jerr = deserializeJson(jsonObjSubmit, json);

	log_e("simpanConfData jerr :: %s", jerr.c_str());


	JsonArray rows = jsonObjSubmit["rows"];
	boolean sk = false, mk = false, lk = false;
	for (JsonObject aRow : rows) {
		const char* e1 = aRow["element"][0]["e1"][0];
		const char* e2 = aRow["element"][1]["e2"][0];
		if (spiffsKe(e1, e2)) {
			sk = true;
		}
		if (mandoKe(e1, e2)) {
			mk = true;
		}
	}

	if (sk) {
		simpanConfData();
		delay(1000);
	}

	if (mk) {
		log_i("Configuring mando");
		_oMando->setNakConfig(true);
	}

}

bool JsonHandler::spiffsKe(String e1, String e2) {
	bool ada = false;
	int noBrg = -1;
	for (int i = 0; i < SAIZ_BRG_SPIFFS; i++) {
		if (e1 == brgSpiffs[i]) {
			ada = true;
			break;
		}
	}

	if (ada) {
		if (e1 == "Beat") {
			_oMando->SpiffsData.Beat = e2;
		}
		else if (e1 == "Reboot Mode") {
			_oMando->SpiffsData.Reboot_Mode = e2;
		}
		else if (e1 == "Light Detect Method") {
			_oMando->SpiffsData.Light_Detect_Method = e2;
		}
		else if (e1 == "Format") {
			_oMando->SpiffsData.Format = e2;
		}
		else if (e1 == "DAC") {
			_oMando->SpiffsData.DAC = e2;
		}
		else if (e1 == "FI") {
			_oMando->SpiffsData.FI = e2;
		}
		else if (e1 == "Dest. ID") {
			_oMando->SpiffsData.Dest_ID = e2;
		}
		else if (e1 == "UTC Offset") {
			_oMando->SpiffsData.UTC_Offset = e2;
		}
		else if (e1 == "Sec. Mon.") {
			_oMando->SpiffsData.Sec_Mon = e2;
		}
		else if (e1 == "RACON Mon.") {
			_oMando->SpiffsData.RACON_Mon = e2;
		}
		else if (e1 == "Lantern_Lost") {
			_oMando->SpiffsData.Lantern_Lost = e2;
		}
		else if (e1 == "Use LDR") {
			_oMando->SpiffsData.Use_LDR = e2;
		}
		else if (e1 == "Calibrate") {
			_oMando->SpiffsData.Calibrate = e2;
		}
		else if (e1 == "Calib. Prim. (M)") {
			_oMando->SpiffsData.Calib_Prim_M = e2;
		}
		else if (e1 == "Calib. Sec. (M)") {
			_oMando->SpiffsData.Calib_Sec_M = e2;
		}

	}

	return ada;

}

bool JsonHandler::mandoKe(String e1, String e2) {

	bool ada = false;
	int noBrg;
	for (int i = 0; i < SAIZ_BRG_MANDO; i++) {
		if (e1 == brgMando[i]) {
			ada = true;
			noBrg = i;
			break;
		}
	}
	if (ada) {

		switch (noBrg) {
		case 0:
			_oMando->_mConfig.MName = e2;
			break;
		case 1:
			_oMando->_mConfig.MMmsi = e2;
			break;
		case 2:
			e2.substring(0, 2).toCharArray(_oMando->_mConfig.MAtonType, 3);
			break;
		case 3:
			_oMando->_mConfig.MLengthA = e2;
			break;
		case 4:
			_oMando->_mConfig.MLengthB = e2;
			break;
		case 5:
			_oMando->_mConfig.MLengthC = e2;
			break;
		case 6:
			_oMando->_mConfig.MLengthD = e2;
			break;
		case 7:
			_oMando->_mConfig.MOffThres = e2.toInt();
			break;
		case 8:
			e2.toCharArray(_oMando->_mConfig.MEpfs, 2);
			break;
		case 9:
			e2.toCharArray(_oMando->_mConfig.MPosAcc, 2);
			break;
		case 10:
			_oMando->_mConfig.MConfigLat = e2;  // MTempStrRegLat ni dah digunakan untuk string NMEA untuk AIS. bukan untuk configuration. kene buat string baru utk tujuan configure je.
			break;
		case 11:
			e2.toCharArray(_oMando->_mConfig.MRegCardinalLat, 2);
			break;
		case 12:
			_oMando->_mConfig.MConfigLng = e2;
			break;
		case 13:
			e2.toCharArray(_oMando->_mConfig.MRegCardinalLng, 2);
			break;
		case 14:
			_oMando->_mConfig.M06UTChourA = e2;
			break;
		case 15:
			_oMando->_mConfig.M06UTCminA = e2;
			break;
		case 16:
			_oMando->_mConfig.M06IntervalABahagi60 = e2;
			break;
		case 17:
			_oMando->_mConfig.M06UTChourB = e2;
			break;
		case 18:
			_oMando->_mConfig.M06UTCminB = e2;
			break;
		case 19:
			_oMando->_mConfig.M06IntervalBBahagi60 = e2;
			break;
		case 20:
			_oMando->_mConfig.M21UTChourA = e2;
			break;
		case 21:
			_oMando->_mConfig.M21UTCminA = e2;
			break;
		case 22:
			_oMando->_mConfig.M21IntervalABahagi60 = e2;
			break;
		case 23:
			_oMando->_mConfig.M21UTChourB = e2;
			break;
		case 24:
			_oMando->_mConfig.M21UTCminB = e2;
			break;
		case 25:
			_oMando->_mConfig.M21IntervalBBahagi60 = e2;
			break;
		}
	}
	return ada;

}
//
//DynamicJsonDocument JsonHandler::makeJson() {
//	DynamicJsonDocument jDoc(2048);
//
//	jDoc["m"] = mID; // XF7TDK
//	jDoc["n"] = mName;
//	jDoc["j"] = "d";
//
//	JsonArray g = jDoc.createNestedArray("g");
//	g.add("GF7TDK");
//	g.add("G1OFJV");
//
//	JsonObject r = jDoc.createNestedObject("r");
//	r["i"] = "Update List";
//	r["f"] = "##0.00";      //number format
//	r["c"] = "RM";        //currency
//	r["u"] = "0";       //currency
//
//	JsonObject tab = jDoc.createNestedObject("tabs");
//	tab["1"] = "Config";
//
//	JsonArray rows = jDoc.createNestedArray("rows");
//
//	tambahRow(rows, "1", "0", "Machine Detail", "");
//	tambahRow(rows, "1", "2", "MAC", Board.macID);
//	tambahRow(rows, "1", "3", "Name", mName);
//
//	tambahRow(rows, "1", "0", "Sonar Sensor", "");
//	String son = "";
//	if (Ais.sonarCount) son = "Installed";
//	else son = "Not installed";
//	tambahRow(rows, "1", "7", "Sonar", cariDanTukar(JSONSonar, ',', son));
//	tambahRow(rows, "1", "4", "Min", String(Ais.min));
//	tambahRow(rows, "1", "4", "Max", String(Ais.max));
//
//	tambahRow(rows, "1", "0", "Proximity Sensor", "");
//	tambahRow(rows, "1", "7", "Hopper", cariDanTukar(JSONHopperCount, ',', String(Ais.hopperCount)));
//
//	return jDoc;
//
//}
//
//DynamicJsonDocument JsonHandler::getInit()
//{
//	DynamicJsonDocument jDoc(2048);
//	deserializeJson(jDoc, spiffsHandler->readFromSpiffs("init"));
//	return jDoc;
//}
//
//DynamicJsonDocument JsonHandler::makeInit() {
//	DynamicJsonDocument jDoc(2048);
//
//	jDoc["i"] = "1";
//	jDoc["n"] = mName;
//	jDoc["j"] = "d";
//	jDoc["c"] = "I4N2IP.png";
//	jDoc["s"] = "";
//	jDoc["m"] = mID;
//	jDoc["e"] = "10800000";
//
//	JsonArray g = jDoc.createNestedArray("g");
////	int x;
////	String temp;
////	if (mGroup.length() > 0) {
////		x = 0;
////		while (true) {
////			temp = getValue(mGroup, ',', x);
////			if (temp != "") {
////				g.add(temp);
////			}
////			else break;
////			x++;
////		}
////	}
//	g.add("GF7TDK");
//	g.add("G1OFJV");
//
//	return jDoc;
//}
//
void JsonHandler::tambahRow(JsonArray root, String tab, String type, String e1, String e2)
{
	String temp;
	int x;

	JsonObject r = root.createNestedObject();
	r["tab"] = tab;
	r["type"] = type;

	JsonArray element = r.createNestedArray("element");


	JsonObject e = element.createNestedObject();
	JsonArray arg1 = e.createNestedArray("e1");
	arg1.add(e1);

	JsonObject f = element.createNestedObject();
	JsonArray arg2 = f.createNestedArray("e2");

	if (e2.length() > 0) {
		x = 0;
		while (true) {
			temp = getValue(e2, ',', x);
			if (temp != "") {
				arg2.add(temp);
			}
			else break;
			x++;
		}
	}

}

String JsonHandler::getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


//
