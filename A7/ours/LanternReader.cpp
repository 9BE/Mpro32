/*
 * LanternReader.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: suhaimi
 */

#include <LanternReader.h>

LanternReader::LanternReader(DataHandler * data, WebServer * server) {
	this->_oData = data;
	this->_server = server;

	xTaskCreatePinnedToCore(this->loop, "LanternReader", 2048, this, 1,
			&this->_handle, 1);
}

void LanternReader::loop(void * param) {
	LanternReader * ini = (LanternReader*) param;

	while (1) {
		// check if data valid/locked than update m6 data

		if (!ini->_oData->isLRactive(f_sc35ada)) {

		}

		delay(10);
	}
}

int LanternReader::getStat(String& cari) {
	int ret = -1;
	auto it = std::find(this->_statList, this->_statList+this->_statSize, cari);

	if (it != std::end(this->_statList)) {
		ret = std::distance(this->_statList, it);
	}

	return ret;
}

void LanternReader::buoy(LanternReader* ini) {
	String n = "";
	if (ini->_tempAtonBit != "XX") {
		// amek bit dr hex to binary 4 bit sahaja
		for (int x = 0; x < 2; x ++) {
			char k = ini->_tempAtonBit[x];
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

	ini->_server->send(200, "application/json",  n + "," + ini->_oData->SpiffsData.Calibrate + "," + ini->_oData->SpiffsData.Calib_Prim_M + "," + ini->_oData->SpiffsData.Light_Detect_Method);

	ini->_oData->LRactivate(f_lrBuoy);

	if (!ini->_oData->isLRactive(f_sc35ada)) {
		if (!ini->_locked) {
			String ss = ini->_server->arg("req");
			if (!ss.equals("")) {

				const size_t bufferSize = JSON_OBJECT_SIZE(11) + 150; //asalnya JSON_OBJECT_SIZE(10) + 140;
				DynamicJsonDocument root(bufferSize);
				deserializeJson(root,ss);
				String Status = root["Status"];
				String Runtime = root["Countdown"];

				ini->_oData->LRstat = ini->getStat(Status);

				if (ini->_oData->LRstat == lrs_booting) {
					String Thr = root["ThresholdPrim"];
					if (ini->_oData->SpiffsData.Format == f_lrLH) {
						ini->_oData->LRemcStat = lrs_booting;
					}
					else {
						ini->_oData->LRlongStat = "Booting. Countdown ";        //dah
						ini->_oData->LRlongStat += Runtime;                     //dah
						ini->_oData->LRlongStat += "s.";                        //dah
						ini->_oData->LRlongStat += "\nThres: ";                        //dah
						ini->_oData->LRlongStat += Thr;
					}
				}
				else if (ini->_oData->LRstat == lrs_calibrating) {
					if (ini->_oData->SpiffsData.Format != f_lrLH) {
						ini->_oData->LRlongStat = "Calibrating. Countdown: ";   //dah
						ini->_oData->LRlongStat += Runtime;                     //dah
						ini->_oData->LRlongStat += "s.";
					}

				}
				else if (ini->_oData->LRstat == lrs_processing) {

					String Voltage = root["Voltage"];                 //dah
					String DiffPrimary = root["DiffPrimary"];         //dah
					String Thr = root["ThresholdPrim"];               //dah
					String Light = root["Light"];                     //dah

					if (ini->_oData->SpiffsData.Format == f_lrLH && !ini->_lockedB) {
						ini->_oData->LRemcStat = lrs_processing;
						ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrimary.toFloat();
						ini->_oData->LreaderData.Primthresholdamp = Thr.toFloat();
						ini->_oData->LreaderData.ELStat = Light.toInt();
						float vin = Voltage.toFloat();
						if (vin > 5) {
							ini->_oData->LreaderData.ELVin = vin;
						}
					}
					else {
						ini->_oData->LRlongStat = "Processing. Countdown ";     //dah
						ini->_oData->LRlongStat += Runtime;                     //dah
						ini->_oData->LRlongStat += "s.";                        //dah

						ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrimary.toFloat();
						ini->_oData->LreaderData.Primthresholdamp = Thr.toFloat();
						ini->_oData->LreaderData.LNyala = Light.toInt();
						float vin = Voltage.toFloat();
						if (vin > 5) {
							ini->_oData->LreaderData.LVin = vin;
						}
					}

				}
				else if (ini->_oData->LRstat == lrs_locked) {

					String Voltage = root["Voltage"];                 //dah
					String DiffPrimary = root["DiffPrimary"];         //dah
					String Thr = root["ThresholdPrim"];               //dah
					String Light = root["Light"];                     //dah
					int lampu = Light.toInt();

					if (lampu == 1) {
						ini->_tempAtonBit = "E2";
					}
					else if (lampu == 0) {
						ini->_tempAtonBit = "E4";
					}

					if (ini->_oData->SpiffsData.Format == f_lrLH && !ini->_lockedB) {
						float vin = Voltage.toFloat();
						ini->_oData->LreaderData.ELVin = vin;
						ini->_oData->LRemcStat = lrs_locked;
						ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrimary.toFloat();
						ini->_oData->LreaderData.Primthresholdamp = Thr.toFloat();
						ini->_oData->LreaderData.ELStat = Light.toInt();

						if (vin > 5) {
							ini->_lockedB = true;
						}


					}
					else {
						float vin = Voltage.toFloat();
						ini->_oData->LreaderData.LVin = vin;
						ini->_oData->LRlongStat = "Data locked.";     //dah

						ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrimary.toFloat();
						ini->_oData->LreaderData.Primthresholdamp = Thr.toFloat();
						ini->_oData->LreaderData.LNyala = Light.toInt();

						if (vin > 5) {
							ini->_locked = true;
						}


					}

				}
			}
		}

	}

	bool adaNakSav = false;

	if (ini->_oData->SpiffsData.Calibrate != "No") {
		ini->_oData->SpiffsData.Calibrate = "No";
		adaNakSav = true;
	}

	if (ini->_oData->SpiffsData.Format != f_lrLH) {
		if (ini->_oData->SpiffsData.Sec_Mon != "No") {
			ini->_oData->SpiffsData.Sec_Mon = "No";
			adaNakSav = true;
		}

		if (ini->_oData->SpiffsData.RACON_Mon != "No") {
			ini->_oData->SpiffsData.RACON_Mon = "No";
			adaNakSav = true;
		}

		if (ini->_oData->SpiffsData.Use_LDR != "No") {
			ini->_oData->SpiffsData.Use_LDR = "No";
			adaNakSav = true;
		}

		if (ini->_oData->SpiffsData.DAC == "533") {
			if (ini->_oData->SpiffsData.FI != "1") {
				ini->_oData->SpiffsData.FI = "1";
				adaNakSav = true;
			}
		}

		if (ini->_oData->SpiffsData.Format != f_lrBuoy) {
			ini->_oData->SpiffsData.Format = f_lrBuoy;
			adaNakSav = true;
		}
	}

	if (adaNakSav) {
		ini->_oData->savToSpiffs();
	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$");
	log_i("$$$$$$ GF-LR-BUOY");
	log_i("$$$$$$ LanternStat = %d", ini->_oData->LRstat);

	if (ini->_oData->LRstat == lrs_locked) {
		log_i("$$$$$$$$$$$$$$$$$$$$$$");
		if (ini->_oData->SpiffsData.Format == f_lrLH) {

			log_i("$$$$$$ PrimMaxMinDiff = %f", ini->_oData->LreaderData.PrimMaxMinDiff);
			log_i("$$$$$$ Primthresholdamp = %f", ini->_oData->LreaderData.Primthresholdamp);
			log_i("$$$$$$ LNyala = %d", ini->_oData->LreaderData.LNyala);
			log_i("$$$$$$ ELStat = %d", ini->_oData->LreaderData.ELStat);
			log_i("$$$$$$ ELCond prev = %d", ini->_oData->LreaderData.ELCond);

		}
		else {

			log_i("$$$$$$ LVin = %f", ini->_oData->LreaderData.LVin);
			log_i("$$$$$$ PrimMaxMinDiff = %f", ini->_oData->LreaderData.PrimMaxMinDiff);
			log_i("$$$$$$ Primthresholdamp = %f", ini->_oData->LreaderData.Primthresholdamp);
//			log_i("$$$$$$ ThresApp = %f", iniLreader->_oLantern->ThresApp);
			log_i("$$$$$$ LNyala = %d", ini->_oData->LreaderData.LNyala);
		}

	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$  ==>> %d", millis()/1000);


}

void LanternReader::lighthouse(LanternReader* ini) {
	String n = "";
	if (ini->_oData->procAtonBit_prev != "NDA") {
		// amek bit dr hex to binary 4 bit sahaja
		for (int x = 0; x < 2; x ++) {
			char k = ini->_oData->procAtonBit_prev[x];
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

	ini->_server->send(200, "application/json",  n);

	ini->_oData->LRactivate(f_lrLH);

	if (!ini->_oData->isLRactive(f_sc35ada)) {
		if (!ini->_locked) {
			String ss = ini->_server->arg("req");
			if (!ss.equals("")) {

				log_i("Lighthouse req :: %s", ss.c_str());
				const size_t bufferSize = JSON_OBJECT_SIZE(6) + 90; //asalnya JSON_OBJECT_SIZE(10) + 140;
				DynamicJsonDocument root(bufferSize);
				deserializeJson(root, ss);
				String Status = root["Status"];
				String Runtime = root["Counting"];

				ini->_oData->LRstat = ini->getStat(Status);

				if (ini->_oData->LRstat == lrs_locked) {
					ini->_lockedC = true;
					String LV = root["LV"];
					String RV = root["RV"];
					String FC = root["FC"];  //0 means open. not contact. 1 means contact;
					String D = root["D"];
					String LDR = root["LDR"];

					ini->_oData->LreaderData.LVin = LV.toFloat();
					ini->_oData->LreaderData.RVin = RV.toFloat();
					ini->_oData->LreaderData.Rcontact = FC.toInt();
					ini->_oData->LreaderData.LDRStatus = LDR.toInt();

					String welp;
					welp = D.charAt(0);		ini->_oData->LreaderData.MLCond = welp.toInt();
					welp = D.charAt(1);		ini->_oData->LreaderData.MLStat = welp.toInt();
					welp = D.charAt(2);		ini->_oData->LreaderData.SLCond = welp.toInt();
					welp = D.charAt(3);		ini->_oData->LreaderData.SLStat = welp.toInt();
					welp = D.charAt(6);		ini->_oData->LreaderData.D1Stat = welp.toInt();
					welp = D.charAt(7);		ini->_oData->LreaderData.D1Cond = welp.toInt();
					welp = D.charAt(8);		ini->_oData->LreaderData.D2Stat = welp.toInt();
					welp = D.charAt(9);		ini->_oData->LreaderData.D2Cond = welp.toInt();
					welp = D.charAt(10);	ini->_oData->LreaderData.Door = welp.toInt();
					welp = D.charAt(11);	ini->_oData->LreaderData.ACPower = welp.toInt();
					welp = D.charAt(12);	ini->_oData->LreaderData.BMS = welp.toInt();
					welp = D.charAt(13);	ini->_oData->LreaderData.PSU = welp.toInt();
					welp = D.charAt(14);	ini->_oData->LreaderData.Xtra1 = welp.toInt();
					welp = D.charAt(15);	ini->_oData->LreaderData.Xtra2 = welp.toInt();

				}


			} // !ss.equals("")


		} // !ini->_locked


	} // !ini->_oData->isLRactive(f_sc35ada)



	bool adaNakSav = false;

	if (ini->_oData->SpiffsData.Sec_Mon != "Yes") {
		ini->_oData->SpiffsData.Sec_Mon = "Yes";
		adaNakSav = true;
	}

	if (ini->_oData->SpiffsData.Format != f_lrLH) {
		ini->_oData->SpiffsData.Format = f_lrLH;
		adaNakSav = true;
	}

	if (ini->_oData->SpiffsData.DAC == "533") {
		if (ini->_oData->SpiffsData.FI != "2") {
			ini->_oData->SpiffsData.FI = "2";
			adaNakSav = true;
		}
	}

	if (ini->_oData->SpiffsData.Use_LDR != "Yes") {
		ini->_oData->SpiffsData.Use_LDR = "Yes";
		adaNakSav = true;
	}

	if (adaNakSav) {
		ini->_oData->savToSpiffs();
	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$");
	log_i("$$$$$$ GF-LR-LIGHTHOUSE");
	log_i("$$$$$$ LanternStat = %d", ini->_oData->LRstat);

	if (ini->_oData->LRstat == lrs_locked) {
		log_i("$$$$$$$$$$$$$$$$$$$$$$");
		log_i("$$$$$$ LVin = %.02f", ini->_oData->LreaderData.LVin);
		log_i("$$$$$$ LDRStatus = %d", ini->_oData->LreaderData.LDRStatus);
		log_i("$$$$$$ RVin = %.02f", ini->_oData->LreaderData.RVin);
		log_i("$$$$$$ FreeContact = %d", ini->_oData->LreaderData.Rcontact);
		log_i("$$$$$$$$$$$$$$$$$$$$$$");
		log_i("$$$$$$ MLCond = %d", ini->_oData->LreaderData.MLCond);
		log_i("$$$$$$ MLStat = %d", ini->_oData->LreaderData.MLStat);
		log_i("$$$$$$ SLCond = %d", ini->_oData->LreaderData.SLCond);
		log_i("$$$$$$ SLStat = %d", ini->_oData->LreaderData.SLStat);
		log_i("$$$$$$ D1Stat = %d", ini->_oData->LreaderData.D1Stat);
		log_i("$$$$$$ D1Cond = %d", ini->_oData->LreaderData.D1Cond);
		log_i("$$$$$$ D2Stat = %d", ini->_oData->LreaderData.D2Stat);
		log_i("$$$$$$ D2Cond = %d", ini->_oData->LreaderData.D2Cond);
		log_i("$$$$$$ Door = %d", ini->_oData->LreaderData.Door);
		log_i("$$$$$$ ACPower = %d", ini->_oData->LreaderData.ACPower);
		log_i("$$$$$$ BMS = %d", ini->_oData->LreaderData.BMS);
		log_i("$$$$$$ PSU = %d", ini->_oData->LreaderData.PSU);
		log_i("$$$$$$ Xtra1 = %d", ini->_oData->LreaderData.Xtra1);
		log_i("$$$$$$ Xtra2 = %d", ini->_oData->LreaderData.Xtra2);


	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$  ==>> %d", millis()/1000);


}

void LanternReader::beacon(LanternReader* ini) {

	ini->_server->send(200, "application/json",  String(ini->_oData->SpiffsData.Calibrate) + "," + ini->_oData->SpiffsData.Sec_Mon + "," + ini->_oData->SpiffsData.Calib_Prim_M + "," + ini->_oData->SpiffsData.Calib_Sec_M + "," + ini->_oData->SpiffsData.Light_Detect_Method + "," + ini->_oData->SpiffsData.Reboot_Mode);
	ini->_oData->LRactivate(f_lrBeacon);

	if (!ini->_oData->isLRactive(f_sc35ada)) {
		if (!ini->_locked) {
			String ss = ini->_server->arg("req");
			if (!ss.equals("")) {
				const size_t bufferSize = JSON_OBJECT_SIZE(6) + 90; //asalnya JSON_OBJECT_SIZE(10) + 140;
				DynamicJsonDocument root(bufferSize);
				deserializeJson(root, ss);
				String Status = root["Status"];
				String Runtime = root["Counting"];

				ini->_oData->LRstat = ini->getStat(Status);

				if (ini->_oData->LRstat == lrs_booting) {
					// ini->_oData->LRlongStat
					ini->_oData->LRlongStat = "Booting. Countdown ";
					ini->_oData->LRlongStat += Runtime;
					ini->_oData->LRlongStat += "s.";

				}
				else if (ini->_oData->LRstat == lrs_calibrating) {
					ini->_oData->LRlongStat = "Calibrating. Countdown ";   //dah
					ini->_oData->LRlongStat += Runtime;                     //dah
					ini->_oData->LRlongStat += "s.";

				}
				else if (ini->_oData->LRstat == lrs_processing) {
					ini->_oData->LRlongStat = "Processing. Countdown ";   //dah
					ini->_oData->LRlongStat += Runtime;                     //dah
					ini->_oData->LRlongStat += "s.";

					String Voltage = root["Voltage"];
					String DiffPrim = root["DiffPrim"];
					String ThresPrim = root["ThresPrim"];
					String DiffSec = root["DiffSec"];
					String ThresSec = root["ThresSec"];
					String LightPrim = root["LightPrim"];                     //decision is made by Lantern Reader.
					String LightSec = root["LightSec"];                     //decision is made by Lantern Reader.
					String RaconVoltage = root["RACONVoltage"];
					String RaconContact = root["RACONContact"];         //0 open; 1 contact. decide based on configuration.
					String LDR = root["LDR"];
					String Door = root["Door"];

					ini->_oData->LreaderData.LVin = Voltage.toFloat();
					ini->_oData->LreaderData.RVin = RaconVoltage.toFloat();
					ini->_oData->LreaderData.Rcontact = RaconContact.toInt();
					ini->_oData->LreaderData.LDRStatus = LDR.toInt();
					ini->_oData->LreaderData.Door = Door.toInt();

					ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrim.toFloat();
					ini->_oData->LreaderData.Primthresholdamp = ThresPrim.toFloat();

					ini->_oData->LreaderData.SecMaxMinDiff = DiffSec.toFloat();
					ini->_oData->LreaderData.Secthresholdamp = ThresSec.toFloat();

					ini->_oData->LreaderData.LNyala = LightPrim.toInt();
					ini->_oData->LreaderData.SLNyala = LightSec.toInt();

				}
				else if (ini->_oData->LRstat == lrs_locked) {

					ini->_oData->LRlongStat = "Data locked.";

					String Voltage = root["Voltage"];
					float vin = Voltage.toFloat();
					String DiffPrim = root["DiffPrim"];
					String ThresPrim = root["ThresPrim"];
					String DiffSec = root["DiffSec"];
					String ThresSec = root["ThresSec"];
					String LightPrim = root["LightPrim"];                     //decision is made by Lantern Reader.
					String LightSec = root["LightSec"];                     //decision is made by Lantern Reader.
					String RaconVoltage = root["RACONVoltage"];
					String RaconContact = root["RACONContact"];         //0 open; 1 contact. decide based on configuration.
					String LDR = root["LDR"];
					String Door = root["Door"];

					ini->_oData->LreaderData.LVin = vin;
					ini->_oData->LreaderData.RVin = RaconVoltage.toFloat();
					ini->_oData->LreaderData.Rcontact = RaconContact.toInt();
					ini->_oData->LreaderData.LDRStatus = LDR.toInt();
					ini->_oData->LreaderData.Door = Door.toInt();

					ini->_oData->LreaderData.PrimMaxMinDiff = DiffPrim.toFloat();
					ini->_oData->LreaderData.Primthresholdamp = ThresPrim.toFloat();

					ini->_oData->LreaderData.SecMaxMinDiff = DiffSec.toFloat();
					ini->_oData->LreaderData.Secthresholdamp = ThresSec.toFloat();

					ini->_oData->LreaderData.LNyala = LightPrim.toInt();
					ini->_oData->LreaderData.SLNyala = LightSec.toInt();

					if (!ini->_oData->LreaderData.LNyala && (ini->_oData->LreaderData.LDRStatus == 3 || ini->_oData->LreaderData.LDRStatus == 2 || ini->_oData->LreaderData.LDRStatus == 0)) {
						ini->_oData->LRlongStat += "\nNo Light";
					}
					else if (ini->_oData->LreaderData.LNyala) {
						ini->_oData->LRlongStat += "\nOn Primary";
					}
					else if (ini->_oData->LreaderData.SLNyala) {
						ini->_oData->LRlongStat += "\nOn Secondary";
					}
					else {
						ini->_oData->LRlongStat += "\nLight Error";
					}

					if (vin > 5) {
						ini->_locked = true;
					}
				}

			} // !ss.equals("")
		} // !ini->_locked

	} // !ini->_oData->isLRactive(f_sc35ada)



	bool adaNakSav = false;

	if (ini->_oData->SpiffsData.Calibrate != "No") {
		ini->_oData->SpiffsData.Calibrate = "No";
		adaNakSav = true;
	}

	if (ini->_oData->SpiffsData.Use_LDR != "Yes") {
		ini->_oData->SpiffsData.Use_LDR = "Yes";
		adaNakSav = true;
	}

	if (ini->_oData->SpiffsData.DAC == "533") {
		if (ini->_oData->SpiffsData.FI != "1") {
			ini->_oData->SpiffsData.FI = "1";
			adaNakSav = true;
		}
	}

	if (ini->_oData->SpiffsData.Format != f_lrBeacon) {
		ini->_oData->SpiffsData.Format = f_lrBeacon;
		adaNakSav = true;
	}

	if (adaNakSav) {
		ini->_oData->savToSpiffs();
	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$");
	log_i("$$$$$$ GF-LR-BEACON");
	log_i("$$$$$$ LanternStat = %d", ini->_oData->LRstat);

	if (ini->_oData->LRstat == lrs_locked) {
		log_i("$$$$$$$$$$$$$$$$$$$$$$");
		log_i("$$$$$$ LVin = %f", ini->_oData->LreaderData.LVin);
		log_i("$$$$$$ PrimMaxMinDiff = %f", ini->_oData->LreaderData.PrimMaxMinDiff);
		log_i("$$$$$$ Primthresholdamp = %f", ini->_oData->LreaderData.Primthresholdamp);
		log_i("$$$$$$ LNyala = %d", ini->_oData->LreaderData.LNyala);
		log_i("$$$$$$ LDRStatus = %d", ini->_oData->LreaderData.LDRStatus);
	}

	log_i("$$$$$$$$$$$$$$$$$$$$$$  ==>> %d", millis()/1000);


}





















