/*
 * LReader.cpp
 *
 *  Created on: Nov 7, 2019
 *      Author: suhaimi
 */

#include "LReader.h"

LReader * iniLreader;
WebServer _server;

LReader::LReader(Lantern* lantern, Mando* mando) {
	// TODO Auto-generated constructor stub

	iniLreader = this;
	iniLreader->_oLantern = lantern;
	iniLreader->_oMando = mando;
}

LReader::~LReader() {
	// TODO Auto-generated destructor stub
}

void LReader::Buoy() {

	String n = "";
	if (iniLreader->_oMando->ProcAtonbit_prev != "XX") {
		// amek bit dr hex to binary 4 bit sahaja
		for (int x = 0; x < 2; x ++) {
			char k = iniLreader->_oMando->ProcAtonbit_prev[x];
			int l = int(k);
			l = l - 48;
			if (l >= 17) {
				l = l - 7;
			}
			String m = String(l, BIN);
			if (m.length() == 3)       m = "0" + m;
			else if (m.length() == 2)  m = "00" + m;
			else if (m.length() == 1)  m = "000" + m;
			n += m;
		}
	}
	else {
		n = "XX";
	}
	iniLreader->_oLantern->jumpaLR = true;
	_server.send(200, "application/json",  n + "," + iniLreader->_oMando->SpiffsData.Calibrate + "," + iniLreader->_oMando->SpiffsData.Calib_Prim_M + "," + iniLreader->_oMando->SpiffsData.Light_Detect_Method);
	n = "";

	if (!iniLreader->_oLantern->jumpaSC35) {
		if (!iniLreader->_oLantern->lanternlock) {
			for (int i = 0; i < _server.args(); i++) {
				if (_server.argName(i) == "req") {

					String ss = _server.arg(i);
					const size_t bufferSize = JSON_OBJECT_SIZE(11) + 150; //asalnya JSON_OBJECT_SIZE(10) + 140;
					DynamicJsonDocument root(bufferSize);
					deserializeJson(root,ss);
					String Status = root["Status"];
					String Runtime = root["Countdown"];
					iniLreader->_oLantern->LanternStat = Status;
					if (iniLreader->_oLantern->LanternStat == "Booting") {                     //dah
						String Thr = root["ThresholdPrim"];
						if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE") & !iniLreader->_oLantern->lanternlockB) {
							iniLreader->_oLantern->LanternLongStatus = "Booting. Countdown ";        //dah
							iniLreader->_oLantern->LanternLongStatus += Runtime;                     //dah
							iniLreader->_oLantern->LanternLongStatus += "s.";                        //dah
							iniLreader->_oLantern->LanternLongStatus += "\nThres: ";                        //dah
							iniLreader->_oLantern->LanternLongStatus += Thr;

						}
						else
							iniLreader->_oLantern->emergencyStat = "Booting";

					}
					else if (iniLreader->_oLantern->LanternStat == "Calibrating") {            //dah
						if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE") & !iniLreader->_oLantern->lanternlockB) {
							iniLreader->_oLantern->LanternLongStatus = "Calibrating. Countdown: ";   //dah
							iniLreader->_oLantern->LanternLongStatus += Runtime;                     //dah
							iniLreader->_oLantern->LanternLongStatus += "s.";                        //dah
						}
					}
					else if (iniLreader->_oLantern->LanternStat == "Processing" ) {            //dah

						String Voltage = root["Voltage"];                 //dah
						String DiffPrimary = root["DiffPrimary"];         //dah
						String Thr = root["ThresholdPrim"];               //dah
						String Light = root["Light"];                     //dah

						if (!strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE") & !iniLreader->_oLantern->lanternlockB) {
							iniLreader->_oLantern->emergencyStat = "Processing";
							iniLreader->_oLantern->PrimMaxMinDiff = DiffPrimary.toFloat();
							iniLreader->_oLantern->Primthresholdamp = Thr.toFloat();
							iniLreader->_oMando->M6data.ELStat = Light.toInt();
							if (iniLreader->_oMando->M6data.ELStat)
								iniLreader->_oMando->M6data.LNyala = 1;
						if (!strcmp(iniLreader->_oMando->SpiffsData.RACON_Mon.c_str(), "No"))
							iniLreader->_oMando->M6data.RVin = Voltage.toFloat();
						}
						else if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE")) {
							iniLreader->_oLantern->PrimMaxMinDiff = DiffPrimary.toFloat();
							iniLreader->_oLantern->Primthresholdamp = Thr.toFloat();

							iniLreader->_oLantern->LanternLongStatus = "Processing. Countdown ";     //dah
							iniLreader->_oLantern->LanternLongStatus += Runtime;                     //dah
							iniLreader->_oLantern->LanternLongStatus += "s.";                        //dah

							iniLreader->_oMando->M6data.LVin = Voltage.toFloat();
							iniLreader->_oMando->M6data.LNyala = Light.toInt();
							iniLreader->_oLantern->ThresApp = iniLreader->_oLantern->Primthresholdamp;
						}
					}
					else if (iniLreader->_oLantern->LanternStat == "Locked") {

						String Voltage = root["Voltage"];                 //dah
						String DiffPrimary = root["DiffPrimary"];         //dah
						String Thr = root["ThresholdPrim"];               //dah
						String Light = root["Light"];                     //dah

						if (!strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE") & !iniLreader->_oLantern->lanternlockB) {
							iniLreader->_oLantern->PrimMaxMinDiff = DiffPrimary.toFloat();
							iniLreader->_oLantern->Primthresholdamp = Thr.toFloat();


							if (iniLreader->_oLantern->PrimMaxMinDiff < iniLreader->_oLantern->Primthresholdamp) {
								iniLreader->_oMando->M6data.ELStat = 0;
							}
							else {
								iniLreader->_oMando->M6data.ELStat = 1;
							}
							iniLreader->_oLantern->lanternlockB = true;
							iniLreader->_oMando->M6data.ELStat = Light.toInt();
							iniLreader->_oLantern->emergencyStat = "Locked";
							if (iniLreader->_oMando->M6data.ELStat)
								iniLreader->_oMando->M6data.LNyala = 1;
							if (!strcmp(iniLreader->_oMando->SpiffsData.RACON_Mon.c_str(), "No"))
								iniLreader->_oMando->M6data.RVin = Voltage.toFloat();
							if (iniLreader->_oLantern->lanternlockB & iniLreader->_oLantern->lanternlockC) {
								iniLreader->_oLantern->LanternLongStatus = "Data locked.";
							}
						}
						else if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE")) {
							iniLreader->_oMando->M6data.LVin = Voltage.toFloat();
							iniLreader->_oLantern->PrimMaxMinDiff = DiffPrimary.toFloat();
							iniLreader->_oLantern->Primthresholdamp = Thr.toFloat();
							iniLreader->_oLantern->LanternLongStatus = "Data locked.";
							iniLreader->_oLantern->lanternlock = true;
							iniLreader->_oLantern->ThresApp = iniLreader->_oLantern->Primthresholdamp;

							if (strcmp(iniLreader->_oMando->SpiffsData.Light_Detect_Method.c_str(), "AI"))
								iniLreader->_oMando->M6data.LNyala = Light.toInt();

						}
					}
				}
			}
		}

		if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE")) {
			iniLreader->_oLantern->lanternRespond = 4;
		}

	}



	if (strcmp(iniLreader->_oMando->SpiffsData.Calibrate.c_str(),"No")) {
		iniLreader->_oMando->SpiffsData.Calibrate = "No";
	}

	if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-LIGHTHOUSE")) {
		if (strcmp(iniLreader->_oMando->SpiffsData.Lantern_Lost.c_str(), "0")) {
			iniLreader->_oMando->SpiffsData.Lantern_Lost = "0";
		}
		if (strcmp(iniLreader->_oMando->SpiffsData.Sec_Mon.c_str(), "No")) {
			iniLreader->_oMando->SpiffsData.Sec_Mon = "No";
		}
		if (strcmp(iniLreader->_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
			iniLreader->_oMando->SpiffsData.RACON_Mon = "No";
		}
		if (strcmp(iniLreader->_oMando->SpiffsData.Use_LDR.c_str(), "No")) {
			iniLreader->_oMando->SpiffsData.Use_LDR = "No";
		}
		if (!strcmp(iniLreader->_oMando->SpiffsData.DAC.c_str(), "533")) {
			if (strcmp(iniLreader->_oMando->SpiffsData.FI.c_str(),"1")) {
				iniLreader->_oMando->SpiffsData.FI = "1";
			}
		}
		if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(),"GF-LR-BUOY")) {
			iniLreader->_oMando->SpiffsData.Format = "GF-LR-BUOY";
		}
	}
	log_i("$$$$$$$$$$$$$$$$$$$$$$");
	log_i("$$$$$$ GF-LR-BUOY");
	log_i("$$$$$$ LanternStat = %s", iniLreader->_oLantern->LanternStat.c_str());

	if (iniLreader->_oLantern->LanternStat == "Locked") {
		log_i("$$$$$$$$$$$$$$$$$$$$$$");
		log_i("$$$$$$ LVin = %f", iniLreader->_oMando->M6data.LVin);
		log_i("$$$$$$ PrimMaxMinDiff = %f", iniLreader->_oLantern->PrimMaxMinDiff);
		log_i("$$$$$$ Primthresholdamp = %f", iniLreader->_oLantern->Primthresholdamp);
		log_i("$$$$$$ ThresApp = %f", iniLreader->_oLantern->ThresApp);
		log_i("$$$$$$ LNyala = %d", iniLreader->_oMando->M6data.LNyala);
	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$");


}

void LReader::Lighthouse() {
	String n = "";
	if (iniLreader->_oMando->ProcAtonbit_prev != "XX") {
		for (int x = 0; x < 2; x ++) {
			char k = iniLreader->_oMando->ProcAtonbit_prev[x];
			int l = int(k);
			l = l - 48;
			if (l >= 17) {
				l = l - 7;
			}
			String m = String(l, BIN);
			if (m.length() == 3)       m = "0" + m;
			else if (m.length() == 2)  m = "00" + m;
			else if (m.length() == 1)  m = "000" + m;
			n += m;
		}
	}
	else {
		n = "XX";
	}
	//  jumpaLR = true;
	_server.send(200, "application/json",  n);

	if (!iniLreader->_oLantern->jumpaSC35) {
		if (!iniLreader->_oLantern->lanternlock) {
			for (int i = 0; i < _server.args(); i++) {
				if (_server.argName(i) == "req") {

					String ss = _server.arg(i);
					const size_t bufferSize = JSON_OBJECT_SIZE(6) + 90; //asalnya JSON_OBJECT_SIZE(10) + 140;
					DynamicJsonDocument root(bufferSize);
					deserializeJson(root, ss);
					String Status = root["Status"];
					String Runtime = root["Counting"];
					iniLreader->_oLantern->LanternStat = Status;

					if (iniLreader->_oLantern->LanternStat == "Locked") {
						iniLreader->_oLantern->lanternlockC = true;
						String LV = root["LV"];
						String RV = root["RV"];
						String FC = root["FC"];  //0 means open. not contact. 1 means contact;
						String D = root["D"];
						String LDR_ = root["LDR"];
						iniLreader->_oMando->M6data.LDRStatus = LDR_.toInt();
						if (strcmp(iniLreader->_oMando->SpiffsData.RACON_Mon.c_str(), "No")) {
							iniLreader->_oMando->M6data.RVin = RV.toFloat();
						}
						iniLreader->_oMando->M6data.LVin = LV.toFloat();
						iniLreader->_oLantern->FreeContact = FC.toInt();

						String welp;
						welp = D[0];  iniLreader->_oMando->M6data.MLCond = welp.toInt();
						welp = D[1];  iniLreader->_oMando->M6data.MLStat = welp.toInt();    //0-Off;1-On
						welp = D[2];  iniLreader->_oMando->M6data.SLCond = welp.toInt();    //0-Normal;1-Fail
						welp = D[3];  iniLreader->_oMando->M6data.SLStat = welp.toInt();    //0-Off;1-On
						welp = D[6];  iniLreader->_oMando->M6data.D1Stat = welp.toInt();    //0-Off;1-On
						welp = D[7];  iniLreader->_oMando->M6data.D1Cond = welp.toInt();    //0-Normal;1-Fail
						welp = D[8];  iniLreader->_oMando->M6data.D2Stat = welp.toInt();    //0-Off;1-On
						welp = D[9];  iniLreader->_oMando->M6data.D2Cond = welp.toInt();    //0-Normal;1-Fail
						welp = D[10];  iniLreader->_oMando->M6data.Door = welp.toInt();     //0-Close;1-Open
						welp = D[11];  iniLreader->_oMando->M6data.ACPower = welp.toInt();  //0-Off;1-On
						welp = D[12];  iniLreader->_oMando->M6data.BMS = welp.toInt();      //0-Normal;1-Fail
						welp = D[13];  iniLreader->_oLantern->PSU = welp.toInt();      //0-No;1-Yes
						welp = D[14];  iniLreader->_oLantern->Xtra1 = welp.toInt();    //0-No;1-Yes
						welp = D[15];  iniLreader->_oLantern->Xtra2 = welp.toInt();    //0-No;1-Yes

						if (iniLreader->_oMando->M6data.MLStat | iniLreader->_oMando->M6data.SLStat) {
							iniLreader->_oMando->M6data.LNyala = 1;
						}
						if (iniLreader->_oLantern->lanternlockB & iniLreader->_oLantern->lanternlockC) {
							iniLreader->_oLantern->LanternLongStatus = "Data locked.";
						}
					}
				}
			}
		}
	}
	iniLreader->_oLantern->lanternRespond = 5;
	if (strcmp(iniLreader->_oMando->SpiffsData.Sec_Mon.c_str(), "Yes")) {
		iniLreader->_oMando->SpiffsData.Sec_Mon = "Yes";
	}
	if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(), "GF-LR-LIGHTHOUSE")) {
		iniLreader->_oMando->SpiffsData.Format = "GF-LR-LIGHTHOUSE";
	}
	if (!strcmp(iniLreader->_oMando->SpiffsData.DAC.c_str(), "533")) {
		if (strcmp(iniLreader->_oMando->SpiffsData.FI.c_str(), "2")) {
			iniLreader->_oMando->SpiffsData.FI = "2";
		}
	}

	if (strcmp(iniLreader->_oMando->SpiffsData.Use_LDR.c_str(), "Yes")) {
		iniLreader->_oMando->SpiffsData.Use_LDR = "Yes";
	}
	if (strcmp(iniLreader->_oMando->SpiffsData.Lantern_Lost.c_str(), "0")) {
		iniLreader->_oMando->SpiffsData.Lantern_Lost = "0";
	}
}

void LReader::Beacon() {
	if (!iniLreader->_oLantern->jumpaSC35) {
		if (!iniLreader->_oLantern->lanternlock) {
			for (int i = 0; i < _server.args(); i++) {
				if (_server.argName(i) == "req") {

					String ss = _server.arg(i);
					const size_t bufferSize = JSON_OBJECT_SIZE(6) + 90;
					DynamicJsonDocument root(bufferSize);
					deserializeJson(root, ss);
					String Status = root["Status"];
					String Runtime = root["Counting"];
					iniLreader->_oLantern->LanternStat = Status;

					if (iniLreader->_oLantern->LanternStat == "Booting") {
						iniLreader->_oLantern->LanternLongStatus = "Booting. Countdown ";
						iniLreader->_oLantern->LanternLongStatus += Runtime;
						iniLreader->_oLantern->LanternLongStatus += "s.";
					}
					else if (iniLreader->_oLantern->LanternStat == "Calibrating") {
						iniLreader->_oLantern->LanternLongStatus = "Calibrating. Countdown: ";   //dah
						iniLreader->_oLantern->LanternLongStatus += Runtime;                     //dah
						iniLreader->_oLantern->LanternLongStatus += "s.";
					}
					else if (iniLreader->_oLantern->LanternStat == "Processing") {
						iniLreader->_oLantern->LanternLongStatus = "Processing. Countdown: ";   //dah
						iniLreader->_oLantern->LanternLongStatus += Runtime;                     //dah
						iniLreader->_oLantern->LanternLongStatus += "s.";
						String Voltage = root["Voltage"];
						String DiffPrim = root["DiffPrim"];
						String ThresPrim = root["ThresPrim"];
						String DiffSec = root["DiffSec"];
						String ThresSec = root["ThresSec"];
						String LightPrim = root["LightPrim"];                     //decision is made by Lantern Reader.
						String LightSec = root["LightSec"];                     //decision is made by Lantern Reader.
						String RaconVoltage = root["RACONVoltage"];
						iniLreader->_oMando->M6data.RVin = RaconVoltage.toFloat();
						String RaconContact = root["RACONContact"];         //0 open; 1 contact. decide based on configuration.
						String LDR_ = root["LDR"];
						iniLreader->_oMando->M6data.LDRStatus = LDR_.toInt();
						String Door_ = root["Door"];
						iniLreader->_oMando->M6data.Door = Door_.toInt();
						iniLreader->_oMando->M6data.LVin = Voltage.toFloat();
						iniLreader->_oLantern->FreeContact = RaconContact.toInt();

						iniLreader->_oLantern->PrimMaxMinDiff = DiffPrim.toFloat();
						iniLreader->_oLantern->Primthresholdamp = ThresPrim.toFloat();

						iniLreader->_oLantern->SecMaxMinDiff = DiffSec.toFloat();
						iniLreader->_oLantern->Secthresholdamp = ThresSec.toFloat();

						iniLreader->_oMando->M6data.LNyala = LightPrim.toInt();
						iniLreader->_oMando->M6data.SLNyala = LightSec.toInt();
					}
					else if (iniLreader->_oLantern->LanternStat == "Locked") {
						iniLreader->_oLantern->lanternlock = true;
						iniLreader->_oLantern->LanternLongStatus = "Data locked.";
						String Voltage = root["Voltage"];
						String DiffPrim = root["DiffPrim"];
						String ThresPrim = root["ThresPrim"];
						String DiffSec = root["DiffSec"];
						String ThresSec = root["ThresSec"];
						String LightPrim = root["LightPrim"];                     //decision is made by Lantern Reader.
						String LightSec = root["LightSec"];                     //decision is made by Lantern Reader.
						String RaconVoltage = root["RACONVoltage"];
						iniLreader->_oMando->M6data.RVin = RaconVoltage.toFloat();
						String RaconContact = root["RACONContact"];         //0 open; 1 contact. decide based on configuration.
						String LDR_ = root["LDR"];
						iniLreader->_oMando->M6data.LDRStatus = LDR_.toInt();
						String Door_ = root["Door"];
						iniLreader->_oMando->M6data.Door = Door_.toInt();
						iniLreader->_oMando->M6data.LVin = Voltage.toFloat();
						iniLreader->_oLantern->FreeContact = RaconContact.toInt();

						iniLreader->_oLantern->PrimMaxMinDiff = DiffPrim.toFloat();
						iniLreader->_oLantern->Primthresholdamp = ThresPrim.toFloat();

						iniLreader->_oLantern->SecMaxMinDiff = DiffSec.toFloat();
						iniLreader->_oLantern->Secthresholdamp = ThresSec.toFloat();

						iniLreader->_oMando->M6data.LNyala = LightPrim.toInt();
						iniLreader->_oMando->M6data.SLNyala = LightSec.toInt();

						if (!iniLreader->_oMando->M6data.LNyala && (iniLreader->_oMando->M6data.LDRStatus == 3 || iniLreader->_oMando->M6data.LDRStatus == 2 || iniLreader->_oMando->M6data.LDRStatus == 0))
							iniLreader->_oLantern->LanternLongStatus += "\nNo Light";
						else if(iniLreader->_oMando->M6data.LNyala)
							iniLreader->_oLantern->LanternLongStatus += "\nOn Primary";
						else if(iniLreader->_oMando->M6data.SLNyala)
							iniLreader->_oLantern->LanternLongStatus += "\nOn Secondary";
						else
							iniLreader->_oLantern->LanternLongStatus += "\nLight Error";
					}

				}
			}
		}
	}
	iniLreader->_oLantern->lanternRespond = 3;
	iniLreader->_oLantern->jumpaLR = true;
	_server.send(200, "application/json",  String(iniLreader->_oMando->SpiffsData.Calibrate) + "," + iniLreader->_oMando->SpiffsData.Sec_Mon + "," + iniLreader->_oMando->SpiffsData.Calib_Prim_M + "," + iniLreader->_oMando->SpiffsData.Calib_Sec_M + "," + iniLreader->_oMando->SpiffsData.Light_Detect_Method + "," + iniLreader->_oMando->SpiffsData.Reboot_Mode);

	if (strcmp(iniLreader->_oMando->SpiffsData.Calibrate.c_str(), "No")) {
		iniLreader->_oMando->SpiffsData.Calibrate = "No";
	}

	if (strcmp(iniLreader->_oMando->SpiffsData.Format.c_str(), "GF-LR-BEACON")) {
		iniLreader->_oMando->SpiffsData.Format = "GF-LR-BEACON";
	}

	if (!strcmp(iniLreader->_oMando->SpiffsData.DAC.c_str(), "533")) {
		if (strcmp(iniLreader->_oMando->SpiffsData.FI.c_str(), "1"))
			iniLreader->_oMando->SpiffsData.FI = "1";
	}

	if (strcmp(iniLreader->_oMando->SpiffsData.Use_LDR.c_str(), "YES")) {
		iniLreader->_oMando->SpiffsData.Use_LDR = "YES";
	}

	if (strcmp(iniLreader->_oMando->SpiffsData.Lantern_Lost.c_str(), "0")) {
		iniLreader->_oMando->SpiffsData.Lantern_Lost = "0";
	}
}
