/*
 * NiNe.cpp
 *
 *  Created on: Sep 24, 2020
 *      Author: suhaimi
 */

#include "NiNe.h"

NiNe * ini;

NiNe::~NiNe() {
//	log_i("NiNe::~NiNe(A)");

	ini->_server->close();
	ini->_server->stop();
//	log_i("NiNe::~NiNe(B)");
}


NiNe::NiNe(WebServer* server, String mID) {
	ini = this;
	ini->_server = server;
	ini->_mID = mID;
	ini->_server->on("/nine", HTTP_POST, WebServer::THandlerFunction(ini->nine));
	ini->_server->on("/a", HTTP_GET, WebServer::THandlerFunction([](){
		ini->_server->send(200, "text/plain", "nampak tuuuu");
	}));
}

bool NiNe::nine() {
	log_i("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ nine ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	if (ini->_server->args() == 0) {
		ini->_server->send( 200, "application/json", ini->makeInit() );
	}
	else {
		if (ini->_server->argName(0) == "full") {
			ini->_server->send( 200, "application/json", ini->makeJson() );
		}
		else if (ini->_server->argName(0) == "submit") {
			ini->processSubmitJson(ini->_server->arg("plain"));
			ini->_server->send( 200, "application/json", ini->makeJson() );

		}
	}

	return true;
}

String NiNe::makeInit() {
	DynamicJsonDocument jsonBasic(1024);

	jsonBasic["i"] = "1";
	jsonBasic["n"] = this->_nData->machine.name; //asalnya Mando Pro
	jsonBasic["j"] = "d";
	jsonBasic["c"] = "I4N2IP.png";
	jsonBasic["s"] = "";
	jsonBasic["m"] = this->_nData->machine.id;
	jsonBasic["e"] = "10800000";

	JsonArray g = jsonBasic.createNestedArray("g");
	g.add("GF7TDK");
//	g.add("G3XJRL");

	String res;
	serializeJson(jsonBasic, res);
//	serializeJson(jsonBasic, Serial);
	jsonBasic.clear();
	return res;
}

String NiNe::makeJson() {
	log_i("makeJson() mulai");

	DynamicJsonDocument jsonBasic(15*1024);

	jsonBasic["m"] = this->_nData->machine.id;
	jsonBasic["n"] = "LAST";
	jsonBasic["j"] = "d";

	JsonArray g = jsonBasic.createNestedArray("g");
	g.add("GF7TDK");
//	g.add("G3XJRL");

	JsonObject r = jsonBasic.createNestedObject("r");
	r["i"] = "Update List";
	r["f"] = "##0.00";      //number format
	r["c"] = "RM";        //currency
	r["u"] = "0";       //currency
	r["ar"] = false;
	r["rr"] = "5";

	JsonObject tab = jsonBasic.createNestedObject("tabs");
	tab["1"] = "Monitor";
	tab["2"] = "Config";

	JsonArray rows = jsonBasic.createNestedArray("rows");
	tambahRow(rows, "1", "30", "Refresh", "");

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Monitor >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	tambahRow(rows, "1", "0", "Controller", "");
	tambahRow(rows, "1", "2", "Prog name", _mID);

	String jkl = "";

	jkl = _nData->nTiming->now.sMasa;
	jkl += (" | ");
	jkl += (_nData->nTiming->now.hari);
	jkl += ("/");
	jkl += (_nData->nTiming->now.bulan);
	jkl += ("/");
	jkl += (_nData->nTiming->now.tahun);
	jkl += (" | ");

	if (_nData->nTiming->ZoneTime == e_nite)		jkl += "Night";
	else if (_nData->nTiming->ZoneTime == e_dawn)	jkl += "Dawn";
	else if (_nData->nTiming->ZoneTime == e_day) 	jkl += "Daylight";
	else if (_nData->nTiming->ZoneTime == e_dusk)  	jkl += "Dusk";

	tambahRow(rows, "1", "2", "Clock", _nData->_jumpaGPS ? jkl : "No Data");
	tambahRow(rows, "1", "2", "Runtime (s)", String(millis()/1000));

	tambahRow(rows, "1", "0", "Sensors", "");

	tambahRow(rows, "1", "2", "Hatch Door", _nData->getDoorStatStr());
	tambahRow(rows, "1", "2", "LDR", _nData->getLDRStatStr());

	tambahRow(rows, "1", "0", "AIS", "");

	if (_nData->_configuringMando) {
		jkl = "";
		jkl += _nData->_confProgress;
		jkl += "/";
		jkl += _nData->_conFull;
		jkl += " items";

		if (_nData->_conFull - _nData->_confProgress == 0) {
			jkl = "Retrieve config data";
		}

		tambahRow(rows, "1", "2", "Configuring", jkl);
	}

	tambahRow(rows, "1", "2", "Comm AIS", _nData->_masaSerial > 0 ? (_nData->_mandoJaga ? "Active" : "Sleep") : "Searching . . .");

	jkl = "Found : ";
	jkl += String(_nData->_LAT < 0 ? (_nData->_LAT*=-1) : _nData->_LAT, 5);
	jkl += ";";
	jkl += String(_nData->_LNG < 0 ? (_nData->_LNG*=-1) : _nData->_LNG, 5);
	jkl += "\nConfig : ";
	if (_nData->_xTaskComplete == false) {
		jkl += "NDA";
	}
	else {
		jkl += _nData->mandoConf.MConfigLat;
		jkl += ";";
		jkl += _nData->mandoConf.MConfigLng;
	}

	if (_nData->_masaVdo21 > 0) {
		jkl += _nData->m6data.ProcOffPositionStatus ? "\nOff position" : "\nOn position";
	}



	tambahRow(rows, "1", "2", "GPS", _nData->_jumpaGPS ? jkl : "No Data");

	jkl = String(_nData->m6data.MVin, 2);
	jkl += "\n";
	jkl += _nData->mandoConf.MSerialNumber;
	jkl += "\n";
	jkl += _nData->procAtonBit_prev;
	jkl += "\n";
	jkl += _nData->_masaVdo21 > 0 ? String((millis()/1000UL) - _nData->_masaVdo21) : "NDA";
	jkl += "\n";
	jkl += _nData->_masaVdo6 > 0 ? String((millis() / 1000UL) - _nData->_masaVdo6) : "NDA";
	jkl += "\n";
	jkl += _nData->_masaVdo6 > 0 ? String(_nData->_masaVdo6 + 180) : "NDA";

	tambahRow(rows, "1", "2", "AIS V\nS/N\nAton Bit\nM21 ago\nM06 ago\nNext M06",  jkl);


	tambahRow(rows, "1", "0", "Lantern (" + _nData->confData.LANT_Mon + ")", "");
	if (_nData->confData.LANT_Mon == "Local") {
		tambahRow(rows, "1", "2", "Lantern V", String(_nData->m6data.LVin, 2));
		tambahRow(rows, "1", "2", "Pri Light", _nData->m6data.LNyala ? "On" : "Off");
	}else if (_nData->confData.LANT_Mon == "ASO") {
		String tpp = "";
		if (_nData->_masaASO == 0) {
			tpp = "NDA";
		}else {
			tpp = String((millis()/1000) - _nData->_masaASO);
			tpp += " s ago";
		}
		tambahRow(rows, "1", "2", "Comm", tpp);
		tambahRow(rows, "1", "2", "Lantern V", String(_nData->m6data.LVin, 2));
		tambahRow(rows, "1", "2", "Pri Light", _nData->m6data.LNyala ? "On" : "Off");
		tambahRow(rows, "1", "2", "Sec Light", _nData->m6data.SLNyala ? "On" : "Off");
	}



	// TODO coming soon


//	tambahRow(rows, "1", "1", "Coming soon ! ! !", "");

	if (!_nData->confData.RACON_Mon.equals("No")) {
		tambahRow(rows, "1", "0", "Racon", "");

		if (_nData->atonBitData.racon == 0) jkl = "Not installed";
		else if (_nData->atonBitData.racon == 1) jkl = "Not monitored";
		else if (_nData->atonBitData.racon == 2) jkl = "Operational";
		else if (_nData->atonBitData.racon == 3) jkl = "Error";

		tambahRow(rows, "1", "2", "RACON V | Stat", String(_nData->m6data.RVin) + " | " + jkl);
	}



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CONFIG >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////



	tambahRow(rows, "2", "0", "Controller", "");
	tambahRow(rows, "2", "4", "DAC", _nData->confData.DAC);
	tambahRow(rows, "2", "4", "FI", _nData->confData.FI);
	tambahRow(rows, "2", "4", "Dest. ID", _nData->confData.Dest_ID);
	tambahRow(rows, "2", "7", "RACON Mon.", cariDanTukar(JSONMonitorRACON, ',', _nData->confData.RACON_Mon));
	tambahRow(rows, "2", "7", "LANTERN Mon.", cariDanTukar(JSONLanternMon, ',', _nData->confData.LANT_Mon));
	tambahRow(rows, "2", "1", "Two items below applicable if LANTERN Mon. set to \"Local\"", "");
	tambahRow(rows, "2", "5", "Detect current (A)", String(_nData->confData.currentThres));
	tambahRow(rows, "2", "4", "Flash-Gap (s)", String(_nData->confData.flashGap));

	if (_nData->_xTaskComplete == false) {
		tambahRow(rows, "2", "0", "AIS - Station (Loading...)", "");
	}
	else {
		tambahRow(rows, "2", "0", "AIS - Station", "");
		tambahRow(rows, "2", "3", "Name", _nData->mandoConf.MName);
		tambahRow(rows, "2", "4", "MMSI", _nData->mandoConf.MMmsi);      //how? MMmsi ni dia char. takpe ke? auto string, eh? kalau betul, kiranya kita hnya ada masalah utk serialprint char je la.
		tambahRow(rows, "2", "7", "Type", cariDanTukar(JSONTypeOfAton, ',', _nData->mandoConf.MAtonType));
		tambahRow(rows, "2", "4", "Len A", String(_nData->mandoConf.MLengthA.toInt()));
		tambahRow(rows, "2", "4", "Len B", String(_nData->mandoConf.MLengthB.toInt()));
		tambahRow(rows, "2", "4", "Len C", String(_nData->mandoConf.MLengthC.toInt()));
		tambahRow(rows, "2", "4", "Len D", String(_nData->mandoConf.MLengthD.toInt()));
		tambahRow(rows, "2", "4", "OffPos. Thres.", String(_nData->mandoConf.MOffThres));
		tambahRow(rows, "2", "7", "EPFS", cariDanTukar(JSONEPFS, ',', _nData->mandoConf.MEpfs));
		tambahRow(rows, "2", "7", "Pos.acc.", cariDanTukar(JSONPosAcc, ',', brgPosAcc[_nData->mandoConf.MPosAcc.toInt()]));

		tambahRow(rows, "2", "5", "Lat", _nData->mandoConf.MConfigLat);
		tambahRow(rows, "2", "7", "Card. Lat", cariDanTukar(JSONCardinalLat, ',', _nData->mandoConf.MRegCardinalLat)); // "N,S"     // SUHAIMI =--=-=-=- Contoh utk Type -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=- SUHAIMI
		tambahRow(rows, "2", "5", "Lng", _nData->mandoConf.MConfigLng);
		tambahRow(rows, "2", "7", "Card. Lng.", cariDanTukar(JSONCardinalLng, ',', _nData->mandoConf.MRegCardinalLng)); // "E,W"
	}

	if (_nData->_xTaskComplete == false) {
		tambahRow(rows, "2", "0", "AIS - Msg Scheduling (Loading...)", "");
	}
	else {
		tambahRow(rows, "2", "0", "AIS - Msg Scheduling", "");
		tambahRow(rows, "2", "4", "M6 A UTC h", _nData->mandoConf.M06UTChourA);
		tambahRow(rows, "2", "4", "M6 A UTC m", _nData->mandoConf.M06UTCminA);
		tambahRow(rows, "2", "4", "M6 A Int.", String(_nData->mandoConf.M06IntervalA / 60));
		tambahRow(rows, "2", "4", "M6 B UTC h", _nData->mandoConf.M06UTChourB);
		tambahRow(rows, "2", "4", "M6 B UTC m", _nData->mandoConf.M06UTCminB);
		tambahRow(rows, "2", "4", "M6 B Int.", String(_nData->mandoConf.M06IntervalB / 60));
		tambahRow(rows, "2", "4", "M21 A UTC h", _nData->mandoConf.M21UTChourA);
		tambahRow(rows, "2", "4", "M21 A UTC m", _nData->mandoConf.M21UTCminA);
		tambahRow(rows, "2", "4", "M21 A Int.", String(_nData->mandoConf.M21IntervalA / 60));
		tambahRow(rows, "2", "4", "M21 B UTC h", _nData->mandoConf.M21UTChourB);
		tambahRow(rows, "2", "4", "M21 B UTC m", _nData->mandoConf.M21UTCminB);
		tambahRow(rows, "2", "4", "M21 B Int.", String(_nData->mandoConf.M21IntervalB / 60));

	}



//	tambahRow(rows, "1", "7", "Switching mode", cariDanTukar(_JSONSwitchMode, ',', _ArraySwitchMode[ini->LampConfig.switchMode]));
////	tambahRow(rows, "1", "4", "Reporting interval (s)", String(ini->LampConfig.reportInterval));
////	tambahRow(rows, "1", "5", "Short-circuit thres (A)", String(ini->LampConfig.shortThres));
//
//	tambahRow(rows, "1", "0", "Lantern 1", "");
//
//	tambahRow(rows, "1", "5", "1 Detect current (A)", String(ini->LampConfig.currentThresL1));
//	tambahRow(rows, "1", "4", "2 False-Guard (s)", String(ini->LampConfig.falseGuardTimeL1));
//	log_i(" dalam nine %d", ini->LampConfig.falseGuardTimeL1); //===============================debug
//	tambahRow(rows, "1", "4", "3 Flash-Gap (s)", String(ini->LampConfig.flashGapL1));
//
//	tambahRow(rows, "1", "0", "Lantern 2", "");
//
//	tambahRow(rows, "1", "5", "4 Detect current (A)", String(ini->LampConfig.currentThresL2));
//	tambahRow(rows, "1", "4", "5 False-Guard (s)", String(ini->LampConfig.falseGuardTimeL2));
//	tambahRow(rows, "1", "4", "6 Flash-Gap (s)", String(ini->LampConfig.flashGapL2));

	String res;
	serializeJson(jsonBasic, res);
	jsonBasic.clear();

	log_i("habisi");
	return res;
}

void NiNe::tambahRow(JsonArray root, String tab, String type, String e1,
		String e2) {

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

String NiNe::getValue(String data, char separator, int index) {

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

String NiNe::cariDanTukar(String data, char separator, String cari) {
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

void NiNe::processSubmitJson(String json) {

	DynamicJsonDocument jsonObjSubmit(15*1024);
	DeserializationError jerr = deserializeJson(jsonObjSubmit, json);

	log_e("processSubmitJson jerr :: %s", jerr.c_str());

	JsonArray rows = jsonObjSubmit["rows"];

	for (JsonObject aRow : rows) {
		const char* e1 = aRow["element"][0]["e1"][0];
		const char* e2 = aRow["element"][1]["e2"][0];

		if (spiffsKe(e1, e2)) {
			_nData->_nakSavSpiffs = true;
		}
		if (mandoKe(e1, e2)) {
			_nData->_nakConfigMando = true;
		}

//		// COntroller
//		if (!strcmp(e1, "Switching mode")) {
//			if (!strcmp(e2, "Pri-sec mode")) {
//				ini->LampConfig.switchMode = 1;
//			}else if (!strcmp(e2, "A-B mode")) {
//				ini->LampConfig.switchMode = 2;
//			}
//		}
//		if (!strcmp(e1, "Reporting interval (s)")) {
//			ini->LampConfig.reportInterval = strtol(e2, NULL, 10);
//		}
//		if (!strcmp(e1, "Short-circuit thres (A)")) {
//			ini->LampConfig.shortThres = strtod(e2, NULL);
//		}
//
//		// Lantern 1
//		if (!strcmp(e1, "1 Detect current (A)")) {
//			ini->LampConfig.currentThresL1 = strtod(e2, NULL);
//			log_i("currentThres == %f", ini->LampConfig.currentThresL1);
//		}
//
//		if (!strcmp(e1, "2 False-Guard (s)")) {
//			ini->LampConfig.falseGuardTimeL1 = strtol(e2, NULL, 10);
//		}
//		if (!strcmp(e1, "3 Flash-Gap (s)")) {
//			ini->LampConfig.flashGapL1 = strtol(e2, NULL, 10);
//		}
//
//		// Lantern 2
//		if (!strcmp(e1, "4 Detect current (A)")) {
//			ini->LampConfig.currentThresL2 = strtod(e2, NULL);
//			log_i("currentThres == %f", ini->LampConfig.currentThresL2);
//		}
//		if (!strcmp(e1, "5 False-Guard (s)")) {
//			ini->LampConfig.falseGuardTimeL2 = strtol(e2, NULL, 10);
//		}
//		if (!strcmp(e1, "6 Flash-Gap (s)")) {
//			ini->LampConfig.flashGapL2 = strtol(e2, NULL, 10);
//		}






//		ini->LampConfig.adaBaru = 1;
	}

	if (_nData->_nakSavSpiffs) {
		_nData->saveSpiffsData();
	}

}

bool NiNe::spiffsKe(String e1, String e2) {
	bool ada = false;
	for (int i = 0; i < SAIZ_BRG_SPIFFS; i++) {
		if (e1 == brgSpiffs[i]) {
			ada = true;
			break;
		}
	}

	// {"DAC", "FI", "Dest. ID", "RACON Mon.", "Lantern Mon."};

	if (ada) {
		if (e1.equals("DAC")) {
			_nData->confData.DAC = e2;
		}
		else if (e1.equals("FI")) {
			_nData->confData.FI = e2;
		}
		else if (e1.equals("Dest. ID")) {
			_nData->confData.Dest_ID = e2;
		}
		else if (e1.equals("RACON Mon.")) {
			_nData->confData.RACON_Mon = e2;
		}
		else if (e1.equals("LANTERN Mon.")) {
			_nData->confData.LANT_Mon = e2;
		}
		else if (e1.equals("Detect Current (A)")) {
			_nData->confData.currentThres = e2.toDouble();
		}
		else if (e1.equals("Flash-Gap (s)")) {
			_nData->confData.flashGap = e2.toInt();
		}
	}
	return ada;
}

bool NiNe::mandoKe(String e1, String e2) {
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
		log_i("nak config %s", e1.c_str());

		switch (noBrg) {
		case 0:
			_nData->mandoConf.MName = e2;
			break;
		case 1:
			_nData->mandoConf.MMmsi = e2;
			break;
		case 2:
			_nData->mandoConf.MAtonType = e2.substring(0, 2);
			break;
		case 3:
			_nData->mandoConf.MLengthA = e2;
			break;
		case 4:
			_nData->mandoConf.MLengthB = e2;
			break;
		case 5:
			_nData->mandoConf.MLengthC = e2;
			break;
		case 6:
			_nData->mandoConf.MLengthD = e2;
			break;
		case 7:
			_nData->mandoConf.MOffThres = e2;
			break;
		case 8:
			_nData->mandoConf.MEpfs = e2.substring(0,1);
			break;
		case 9:
			_nData->mandoConf.MPosAcc = e2.substring(0,1);
			break;
		case 10:
			_nData->mandoConf.MConfigLat = e2;  // MTempStrRegLat ni dah digunakan untuk string NMEA untuk AIS. bukan untuk configuration. kene buat string baru utk tujuan configure je.
			break;
		case 11:
			_nData->mandoConf.MRegCardinalLat = e2;
			break;
		case 12:
			_nData->mandoConf.MConfigLng = e2;
			break;
		case 13:
			_nData->mandoConf.MRegCardinalLng = e2;
			break;
		case 14:
			_nData->mandoConf.M06UTChourA = e2;
			break;
		case 15:
			_nData->mandoConf.M06UTCminA = e2;
			break;
		case 16:
			_nData->mandoConf.M06IntervalA = e2.toInt() * 60;
			break;
		case 17:
			_nData->mandoConf.M06UTChourB = e2;
			break;
		case 18:
			_nData->mandoConf.M06UTCminB = e2;
			break;
		case 19:
			_nData->mandoConf.M06IntervalB = e2.toInt() * 60;
			break;
		case 20:
			_nData->mandoConf.M21UTChourA = e2;
			break;
		case 21:
			_nData->mandoConf.M21UTCminA = e2;
			break;
		case 22:
			_nData->mandoConf.M21IntervalA = e2.toInt() * 60;
			break;
		case 23:
			_nData->mandoConf.M21UTChourB = e2;
			break;
		case 24:
			_nData->mandoConf.M21UTCminB = e2;
			break;
		case 25:
			_nData->mandoConf.M21IntervalB = e2.toInt() * 60;
			break;
		}
	}
	return ada;
}
