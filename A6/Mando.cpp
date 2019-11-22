/*
 * Mando.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#include "Mando.h"

TaskHandle_t taskMando;
Mando * iniMando;

Mando::Mando(const BaseType_t xCoreID, uint64_t loopDelay, timing * tim) {
	// TODO Auto-generated constructor stub

	iniMando = this;
	iniMando->_loopDelay = loopDelay;
	iniMando->_oTiming = tim;

//	xTaskCreatePinnedToCore(loop, "task0", 8*1024, NULL, 1, &taskMando, xCoreID);
	xTaskCreatePinnedToCore(loop, "taskMando", 20000, NULL, 1, &taskMando, xCoreID);
}

Mando::~Mando() {
	// TODO Auto-generated destructor stub
}

void Mando::loop(void* param) {
	esp_task_wdt_add(taskMando);
	esp_task_wdt_reset();
	while (1) {
		char cr;
		if (iniMando->_mulai) {

			iniMando->hantarDataMando();// hantarDataKeMando kalo ade

			while (Serial2.available()) {
				cr = Serial2.read();

				if (cr >= 32 && cr <= 126) {	//printable char only
					iniMando->_mandoRaw.concat(cr);
				} else if (cr >= 9 && cr <= 13) {	//control char only
					iniMando->_mandoRaw.concat(cr);
				}

				if (cr == 13) {

					iniMando->_mandoRaw.trim();
					iniMando->processMandoData();

					iniMando->_mandoRaw = "";
					esp_task_wdt_reset();
				}

			}
		}

		if (iniMando->_nmea.length() > 3000) {
			iniMando->_nmea = "";
		}

		esp_task_wdt_reset();
		delay(iniMando->_loopDelay);
	}
}

void Mando::reInit() {
	mandoTask = ACE;
	_mandoTaskStat = mt_none;
}

void Mando::hantarM6() {
	mandoTask = Send06;
}

void Mando::sambung() {
	mandoTask = RMC;
	_mandoTaskStat = mt_ada;
}

void Mando::checkAtonBit() {
	mandoTask = SendBIT;
}

void Mando::processMandoData() {
	int x, y, z;
	AIS_msg * ais_msg;
	ais_msg = new AIS_msg();
	////////////// >>>>>>>>>>>>>>>>>>>>>. DEBUG START <<<<<<<<<<<<<<<<<<<<<, //////////////////
	x = _mandoRaw.indexOf("*");
	y = _mandoRaw.lastIndexOf('!', x);
	z = _mandoRaw.lastIndexOf('$', x);
	if (x>=0 && (y>=0 || z>=0)) {
		if (z > y) {
			y = z;
		}
		String _debugRaw = _mandoRaw.substring(y, x+3);
		if (ais_msg->validate(_debugRaw)) {
			_nmea += _debugRaw;
			_nmea += "</br>";
			_masaNMEA = millis();
		}
	}
	////////////// >>>>>>>>>>>>>>>>>>>>>. DEBUG END <<<<<<<<<<<<<<<<<<<<<, //////////////////


	x = _mandoRaw.indexOf("*");
	y = _mandoRaw.indexOf(getLookUpHeader());
	if (x >= 0 && y >= 0) {
		String _processRaw = _mandoRaw.substring(y,x+3);
		if (ais_msg->validate(_processRaw)) {
			log_i("validated _processRaw ::: %s", _processRaw.c_str());
			String tempY;
			switch (mandoTask) {
			case mt_none:
				break;
			case ACE:
				MFullTextAce = _processRaw;

				tempY = ais_msg->getPara(_processRaw, 1); // parameter 1 = mmsi
				MTemporaryMMSI = tempY;
				_mConfig.MMmsi = tempY;
				_machine.id = tempY;

				ProcAtonbit_prev = ais_msg->getPara(_processRaw, 2); // parameter 2 = aton status bit

				tempY = ais_msg->getPara(_processRaw, 3); // parameter 3 = off-position threshold
				_mConfig.MOffThres = tempY.toInt();

				_mConfig.MName = ais_msg->getPara(_processRaw, 7); // parameter 7 = Name
				_machine.name = _mConfig.MName;

				tempY = ais_msg->getPara(_processRaw, 8);
				_mConfig.MLengthA = String(tempY.substring(0, 3));
				_mConfig.MLengthB = String(tempY.substring(3, 6));
				_mConfig.MLengthC = String(tempY.substring(6, 8));
				_mConfig.MLengthD = String(tempY.substring(8, 10));

				if (MTemporaryMMSI != "") {
					_inputMandoAce = true;
					iniMando->_mandoTaskStat = ACE;
					mandoTask = ACF;
				}

				break;
			case ACF:
				MFullTextAcf = _processRaw;

				tempY = ais_msg->getPara(_processRaw, 2);
//				tempY.toCharArray(_mConfig.MEpfs, 2);
				strncpy(_mConfig.MEpfs, tempY.c_str(), sizeof(_mConfig.MEpfs));

				MTempStrRegLat = ais_msg->getPara(_processRaw, 3);

				tempY = ais_msg->getPara(_processRaw, 4);
//				tempY.toCharArray(_mConfig.MRegCardinalLat, 2);
				strncpy(_mConfig.MRegCardinalLat, tempY.c_str(), sizeof(_mConfig.MRegCardinalLat));

				MTempStrRegLng = ais_msg->getPara(_processRaw, 5);

				tempY = ais_msg->getPara(_processRaw, 6);
//				tempY.toCharArray(_mConfig.MRegCardinalLng, 2);
				strncpy(_mConfig.MRegCardinalLng, tempY.c_str(), sizeof(_mConfig.MRegCardinalLng));

				tempY = ais_msg->getPara(_processRaw, 7);
//				tempY.toCharArray(_mConfig.MPosAcc, 2);
				strncpy(_mConfig.MPosAcc, tempY.c_str(), sizeof(_mConfig.MPosAcc));

				tempY = ais_msg->getPara(_processRaw, 13);
//				tempY.toCharArray(_mConfig.MAtonType, 3);
				strncpy(_mConfig.MAtonType, tempY.c_str(), sizeof(_mConfig.MAtonType));



				tmpLat = ais_msg->getGps(MTempStrRegLat, _mConfig.MRegCardinalLat);
				tmpLng = ais_msg->getGps(MTempStrRegLng, _mConfig.MRegCardinalLng);

				if (tmpLat < 0) {
					tmpLat = tmpLat * -1;
				}
				if (tmpLng < 0) {
					tmpLng = tmpLng * -1;
				}

				_mConfig.MConfigLat = String(tmpLat, 4);
				_mConfig.MConfigLng = String(tmpLng, 4);

				iniMando->_mandoTaskStat = ACF;
				mandoTask = VER;

				break;
			case VER:

				tempY = ais_msg->getPara(_processRaw, 7); // parameter 7 = SerialNumber
				tempY.toCharArray(MSerialNumber, 10);
				strncpy(MSerialNumber, tempY.c_str(), sizeof(MSerialNumber));

				log_i("SERIAL NUMBER :: %s", tempY.c_str());

				iniMando->_mandoTaskStat = VER;
				mandoTask = AAR21;

				break;
			case AAR06:
				tempY = ais_msg->getPara(_processRaw, 2); // parameter 7 = SerialNumber
				if (tempY == "06") {

					_mConfig.M06UTChourA = ais_msg->getPara(_processRaw, 4);
					_mConfig.M06UTCminA = ais_msg->getPara(_processRaw, 5);
					M06IntervalA = ais_msg->getPara(_processRaw, 7);

					_mConfig.M06UTChourB = ais_msg->getPara(_processRaw, 9);
					_mConfig.M06UTCminB = ais_msg->getPara(_processRaw, 10);
					M06IntervalB = ais_msg->getPara(_processRaw, 12);

					_mConfig.M06IntervalABahagi60 = String((M06IntervalA.toInt()) / 60);
					_mConfig.M06IntervalBBahagi60 = String((M06IntervalB.toInt()) / 60);

					iniMando->_mandoTaskStat = AAR06;
					mandoTask = RMC;
				}
				break;
			case AAR21:
				tempY = ais_msg->getPara(_processRaw, 2); // parameter 7 = SerialNumber
				if (tempY == "21") {

					//$AIAAR,995339999,06,01,02,01,,180,1,03,04,,180,C*32
					//$ANAAR,995330984,21,01,00,00,,180,1,00,00,,180,R*20
					_mConfig.M21UTChourA = ais_msg->getPara(_processRaw, 4);
					_mConfig.M21UTCminA = ais_msg->getPara(_processRaw, 5);
					M21IntervalA = ais_msg->getPara(_processRaw, 7);

					_mConfig.M21UTChourB = ais_msg->getPara(_processRaw, 9);
					_mConfig.M21UTCminB = ais_msg->getPara(_processRaw, 10);
					M21IntervalB = ais_msg->getPara(_processRaw, 12);

					_mConfig.M21IntervalABahagi60 = String((M21IntervalA.toInt()) / 60);
					_mConfig.M21IntervalBBahagi60 = String((M21IntervalB.toInt()) / 60);

					iniMando->_mandoTaskStat = AAR21;
					mandoTask = AAR06;
				}
				break;
			case RMC:
				tempY = ais_msg->getPara(_processRaw, 2);
				if (tempY[0] == 'A') {

					// kapala
					//						int k = data.getDataFromJsonFile("config",
					//								"UTC Offset").toInt(); //but it wouldnt be an issue sangat. sbb bukan the whole 1 hour yang error. ada 1 msg je error dalam
					//					int k = atoi(ConfData.UTC_Offset);
					String GPRMCTimeStr = ais_msg->getPara(_processRaw, 1);
					String GPRMCDateStr = ais_msg->getPara(_processRaw, 9);
					String GPRMCLocLatStr = ais_msg->getPara(_processRaw, 3);
					String GPRMCLocLngStr = ais_msg->getPara(_processRaw, 5);
					String GPRMCCardinalLatStr = ais_msg->getPara(_processRaw, 4);
					String GPRMCCardinalLngStr = ais_msg->getPara(_processRaw, 6);
					lat = ais_msg->getGps(GPRMCLocLatStr, GPRMCCardinalLatStr.c_str());
					lng = ais_msg->getGps(GPRMCLocLngStr, GPRMCCardinalLngStr.c_str());

					log_i("********************* LAT : %f ************************** LNG : %f", lat, lng);

					//						now.time.jam = iniMando->getBit(GPRMCTimeStr, 0, 1) + k - 8; // asal
					_oTiming->now.time.jam = iniMando->getBit(GPRMCTimeStr, 0, 1);
					log_i("jam == %d", _oTiming->now.time.jam);

					_oTiming->now.time.minit = iniMando->getBit(GPRMCTimeStr, 2, 3);
					log_i("minit == %d", _oTiming->now.time.minit);

					_oTiming->now.time.saat = iniMando->getBit(GPRMCTimeStr, 4, 5);
					log_i("saat == %d", _oTiming->now.time.saat);

					//
					_oTiming->now.hari = iniMando->getBit(GPRMCDateStr, 0, 1);
					log_i("hari == %d", _oTiming->now.hari);

					_oTiming->now.bulan = iniMando->getBit(GPRMCDateStr, 2, 3);
					log_i("bulan == %d", _oTiming->now.bulan);

					_oTiming->now.tahun = iniMando->getBit(GPRMCDateStr, 4, 5);
					log_i("tahun == %d", _oTiming->now.tahun);

					_oTiming->now.tahun = _oTiming->now.tahun + 2000;
					_oTiming->toSecond();
					_oTiming->susunMasa(lat, lng);
					//
					// check dulu tahunnya. kalau more than 2018, baru proceed. maybe ada problem.
					if (_oTiming->now.tahun >= 2019) {
						iniMando->_inputMandoRmc = true;
						iniMando->_mandoTaskStat = RMC;
						mandoTask = VDO21;
					}

					// test saja
					//					iniMando->_mandoTaskStat++;
					//					mandoTask = none;
				}
				break;
			case VDO21:
				tempY = ais_msg->getPara(_processRaw, 5);
				//!AIVDO,1,1,,B,E=jNAGBVPW27h897h:2ab0000003`Jrk0pTQ`10880fNL0,4*12

				if (tempY[0] == 'E') { // !AIVDO,1,1,,B,E>m>?WBVPW27h8;WP00000000003`JtL0pTN`20@@@SN@0,4*2D
					ais_msg->decodeM21(tempY, M6data.ProcOffPositionStatus);
					agoM21 = millis();
					lastM21 = _oTiming->now.sMasa.c_str();
					_inputMandoVdo21 = true;
					iniMando->_mandoTaskStat = VDO21;
					mandoTask = RMC;

				}
				break;
			case ABK06:
				tempY = ais_msg->getPara(_processRaw, 3);
				if (tempY[1] == '6') {
					//$AIABK,995339991,A,06,0,2*14

					tempY = ais_msg->getPara(_processRaw, 4);

					if (tempY == "0") {
//						_inputMandoVdo06 = true;
						iniMando->_mandoTaskStat = ABK06;
						mandoTask = mt_ada;
					}
				}
				break;
			case SendBIT:
			case mt_cNone:
			case Send06:
			case cAID1:
			case cAID2:
			case cACE:
			case cACF:
			case cAAR:
			case cPAMC:
			case mt_ada:
				break;

			}
		}

	}

	delete ais_msg;

}

void Mando::hantarDataMando() {
	if (millis() - iniMando->_runCtr >= _hantarDelay) {
		if (mandoTask == ACE) {
			Serial2.println("$ECAIQ,ACE*34");
			Serial2.flush();

			log_i("ACE =============== $ECAIQ,ACE*34 :: at %d", millis());

			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
		}
		else if (mandoTask == ACF) {
			Serial2.println("$ECAIQ,ACF*37");
			Serial2.flush();

			log_i("ACF =============== $ECAIQ,ACF*37 :: at %d", millis());

			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
		}
		else if (mandoTask == VER) {
			Serial2.println("$ECAIQ,VER*32");
			Serial2.flush();

			log_i("VER =============== $ECAIQ,VER*32 :: at %d", millis());

			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
		}
		else if (mandoTask == AAR06) {
			Serial2.println("$ECAIQ,AAR*21");
			Serial2.flush();

			log_i("AAR 06 =============== $ECAIQ,AAR*21 :: at %d", millis());

			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
		}
		else if (mandoTask == AAR21) {
			Serial2.println("$ECAIQ,AAR*21");
			Serial2.flush();

			log_i("AAR 21 =============== $ECAIQ,AAR*21 :: at %d", millis());

			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
		}
		else if (mandoTask == SendBIT) {
			log_i(" )()()()()()()()()()()()()()()()(()()()()()()() checkAtonBit() at %d", millis());
			AIS_msg * aisMsg;
			aisMsg = new AIS_msg();
			aisMsg->makeAtonBit(ProcAtonbit_now, AtonBit.alarmX, AtonBit.lantern, AtonBit.racon, AtonBit.page);
			if (ProcAtonbit_prev != ProcAtonbit_now) {
				ProcAtonbit_prev = ProcAtonbit_now;
				aisMsg->ConstructM21(MFullTextAce, ProcAtonbit_prev);
				log_i("MFullTextAce >>> %s", MFullTextAce);
				Serial2.println(MFullTextAce);
				Serial2.flush();
				iniMando->_runCtr = millis();
				_hantarDelay = 10000;
			}

			iniMando->_mandoTaskStat = SendBIT;
			mandoTask = Send06;



			delete aisMsg;
		}
		else if (mandoTask == Send06) {
			log_i(" <<><><><><><><><><><><><><><><><><><><><><><><><><><><><> send Msg6 at %d", millis());

			char* end;
			long hehe = strtol(SpiffsData.Dest_ID.c_str(), &end, 10);

			if (*end) {
				hehe = 701;
			}

			AIS_msg * ais_msg;
			ais_msg = new AIS_msg();

			beating();

			if (_channel == 1) {
				_channel = 2;
				ais_msg->ConstructMsg06(buffer, hehe, 1, iniMando->M6data, AtonBit.alarmX, AtonBit.lantern, AtonBit.racon, AtonBit.page, _oTiming, SpiffsData);
			}else {
				_channel = 1;
				ais_msg->ConstructMsg06(buffer, hehe, 2, iniMando->M6data, AtonBit.alarmX, AtonBit.lantern, AtonBit.racon, AtonBit.page, _oTiming, SpiffsData);
			}


			String g = "";
			for (int i = 0; i < 768; i++) {
				g += buffer[i];
			}
			g += "\r\n\0";
			ais_msg->checkSum(g);
			log_i("MSG 6 hantar ============== %s :: at %d", g.c_str(), millis());
			_abm = g;
			Serial2.println(g);
			Serial2.flush();
			delay(50);

			iniMando->_mandoTaskStat = Send06;
			mandoTask = ABK06;
			iniMando->_runCtr = millis();
			_hantarDelay = 1500;
			_kaliHantarM6++;

			delete ais_msg;
		}
		else if (mandoTask == cAID1) {


			if (MTemporaryMMSI != _mConfig.MMmsi) {
				_confProgress++;
				AIS_msg * ais_msg;
				ais_msg = new AIS_msg();

				_confFull = 6;

				String tempX = "$AIAID,";
				tempX += MTemporaryMMSI;
				tempX += ",0,";
				tempX += MTemporaryMMSI;
				tempX += ",R,C*\r\n";

				String g = "";
				for (int i = 0; i < 768; i++) {
					g += tempX[i];
				}
				ais_msg->checkSum(g);
				Serial2.println(g);
				Serial2.flush();
				delay(50);
				log_i("config AID 1 hantar ============== %s :: at %d", g.c_str(), millis());
				//MMmsi =
				_runCtr = millis();
				_hantarDelay = 3000;
				mandoTask = cAID2;

				delete ais_msg;
			}
			else {
				mandoTask = cACE;
				_hantarDelay = 3000;
				_confFull = 4;
			}

		}
		else if (mandoTask == cAID2) {

			_confProgress++;
			AIS_msg * ais_msg;
			ais_msg = new AIS_msg();

			String tempX = "$AIAID,000000000,1,";
			tempX += _mConfig.MMmsi;
			tempX += ",R,C*\r\n";

			String g = "";
			for (int i = 0; i < 768; i++) {
				g += tempX[i];
			}
			ais_msg->checkSum(g);
			Serial2.println(g);
			Serial2.flush();
			delay(50);
			log_i("config AID 2 hantar ============== %s :: at %d", g.c_str(), millis());
			//MMmsi =
			_runCtr = millis();
			_hantarDelay = 10000;
			mandoTask = cACE;
			_confProgress++;
			delete ais_msg;

		}
		else if (mandoTask == cACE) {

			_confProgress++;


			String ConfOffPosThres = (String)_mConfig.MOffThres;
			if (ConfOffPosThres.length() == 3)       ConfOffPosThres = "0" + ConfOffPosThres;
			else if (ConfOffPosThres.length() == 2)  ConfOffPosThres = "00" + ConfOffPosThres;
			else if (ConfOffPosThres.length() == 1)  ConfOffPosThres = "000" + ConfOffPosThres;
			else if (ConfOffPosThres.length() == 0)  ConfOffPosThres = "0010";
			else if (ConfOffPosThres.length() > 4)   ConfOffPosThres = "1000";

			if (_mConfig.MLengthA.length() == 2)		_mConfig.MLengthA = "0" + _mConfig.MLengthA;
			else if (_mConfig.MLengthA.length() == 1)	_mConfig.MLengthA = "00" + _mConfig.MLengthA;
			else if (_mConfig.MLengthA.length() == 0)	_mConfig.MLengthA = "001";
			else if (_mConfig.MLengthA.length() > 3)	_mConfig.MLengthA = "999";

			if (_mConfig.MLengthB.length() == 2)           _mConfig.MLengthB = "0" + _mConfig.MLengthB;
			else if (_mConfig.MLengthB.length() == 1)      _mConfig.MLengthB = "00" + _mConfig.MLengthB;
			else if (_mConfig.MLengthB.length() == 0)      _mConfig.MLengthB = "001";
			else if (_mConfig.MLengthB.length() > 3)       _mConfig.MLengthB = "999";

			if (_mConfig.MLengthC.length() == 1)           _mConfig.MLengthC = "0" + _mConfig.MLengthC;
			else if (_mConfig.MLengthC.length() == 0)      _mConfig.MLengthC = "01";
			else if (_mConfig.MLengthC.length() > 2)       _mConfig.MLengthC = "99";


			if (_mConfig.MLengthD.length() == 1)           _mConfig.MLengthD = "0" + _mConfig.MLengthD;
			else if (_mConfig.MLengthD.length() == 0)      _mConfig.MLengthD = "01";
			else if (_mConfig.MLengthD.length() > 2)       _mConfig.MLengthD = "99";
			//MName //MMmsi //MAtonType //MLengthA //MLengthB //MLengthC //MLengthD //MOffThres (int) //MEpfs //MPosAcc
			//MTempStrRegLat //MRegCardinalLat //MTempStrRegLng //MRegCardinalLng
			_mConfig.MName.toUpperCase();

			AIS_msg * ais_msg;
			ais_msg = new AIS_msg();


			String tempX = "$AIACE,";
			tempX += _mConfig.MMmsi;
			tempX += ",E0";
			tempX += ",";
			tempX += ConfOffPosThres;
			tempX += ",0,0,0,";
			tempX += _mConfig.MName;
			tempX += ",";
			tempX += _mConfig.MLengthA;
			tempX += _mConfig.MLengthB;
			tempX += _mConfig.MLengthC;
			tempX += _mConfig.MLengthD;
			tempX += ",C*\r\n";

			String g = "";
			for (int i = 0; i < 768; i++) {
				g += tempX[i];
			}

			ais_msg->checkSum(g);
			Serial2.println(g);
			Serial2.flush();
			delay(50);
			log_i("config ACE hantar ============== %s :: at %d", g.c_str(), millis());

			_runCtr = millis();
			_hantarDelay = 10000;
			mandoTask = cACF;
			delete ais_msg;


		}
		else if (mandoTask == cACF) {

			_confProgress++;

			AIS_msg * ais_msg;
			ais_msg = new AIS_msg();

			String temp1 = ais_msg->getPara(MFullTextAcf, 8);
			String temp2 = ais_msg->getPara(MFullTextAcf, 9);
			String temp3 = ais_msg->getPara(MFullTextAcf, 10);
			String temp4 = ais_msg->getPara(MFullTextAcf, 11);
			String temp5 = ais_msg->getPara(MFullTextAcf, 12);
			String temp6 = ais_msg->getPara(MFullTextAcf, 14);

			String tempX = "$AIACF,";
			tempX += _mConfig.MMmsi;
			tempX += ",";
			tempX += _mConfig.MEpfs;
			tempX += ",";
			tempX += ais_msg->gpstoAISlatstandard(_mConfig.MConfigLat);
			tempX += ",";
			tempX += _mConfig.MRegCardinalLat;
			tempX += ",";
			tempX += ais_msg->gpstoAISlngstandard(_mConfig.MConfigLng);
			tempX += ",";
			tempX += _mConfig.MRegCardinalLng;
			tempX += ",";
			tempX += _mConfig.MPosAcc;
			tempX += ",";
			tempX += temp1;
			tempX += ",";
			tempX += temp2;
			tempX += ",";
			tempX += temp3;
			tempX += ",";
			tempX += temp4;
			tempX += ",";
			tempX += temp5;
			tempX += ",";
			tempX += String(_mConfig.MAtonType);
			tempX += ",";
			tempX += temp6;
			tempX += ",C*\r\n";

			String g = "";
			for (int i = 0; i < 768; i++) {
				g += tempX[i];
			}
			ais_msg->checkSum(g);
			Serial2.println(g);
			Serial2.flush();
			delay(50);
			log_i("config ACF hantar ============== %s :: at %d", g.c_str(), millis());

			_runCtr = millis();
			_hantarDelay = 10000;
			mandoTask = cAAR;
			delete ais_msg;


		}
		else if (mandoTask == cAAR) {

			_confProgress++;

			String temp1, temp2, temp3, temp4;

			//AIAAR 06
			//$AIAAR,995339999,06,01,02,01,,180,1,03,04,,180,C*32
			if (_mConfig.M06UTChourA.toInt() < 10)        temp1 = "0" + _mConfig.M06UTChourA;
			else if (_mConfig.M06UTChourA.toInt() > 99)   temp1 = "99";
			else                              temp1 = _mConfig.M06UTChourA;

			if (_mConfig.M06UTCminA.toInt() < 10)         temp2 = "0" + _mConfig.M06UTCminA;
			else if (_mConfig.M06UTCminA.toInt() > 99)    temp2 = "99";
			else                              temp2 = _mConfig.M06UTCminA;

			if (_mConfig.M06UTChourB.toInt() < 10)        temp3 = "0" + _mConfig.M06UTChourB;
			else if (_mConfig.M06UTChourB.toInt() > 99)   temp3 = "99";
			else                              temp3 = _mConfig.M06UTChourB;

			if (_mConfig.M06UTCminB.toInt() < 10)         temp4 = "0" + _mConfig.M06UTCminB;
			else if (_mConfig.M06UTCminB.toInt() > 99)    temp4 = "99";
			else                              temp4 = _mConfig.M06UTCminB;

			unsigned int A6kali60, B6kali60, A21kali60, B21kali60;
			String M6ChA, M6ChB, M21ChA, M21ChB;

			A6kali60 = _mConfig.M06IntervalABahagi60.toInt() * 60;
			if (A6kali60 > 86400) {
				A6kali60 = 86400;
			}
			else if (A6kali60 == 0) {
				A6kali60 = 60;
			}

			B6kali60 = _mConfig.M06IntervalBBahagi60.toInt() * 60;
			if (B6kali60 > 86400) {
				B6kali60 = 86400;
			}
			else if (B6kali60 == 0) {
				B6kali60 = 60;
			}

			AIS_msg * ais_msg;
			ais_msg = new AIS_msg();

			String tempX = "$AIAAR,";
			tempX += _mConfig.MMmsi;
			tempX += ",06,01,";
			tempX += temp1;
			tempX += ",";
			tempX += temp2;
			tempX += ",,";
			tempX += (String)A6kali60;
			tempX += ",1,";
			tempX += temp3;
			tempX += ",";
			tempX += temp4;
			tempX += ",,";
			tempX += (String)B6kali60;
			tempX += ",C*\r\n";

			String g = "";
			for (int i = 0; i < 768; i++) {
				g += tempX[i];
			}
			ais_msg->checkSum(g);
			Serial2.println(g);
			Serial2.flush();
			delay(50);
			log_i("config AAR06 hantar ============== %s :: at %d", g.c_str(), millis());

			//
			//AIAAR 21
			//$AIAAR,995339999,06,01,02,01,,180,1,03,04,,180,C*32
			if (_mConfig.M21UTChourA.toInt() < 10)        temp1 = "0" + _mConfig.M21UTChourA;
			else if (_mConfig.M21UTChourA.toInt() > 99)   temp1 = "99";
			else                              temp1 = _mConfig.M21UTChourA;

			if (_mConfig.M21UTCminA.toInt() < 10)         temp2 = "0" + _mConfig.M21UTCminA;
			else if (_mConfig.M21UTCminA.toInt() > 99)    temp2 = "99";
			else                              temp2 = _mConfig.M21UTCminA;

			if (_mConfig.M21UTChourB.toInt() < 10)        temp3 = "0" + _mConfig.M21UTChourB;
			else if (_mConfig.M21UTChourB.toInt() > 99)   temp3 = "99";
			else                              temp3 = _mConfig.M21UTChourB;

			if (_mConfig.M21UTCminB.toInt() < 10)         temp4 = "0" + _mConfig.M21UTCminB;
			else if (_mConfig.M21UTCminB.toInt() > 99)    temp4 = "99";
			else                              temp4 = _mConfig.M21UTCminB;


			A21kali60 = _mConfig.M21IntervalABahagi60.toInt() * 60;
			if (A21kali60 > 86400) {
				A21kali60 = 86400;
			}
			else if (A21kali60 == 0) {
				A21kali60 = 60;
			}

			B21kali60 = _mConfig.M21IntervalBBahagi60.toInt() * 60;
			if (B21kali60 > 86400) {
				B21kali60 = 86400;
			}
			else if (B21kali60 == 0) {
				B21kali60 = 60;
			}

			//$AIAAR,995339999,21,01,01,02,,180,1,04,05,,360,C*3D
			tempX = "$AIAAR,";
			tempX += _mConfig.MMmsi;
			tempX += ",21,01,";
			tempX += temp1;
			tempX += ",";
			tempX += temp2;
			tempX += ",,";
			tempX += (String)A21kali60;
			tempX += ",1,";
			tempX += temp3;
			tempX += ",";
			tempX += temp4;
			tempX += ",,";
			tempX += (String)B21kali60;
			tempX += ",C*\r\n";

			g = "";
			for (int i = 0; i < 768; i++) {
				g += tempX[i];
			}
			ais_msg->checkSum(g);
			Serial2.println(g);
			Serial2.flush();
			delay(50);
			log_i("config AAR21 hantar ============== %s :: at %d", g.c_str(), millis());

			_runCtr = millis();
			_hantarDelay = 10000;
			mandoTask = cPAMC;

			delete ais_msg;



		}
		else if (mandoTask == cPAMC) {

			_confProgress++;

			Serial2.println("$PDBG,029,0,,,,,,*1A"); //disable dummy VDO
			log_i("$PDBG hantar ============== $PDBG,029,0,,,,,,*1A :: at %d", millis());
			delay(50);
			Serial2.println("$$PDBG,051,5,,,,,,*10"); //set GPS rate to 5sec interval
			log_i("$$PDBG hantar ============== $$PDBG,051,5,,,,,,*10 :: at %d", millis());
			delay(50);
			Serial2.println("$PAMC,C,ATN,20,1,0,*04"); //set interface type lantern monitoring
			log_i("$PAMC hantar ============== $PAMC,C,ATN,20,1,0,*04 :: at %d", millis());

			delay(10000);

			_mandoTaskStat = 0;
			mandoTask = ACE;

			_hantarDelay = 1500;

		}
	}

}

String Mando::getLookUpHeader() {
	switch(iniMando->mandoTask) {
		case ACF:
			return "$ANACF";
		case ACE:
			return "$ANACE";
		case VER:
			return "$ANVER";
		case AAR06:
			return "$ANAAR";
		case AAR21:
			return "$ANAAR";
		case RMC:
			return "$GPRMC";
		case VDO21:
			return "!AIVDO";
		case ABK06:
			return "$AIABK";
		case SendBIT:
			return "$PAMC";
		default:
			return "XX";

	}
}

int Mando::getBit(String& pass, byte from, byte until) {
	char pos[11];
	pass.toCharArray(pos, 11);

	int coor = 0;
	for (int i = from; i <= until; i++) {
		coor += pow(10, (until - i)) * (pos[i] - 48);
	}

	return coor;
}

void Mando::beating() {
	if (!strcmp(SpiffsData.Beat.c_str(), "1")) {
		SpiffsData.Beat = "0";
	}else {
		SpiffsData.Beat = "1";
	}

	log_i("Beat >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s", SpiffsData.Beat.c_str());
}


