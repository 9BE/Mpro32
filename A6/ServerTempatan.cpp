/*
 * ServerTempatan.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#include "ServerTempatan.h"

TaskHandle_t taskServer;
ServerTempatan * iniServer;


ServerTempatan::ServerTempatan(const BaseType_t xCoreID, uint64_t loopDelay, Mando * mando, Lantern * lantern, LReader * lanternReader) {
	// TODO Auto-generated constructor stub
	iniServer = this;
	iniServer->_loopDelay = loopDelay;
	iniServer->_oMando = mando;
	iniServer->_oLantern = lantern;
	iniServer->_oLanReader = lanternReader;
	setup();
	xTaskCreatePinnedToCore(loop, "taskServer", 8192, NULL, 1, &taskServer, xCoreID);
}

ServerTempatan::ServerTempatan(const BaseType_t xCoreID, uint64_t loopDelay,
		Mando* mando, Lantern* lantern, LReader* lanternReader, timing* tim) {

	iniServer = this;
	iniServer->_loopDelay = loopDelay;
	iniServer->_oMando = mando;
	iniServer->_oLantern = lantern;
	iniServer->_oLanReader = lanternReader;
	iniServer->_oTiming = tim;
	setup();
	xTaskCreatePinnedToCore(loop, "taskServer", 30*1024, NULL, 1, &taskServer, xCoreID);

}

ServerTempatan::~ServerTempatan() {
	// TODO Auto-generated destructor stub
}

void ServerTempatan::loop(void* lantern) {
//	esp_task_wdt_add(taskServer);
//	esp_task_wdt_reset();
	while(1) {
		_server.handleClient();
//		Serial.println("stack high");
//		Serial.println(uxTaskGetStackHighWaterMark(NULL));
//		esp_task_wdt_reset();
		delay(iniServer->_loopDelay);
	}
}

void ServerTempatan::setup() {
//	WiFi.mode(WIFI_AP);
//	WiFi.softAP("GreenFinderIOT", "0xadezcsw1");
//	WiFi.softAP("TestZippy", "123qweasd");
//	ArduinoOTA.begin();
	_server.on("/nine", HTTP_POST, WebServer::THandlerFunction(nine));
	_server.on("/Buoy", HTTP_GET, WebServer::THandlerFunction(iniServer->_oLanReader->Buoy));
	_server.on("/Lighthouse", HTTP_GET, WebServer::THandlerFunction(iniServer->_oLanReader->Lighthouse));
	_server.on("/Beacon", HTTP_GET,WebServer::THandlerFunction(iniServer->_oLanReader->Beacon));

	_server.on("/reboot", HTTP_GET, WebServer::THandlerFunction([]() {
		_server.sendHeader("Connection", "close");
		_server.send(200, "text/html", "done");
		delay(1000);
		ESP.restart();
		}));

	_server.on("/stat", WebServer::THandlerFunction(iniServer->StatusViaWiFi));

	_server.on("/ota", HTTP_GET, WebServer::THandlerFunction([]() {
		_server.sendHeader("Connection", "close");
		_server.send(200, "text/html", ota);}));

	_server.on("/update", HTTP_POST, WebServer::THandlerFunction([]() {
		_server.sendHeader("Connection", "close");
		_server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		delay(1000);
		ESP.restart();}),
			WebServer::THandlerFunction([]() {
			HTTPUpload& upload = _server.upload();
			if (upload.status == UPLOAD_FILE_START) {
				Serial.printf("Update: %s\n", upload.filename.c_str());
				if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_WRITE) {
				/* flashing firmware to ESP*/
				if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_END) {
				if (Update.end(true)) { //true to set the size to the current progress
					Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
				}
				else {
					Update.printError(Serial);
				}
			}
		}));
	_server.begin(80);
	MDNS.begin("nine_mpro");


}

void ServerTempatan::StatusViaWiFi() {
	String msg="";

	long rssi = WiFi.RSSI();
	board Board;

	msg  = "MAC = ";
	msg += Board.getMAC();
	msg += "</br>";
	msg += "IP = ";
	msg += WiFi.localIP().toString();
	msg += "</br>";
	msg += "Live = ";
	msg += (millis()/1000);
	msg += "</br>";
	msg += "Time = ";
	msg += iniServer->_oTiming->now.sMasa;
	msg += " (";
	msg += iniServer->_oTiming->utc;
	msg += ")</br>";
//
//	msg += "Pos = ";
//	msg += String(iniServer->_oMando->lat,7);
//	msg += ",";
//	msg += String(iniServer->_oMando->lng,7);
//	msg += "</br>";
//
	msg += "Battery = ";
	msg += iniServer->_oMando->M6data.MVin;
	msg += " (";
	msg += iniServer->_oMando->rawBattery;
	msg += ")</br>";
//
	msg += "RSSI = ";
	msg += String(rssi);
	msg += "</br>";
	msg += "WiFi tx power = ";
	msg += (int) WiFi.getTxPower();
	msg += "</br>";
//	msg += "Actual = " + String(Ais.actual) + "</br>";
////	msg += "Temp = " + String((temprature_sens_read() - 32) / 1.8) + "</br>";
	msg += "Activity = ";
	msg += iniServer->alert;
	msg += "</br>";

	msg += "============================================== DEBUG =====================================================</br>";
	String wkl = "";
	if (iniServer->_oLantern->lanternRespond == 0)          wkl = "Not found";
	else if (iniServer->_oLantern->lanternRespond == 1)     wkl = "Searching Lantern";
	else if (iniServer->_oLantern->lanternRespond == 2)     wkl = "Found SC35";
	else if (iniServer->_oLantern->lanternRespond == 3)     wkl = iniServer->_oLantern->LanternLongStatus;
	else if (iniServer->_oLantern->lanternRespond == 4)     wkl = iniServer->_oLantern->LanternLongStatus;
	else if (iniServer->_oLantern->lanternRespond == 5)     wkl = iniServer->_oLantern->LanternLongStatus;

	msg += ">>> LANTERN >>> </br>";
	msg += "Lantern respong = ";
	msg += (int) iniServer->_oLantern->lanternRespond;
	msg += "</br>";
	msg += "Lantern Status = ";
	msg += wkl;
	msg += "</br>";
	msg += "Normalize = ";
	msg += iniServer->_oLantern->_nyalaNormal;
	msg += "</br>";
	msg += "Lantern nyala = ";
	msg += iniServer->_oMando->M6data.LNyala;
	msg += "</br>";
	msg += "Lantern Vin = ";
//	msg += iniServer->_oMando->M6data.LVin;
	msg += "</br>";

//	msg += "AIS port = " ;
//	msg += iniServer->AppCommPort;
//	msg += "</br>";
	msg += ">>> MANDO >>> </br>";
	msg += "Aton Bit = ";
	msg += iniServer->_oMando->ProcAtonbit_prev;
	msg += "</br>";
	msg += "Kali hantar msg 6 = ";
	msg += iniServer->_oMando->_kaliHantarM6;
	msg += "</br>";
	msg += "ABM = ";
	msg += iniServer->_oMando->_abm;
	msg += "</br>";
	msg += "Last msg21 = ";
	msg += (millis() - iniServer->_oMando->agoM21) / 1000;
	msg += "</br>";
	msg += "Last msg06 = ";
	msg += (millis() - iniServer->_oMando->agoM6) / 1000;
	msg += "</br>";
	msg += "Off position = ";
	msg += iniServer->_oMando->M6data.ProcOffPositionStatus;
	msg += "</br>";


	msg += "============================================== NMEA =====================================================</br>";

	msg += iniServer->_oMando->_nmea;
	iniServer->_oMando->_nmea = "";
	_server.sendHeader("Connection", "close");
	_server.send(200, "text/html", msg);
}

bool ServerTempatan::nine() {

	//	  jsonBuffer.clear();

	if (_server.args() == 0) {
		_server.send( 200, "application/json", iniServer->makeInit() );
	}
	else {
		if (_server.argName(0) == "full") {
			String msg = "";
			_server.send( 200, "application/json", iniServer->makeJson() );
		}
		else if (_server.argName(0) == "submit") {
			String msg = "";
			JsonHandler *jsonHandler;
			jsonHandler = new JsonHandler(iniServer->_oMando);

			jsonHandler->processSubmitJSON(_server.arg("plain"));

			_server.send( 200, "application/json", iniServer->makeJson() );
		}
	}

	return true;
}


String ServerTempatan::makeInit() {

	DynamicJsonDocument jsonBasic(1024);

	jsonBasic["i"] = "1";
	jsonBasic["n"] = _oMando->_machine.name.c_str(); //asalnya Mando Pro
	jsonBasic["j"] = "d";
	jsonBasic["c"] = "I4N2IP.png";
	jsonBasic["s"] = "";
	jsonBasic["m"] = _oMando->_machine.id.c_str();
	jsonBasic["e"] = "10800000";

	JsonArray g = jsonBasic.createNestedArray("g");
	g.add("GF7TDK");
	g.add("G3XJRL");

	String res;
	serializeJson(jsonBasic, res);
	jsonBasic.clear();
	return res;
}



String ServerTempatan::makeJson() {

	JsonHandler *jsonHandler;
	jsonHandler = new JsonHandler(_oMando);

	DynamicJsonDocument jsonBasic(15*1024);

	jsonBasic["m"] = _oMando->_machine.id.c_str();
	jsonBasic["n"] = _oMando->_machine.name.c_str();
	jsonBasic["j"] = "d";

	JsonArray g = jsonBasic.createNestedArray("g");
	g.add("GF7TDK");
	g.add("G3XJRL");

	JsonObject r = jsonBasic.createNestedObject("r");
	r["i"] = "Update List";
	r["f"] = "##0.00";      //number format
	r["c"] = "RM";        //currency
	r["u"] = "0";       //currency

	JsonObject tab = jsonBasic.createNestedObject("tabs");
	tab["1"] = "Monitor";
	tab["2"] = "Config";

	String wkl;
	JsonArray rows = jsonBasic.createNestedArray("rows");

	jsonHandler->tambahRow(rows, "1", "30", "Refresh", "");

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CONTROLLER >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	jsonHandler->tambahRow(rows, "1", "0", "Controller", "");
	jsonHandler->tambahRow(rows, "1", "2", "Prog Name", binFile);

	String jkl = "";

	if (_oTiming->now.tahun < 1970) {
		log_i("sinis");
		if (_oMando->SpiffsData.Beat == "2")       {
			wkl = "Not yet";
		}else {
			wkl = _oMando->SpiffsData.Beat;
		}

		jkl = String(millis() / 1000);
		jsonHandler->tambahRow(rows, "1", "2", "Runtime/TimeUp\nBeat", String(millis() / 1000) + "/" + String((3600000-millis()) / 1000) + "\n" + wkl);
	}
	else {
		jkl = "";
		jkl = _oTiming->now.sMasa;
		jkl += (" | ");
		jkl += (_oTiming->now.hari);
		jkl += ("/");
		jkl += (_oTiming->now.bulan);
		jkl += ("/");
		jkl += (_oTiming->now.tahun);
		jkl += (" | ");
		String mmnntah;
		// patadaa
		if (!_oLantern->askLantern3) {
			if (_oTiming->ZoneTime == e_nite)       mmnntah = "Night";
			else if (_oTiming->ZoneTime == e_dawn)  mmnntah = "Dawn";
			else if (_oTiming->ZoneTime == e_day)  mmnntah = "Daylight";
			else if (_oTiming->ZoneTime == e_dusk)  mmnntah = "Dusk";
			jkl += mmnntah;
		}


		if (!strcmp(_oMando->SpiffsData.Beat.c_str(), "2")) {
			wkl = "Not yet";
		}
		else {
			wkl = _oMando->SpiffsData.Beat;
		}

		jsonHandler->tambahRow(rows, "1", "2", "Runtime/TimeUp\nBeat\nClock", String(millis() / 1000) + "/" + String((3600000-millis()) / 1000) + "\n" + wkl + "\n" + jkl);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RECEPTION >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	jsonHandler->tambahRow(rows, "1", "0", "Reception", "");


	if (_oMando->_inputMandoRmc) {
		wkl = "Found : ";
		wkl += String(_oMando->lat, 4);
		wkl += ";";
		wkl += String(_oMando->lng, 4);
		wkl += "\nConfig : ";
		wkl += String(_oMando->tmpLat, 4);
		wkl += ";";
		wkl += String(_oMando->tmpLng, 4);
		if (_oMando->M6data.ProcOffPositionStatus) {
			wkl += "\nOff position";
		}else {
			wkl += "\nOn position";
		}
	}
	else {
		wkl = "Not Found";
	}


	jsonHandler->tambahRow(rows, "1", "2", "Comm AIS\nGPS", AppCommPort + "\n" + wkl);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AIS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	jsonHandler->tambahRow(rows, "1", "0", "AIS", "");

	jsonHandler->tambahRow(rows, "1", "2", "Activity", alert);

	String kkkkk, nnnnn;
	if (_oMando->_inputMandoVdo21) {
		kkkkk = String((millis() - _oMando->agoM21) / 1000);
	}else {
		kkkkk = "Not yet";
	}
	if (_oMando->_inputMandoVdo06) {
		nnnnn = String((millis() - _oMando->agoM6) / 1000);
	}else {
		nnnnn = "Not yet";
	}
//
//	String next6 = ((_oMando->_masaMando + 30000) - millis())/1000;
//	if (((_oMando->_masaMando + 30000) - millis())/1000 =)


	jsonHandler->tambahRow(rows, "1", "2", "V AIS\nS/N\nAton Bit\nM21/06 ago\nNext M06", String(_oMando->M6data.MVin, 2) + "\n" + _oMando->MSerialNumber + "\n" + _oMando->ProcAtonbit_prev + "\n" + kkkkk + "/" + nnnnn);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< LANTERN >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	jsonHandler->tambahRow(rows, "1", "0", "Lantern", "");
	if (_oLantern->lanternRespond == 0)          wkl = _oLantern->getAlert();
	else if (_oLantern->lanternRespond == 1)     wkl = "Searching Lantern";
	if (_oLantern->lanternRespond == 2)     wkl = "Found SC35";
	else if (_oLantern->lanternRespond == 3)     wkl = _oLantern->LanternLongStatus;
	else if (_oLantern->lanternRespond == 4)     wkl = _oLantern->LanternLongStatus;
	else if (_oLantern->lanternRespond == 5)     wkl = _oLantern->LanternLongStatus;

	log_i("LanternLongStatus ====================== %s", _oLantern->LanternLongStatus.c_str());

	jsonHandler->tambahRow(rows, "1", "2", "Status", wkl);

	if (_oLantern->lanternRespond == 2) {  //SC35
		String hmmm, yawww, bekkk;
		if (_oMando->M6data.LDRStatus == 3)         wkl = "Bright";
		else if (_oMando->M6data.LDRStatus == 2)    wkl = "Dim";
		else if (_oMando->M6data.LDRStatus == 1)    wkl = "Dark";
		else                        wkl = "Not detected";

		if (_oMando->M6data.LNyala == 0)                  hmmm = "Off";
		else if (_oMando->M6data.LNyala == 1)             hmmm = "On";

		if (_oMando->M6data.LSolarChargingOn == 0)        yawww = "No";
		else if (_oMando->M6data.LSolarChargingOn == 1)   yawww = "Yes";
		if (_oMando->M6data.LIsNight == 0)                bekkk = "No";
		else if (_oMando->M6data.LIsNight == 1)           bekkk = "Yes";

		jsonHandler->tambahRow(rows, "1", "2", "V Lantern\nLDR/Flash\nLED temp. (°C)\nSolarCharging/Night", String(_oMando->M6data.LVin, 2) + "\n" + wkl + "/" + hmmm + "\n" + String(_oMando->M6data.LLedTemp) + "\n" + yawww + "/" + bekkk);

		if (!_oMando->M6data.LNyala) {
			jsonHandler->tambahRow(rows, "1", "0", "Lantern Off Due To", "");
			if (_oMando->M6data.LOffLedPowThres == 1)   jsonHandler->tambahRow(rows, "1", "2", "LedPowThres", "Power too low");
			if (_oMando->M6data.LOffLowVin == 1)        jsonHandler->tambahRow(rows, "1", "2", "LowVin", "Vin too low");
			if (_oMando->M6data.LOffLDR == 1)           jsonHandler->tambahRow(rows, "1", "2", "LDR", "Too bright");
			if (_oMando->M6data.LOffTemp == 1)          jsonHandler->tambahRow(rows, "1", "2", "Temperature", "Too hot or too cold");
			if (_oMando->M6data.LOffForce == 1)         jsonHandler->tambahRow(rows, "1", "2", "Forced", "Programmed to off");
		}
	}
	else if (_oLantern->lanternRespond == 3) { //beacon
		if (_oLantern->LanternStat == "Processing" || _oLantern->LanternStat == "Locked") {
			String wkl, jiii, mmse, doidoi;
			if (_oMando->M6data.LDRStatus == 3)       mmse = "Bright";
			else if (_oMando->M6data.LDRStatus == 2)  mmse = "Dim";
			else if (_oMando->M6data.LDRStatus == 1)  mmse = "Dark";
			else if (_oMando->M6data.LDRStatus == 0)  mmse = "Not install";

			if (_oMando->M6data.Door)  doidoi = "Open";
			else      doidoi = "Close";
		if (strcmp(_oMando->SpiffsData.Sec_Mon.c_str(), "Yes")) {
				if (_oMando->M6data.SLNyala == 0)                   wkl = "Off";
				else if (_oMando->M6data.SLNyala == 1)              wkl = "On";
				if (_oMando->M6data.LNyala == 0)                    jiii = "Off";
				else if (_oMando->M6data.LNyala == 1)               jiii = "On";
				jsonHandler->tambahRow(rows, "1", "2", "V Lantern\nPrim. Flash/Diff/Thres\nSec. Flash/Diff/Thres\nLDR/Door", String(_oMando->M6data.LVin, 2) + "\n" + jiii + "/" + String(_oLantern->PrimMaxMinDiff, 2) + "/" + String(_oLantern->Primthresholdamp, 2) + "\n" + wkl + "/" + String(_oLantern->SecMaxMinDiff, 2) + "/" + String(_oLantern->Secthresholdamp, 2) + "\n" + mmse + "/" + doidoi);
			}
			else {
				if (_oMando->M6data.LNyala == 0)                  wkl = "Off";
				else if (_oMando->M6data.LNyala == 1)             wkl = "On";
				jsonHandler->tambahRow(rows, "1", "2", "V Lantern\nPrim. Flash/Diff/Thres\nLDR/Door", String(_oMando->M6data.LVin, 2) + "\n" + wkl + "/" + String(_oLantern->PrimMaxMinDiff, 2) + "/" + String(_oLantern->Primthresholdamp, 2) + "\n" + mmse + "/" + doidoi);
			}

			String rraacoon, strehh;
			if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
				_oMando->AtonBit.racon = 0;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "10Vin")) {
				if (_oMando->M6data.RVin >= 10)       _oMando->AtonBit.racon = 2;
				else                  _oMando->AtonBit.racon = 3;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "Contact ON")) {
				if (_oLantern->FreeContact == 1) _oMando->AtonBit.racon = 2;
				else                  _oMando->AtonBit.racon = 3;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "Contact Fail")) {
				if (_oLantern->FreeContact == 0) _oMando->AtonBit.racon = 2;
				else                  _oMando->AtonBit.racon = 3;
			}
			if (_oMando->AtonBit.racon == 0)      rraacoon = "Not installed";
			else if (_oMando->AtonBit.racon == 1) rraacoon = "Not monitored";
			else if (_oMando->AtonBit.racon == 2) rraacoon = "Operational";
			else if (_oMando->AtonBit.racon == 3) rraacoon = "Error";

			if (_oMando->AtonBit.racon != 0) {
				jsonHandler->tambahRow(rows, "1", "2", "RACON V/Stat", String(_oMando->M6data.RVin, 2) + "/" + rraacoon);
			}


		}

	}
	else if (_oLantern->lanternRespond == 4) { //buoy
		if (_oLantern->LanternStat == "Processing" || _oLantern->LanternStat == "Locked") {
			String wkl;
			if (_oMando->M6data.LNyala == 0)                  wkl = "Off";
			else if (_oMando->M6data.LNyala == 1)             wkl = "On";
			jsonHandler->tambahRow(rows, "1", "2", "V Lantern\nPrim Diff./Thres.\nPrim. Flash", String(_oMando->M6data.LVin, 2) + "\n" + String(_oLantern->PrimMaxMinDiff, 4) + "/" + String(_oLantern->ThresApp, 4) + "\n" + wkl);
//			jsonHandler->.tambahRow(rows, "1", "2", "Weight/CounterD\nDoNotChange\nLigstat", String(Weight) + "/" + String(CounterD) + "\n" + String(DoNotChange) + "/" + lightStatus);
		}
	}
	else if (!strcmp(_oMando->SpiffsData.Format.c_str(), "GF-LR-LIGHTHOUSE")) { //lighthouse
		if (_oLantern->lanternlockC) {
			String rraacoon, strehh;
			if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
				_oMando->AtonBit.racon = 0;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "10Vin")) {
				if (_oMando->M6data.RVin >= 10)       _oMando->AtonBit.racon = 2;
				else                 _oMando->AtonBit. racon = 3;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "Contact ON")) {
				if (_oLantern->FreeContact == 1) _oMando->AtonBit.racon = 2;
				else                  _oMando->AtonBit.racon = 3;
			}
			else if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "Contact Fail")) {
				if (_oLantern->FreeContact == 0) _oMando->AtonBit.racon = 2;
				else                  _oMando->AtonBit.racon = 3;
			}
			if (_oMando->AtonBit.racon == 0)      rraacoon = "Not installed";
			else if (_oMando->AtonBit.racon == 1) rraacoon = "Not monitored";
			else if (_oMando->AtonBit.racon == 2) rraacoon = "Operational";
			else if (_oMando->AtonBit.racon == 3) rraacoon = "Error";
			if (_oMando->M6data.LDRStatus == 3)         strehh = "Bright";
			else if (_oMando->M6data.LDRStatus == 2)    strehh = "Dim";
			else if (_oMando->M6data.LDRStatus == 1)    strehh = "Dark";
			else                        strehh = "Not detected";
			if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
				jsonHandler->tambahRow(rows, "1", "2", "V Main\nV Emerg\nLDR", String(_oMando->M6data.LVin, 2) + "\n" + String(_oMando->M6data.RVin, 2) + "\n" + strehh);
			}
			else {
				jsonHandler->tambahRow(rows, "1", "2", "V Lantern\nV RACON/Stat\nLDR", String(_oMando->M6data.LVin, 2) + "\n" + String(_oMando->M6data.RVin, 2) + "/" + rraacoon + "\n" + strehh);
			}

			if (_oLantern->jumpaLR)
				jsonHandler->tambahRow(rows, "1", "2", "EL Diff/Thres\nEL Activity", String(_oLantern->PrimMaxMinDiff, 2) + "/" + String(_oLantern->Primthresholdamp, 2) + "\n" + _oLantern->emergencyStat);


			//racon status: 0 = no racon; 1 = not monitored; 2 = operational; 3 = error
			String taek, kucing, sangat, busuk, korek, idung, jari, kelingking, takdeidea, dahni;
			String kambing, wangi, monyet, hodoh;
			if (_oMando->M6data.MLCond)  taek = "Fail";
			else        taek = "Normal";
			if (_oMando->M6data.MLStat)  kucing = "On";
			else        kucing = "Off";
			if (_oMando->M6data.SLCond)  sangat = "Fail";
			else        sangat = "Normal";
			if (_oMando->M6data.SLStat)  busuk = "On";
			else        busuk = "Off";
			if (_oMando->M6data.D1Stat)  jari = "On";
			else        jari = "Off";
			if (_oMando->M6data.D1Cond)  kelingking = "Fail";
			else        kelingking = "Normal";
			if (_oMando->M6data.D2Stat)  takdeidea = "On";
			else        takdeidea = "Off";
			if (_oMando->M6data.D2Cond)  dahni = "Fail";
			else        dahni = "Normal";
			if (_oMando->M6data.Door)    kambing = "Open";
			else        kambing = "Close";
			if (_oMando->M6data.ACPower) wangi = "On";
			else        wangi = "Off";
			if (_oMando->M6data.BMS)     monyet = "Fail";
			else        monyet = "Normal";
			if (_oLantern->PSU)     hodoh = "On";
			else        hodoh = "Off";


			if (!_oLantern->lanternlockB) {
				korek = "Not yet";
				idung = "Not yet";
			}
			if (_oLantern->findEmergency) {  //nak kene verify dulu value ni.
				korek = "Not found";
				idung = "Not found";
			}
			jsonHandler->tambahRow(rows, "1", "2", "ML Con/Stat\nSL Con/Stat\nEL Con/Stat\nD1 Stat/Con\nD2 Stat/Con\nDoor/AC\nBMS/PSU", taek + "/" + kucing + "\n" + sangat + "/" + busuk + "\n" + korek + "/" + idung + "\n" + jari + "/" + kelingking + "\n" + takdeidea + "/" + dahni + "\n" + kambing + "/" + wangi + "\n" + monyet + "/" + hodoh);
		}
		else if (_oLantern->jumpaLR) {
			String rt, fb;
			if (!_oLantern->lanternlockB) {
				rt = "Processing";
				fb = "Processing";
			}
			else {
				if (_oMando->M6data.ELCond)  rt = "Fail";
				else        rt = "Normal";
				if (_oMando->M6data.ELStat)  fb = "On";
				else        fb = "Off";
			}

			if (!strcmp(_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
				if (_oLantern->lanternlockB) {
					_oLantern->emergencyStat = "Locked " + rt + " " + fb;
				}
				jsonHandler->tambahRow(rows, "1", "2", "EL Batt\nDiff/Thres\nEL Activity", String(_oMando->M6data.RVin, 2) + "\n" + String(_oLantern->PrimMaxMinDiff, 2) + "/" + String(_oLantern->Primthresholdamp, 2) + "\n" + _oLantern->emergencyStat);

			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CONFIG >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	jsonHandler->tambahRow(rows, "2", "1", "Do not config more than 10 item at one time.\nSend after Query AIS.", "");
	jsonHandler->tambahRow(rows, "2", "0", "Controller", "");
	jsonHandler->tambahRow(rows, "2", "7", "Format", jsonHandler->cariDanTukar(JSONFormat, ',', _oMando->SpiffsData.Format)); // "GF,A126" //getFormat(data.getDataFromJsonFile("config", "Format"))
	jsonHandler->tambahRow(rows, "2", "4", "DAC", _oMando->SpiffsData.DAC);
	jsonHandler->tambahRow(rows, "2", "4", "FI", _oMando->SpiffsData.FI);
	jsonHandler->tambahRow(rows, "2", "4", "Dest. ID", _oMando->SpiffsData.Dest_ID);
	jsonHandler->tambahRow(rows, "2", "6", "UTC Offset", _oMando->SpiffsData.UTC_Offset);
	if (strcmp(_oMando->SpiffsData.Format.c_str(), "GF-SC35")) {
		jsonHandler->tambahRow(rows, "2", "0", "LANTERN - GF-LR-*", "");
		jsonHandler->tambahRow(rows, "2", "7", "RACON Mon.", jsonHandler->cariDanTukar(JSONMonitorRACON, ',', _oMando->SpiffsData.RACON_Mon));
		jsonHandler->tambahRow(rows, "2", "7", "Use LDR", jsonHandler->cariDanTukar(JSONUseLDR, ',', _oMando->SpiffsData.Use_LDR));
		jsonHandler->tambahRow(rows, "2", "7", "Light Detect Method", jsonHandler->cariDanTukar(JSONLightMode, ',', _oMando->SpiffsData.Light_Detect_Method));
		jsonHandler->tambahRow(rows, "2", "7", "Calibrate", jsonHandler->cariDanTukar(JSONCalibrate, ',', _oMando->SpiffsData.Calibrate));
		jsonHandler->tambahRow(rows, "2", "5", "Calib. Prim. (M)", _oMando->SpiffsData.Calib_Prim_M);
	}
	if (!strcmp(_oMando->SpiffsData.Format.c_str() , "GF-LR-BEACON")) {
		jsonHandler->tambahRow(rows, "2", "0", "LANTERN - GF-LR-BEACON", "");
		jsonHandler->tambahRow(rows, "2", "7", "Sec. Mon.", jsonHandler->cariDanTukar(JSONMonitorSecondary, ',', _oMando->SpiffsData.Sec_Mon));
		jsonHandler->tambahRow(rows, "2", "5", "Calib. Sec. (M)", _oMando->SpiffsData.Calib_Sec_M);
		jsonHandler->tambahRow(rows, "2", "7", "Reboot Mode", jsonHandler->cariDanTukar(JSONRebootMode, ',', _oMando->SpiffsData.Reboot_Mode));
	}



	if (_oMando->getMandoTaskStat() > 30) {
		jsonHandler->tambahRow(rows, "2", "0", "AIS - Station", "");
	}
	else {
		jsonHandler->tambahRow(rows, "2", "0", "AIS - Station (Loading...)", "");
	}
	jsonHandler->tambahRow(rows, "2", "3", "Name", _oMando->_mConfig.MName);
	jsonHandler->tambahRow(rows, "2", "4", "MMSI", _oMando->_mConfig.MMmsi);      //how? MMmsi ni dia char. takpe ke? auto string, eh? kalau betul, kiranya kita hnya ada masalah utk serialprint char je la.
	jsonHandler->tambahRow(rows, "2", "7", "Type", jsonHandler->cariDanTukar(JSONTypeOfAton, ',', _oMando->_mConfig.MAtonType));
	jsonHandler->tambahRow(rows, "2", "4", "Len A", String(_oMando->_mConfig.MLengthA.toInt()));
	jsonHandler->tambahRow(rows, "2", "4", "Len B", String(_oMando->_mConfig.MLengthB.toInt()));
	jsonHandler->tambahRow(rows, "2", "4", "Len C", String(_oMando->_mConfig.MLengthC.toInt()));
	jsonHandler->tambahRow(rows, "2", "4", "Len D", String(_oMando->_mConfig.MLengthD.toInt()));
	jsonHandler->tambahRow(rows, "2", "4", "OffPos. Thres.", String(_oMando->_mConfig.MOffThres));
	jsonHandler->tambahRow(rows, "2", "7", "EPFS", jsonHandler->cariDanTukar(JSONEPFS, ',', _oMando->_mConfig.MEpfs));


	String posacc;
	if ((String)_oMando->_mConfig.MPosAcc == "1") {
		posacc = "1,0";
	}
	else if ((String)_oMando->_mConfig.MPosAcc == "0") {
		posacc = "0,1";
	}
	jsonHandler->tambahRow(rows, "2", "7", "Pos.acc.", posacc);
	jsonHandler->tambahRow(rows, "2", "5", "Lat", String(_oMando->tmpLat, 4));
	jsonHandler->tambahRow(rows, "2", "7", "Card. Lat", jsonHandler->cariDanTukar(JSONCardinalLat, ',', _oMando->_mConfig.MRegCardinalLat)); // "N,S"     // SUHAIMI =--=-=-=- Contoh utk Type -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=- SUHAIMI
	jsonHandler->tambahRow(rows, "2", "5", "Lng", String(_oMando->tmpLng, 4));
	jsonHandler->tambahRow(rows, "2", "7", "Card. Lng.", jsonHandler->cariDanTukar(JSONCardinalLng, ',', _oMando->_mConfig.MRegCardinalLng)); // "E,W"

	//mando end
	if (_oMando->getMandoTaskStat() > 30) {
		jsonHandler->tambahRow(rows, "2", "0", "AIS - Msg Scheduling", "");
	}
	else {
		jsonHandler->tambahRow(rows, "2", "0", "AIS - Msg Scheduling (Loading...)", "");
	}
	jsonHandler->tambahRow(rows, "2", "4", "M6 A UTC h", _oMando->_mConfig.M06UTChourA);
	jsonHandler->tambahRow(rows, "2", "4", "M6 A UTC m", _oMando->_mConfig.M06UTCminA);
	jsonHandler->tambahRow(rows, "2", "4", "M6 A Int.", _oMando->_mConfig.M06IntervalABahagi60);
	jsonHandler->tambahRow(rows, "2", "4", "M6 B UTC h", _oMando->_mConfig.M06UTChourB);
	jsonHandler->tambahRow(rows, "2", "4", "M6 B UTC m", _oMando->_mConfig.M06UTCminB);
	jsonHandler->tambahRow(rows, "2", "4", "M6 B Int.", _oMando->_mConfig.M06IntervalBBahagi60);
	jsonHandler->tambahRow(rows, "2", "4", "M21 A UTC h", _oMando->_mConfig.M21UTChourA);
	jsonHandler->tambahRow(rows, "2", "4", "M21 A UTC m", _oMando->_mConfig.M21UTCminA);
	jsonHandler->tambahRow(rows, "2", "4", "M21 A Int.", _oMando->_mConfig.M21IntervalABahagi60);
	jsonHandler->tambahRow(rows, "2", "4", "M21 B UTC h", _oMando->_mConfig.M21UTChourB);
	jsonHandler->tambahRow(rows, "2", "4", "M21 B UTC m", _oMando->_mConfig.M21UTCminB);
	jsonHandler->tambahRow(rows, "2", "4", "M21 B Int.", _oMando->_mConfig.M21IntervalBBahagi60);

	String res;
	serializeJson(jsonBasic, res);
	jsonBasic.clear();
	delay(10);
	delete jsonHandler;
	return res;
}


