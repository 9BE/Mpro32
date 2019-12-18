/*
 * Lantern.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#include "Lantern.h"

TaskHandle_t taskLantern;
Lantern *iniLantern;



Lantern::Lantern(const BaseType_t xCoreID, uint64_t loopDelay, timing * tim, Mando * mando) {
	// TODO Auto-generated constructor stub
	iniLantern = this;
	iniLantern->_loopDelay = loopDelay;
	iniLantern->_oTiming = tim;
	iniLantern->_oMando = mando;
	kalaujumpalanterncepat = millis() + 20000;
	xTaskCreatePinnedToCore(loop, "taskLantern", 4096, NULL, 1, &taskLantern, 0);
}

Lantern::~Lantern() {
	// TODO Auto-generated destructor stub
}

void Lantern::loop(void* param) {
	iniLantern->node.begin(1, Serial1);
	iniLantern->node.preTransmission(iniLantern->preTransmission);
	iniLantern->node.postTransmission(iniLantern->postTransmission);
	esp_task_wdt_add(taskLantern);
	esp_task_wdt_reset();
	while(1) {

		if (iniLantern->_mulai == true) {
			if (iniLantern->_lanternTask == lt_SC35) {
				if (millis() < iniLantern->kalaujumpalanterncepat) {
					iniLantern->alert = "Query SC35";
					if (iniLantern->handleSC35()) {
						iniLantern->_lanternTaskStat = lt_SC35;
						iniLantern->_lanternTask = lt_CheckLanternLost;
					}
				}
				else {
					if (iniLantern->jumpaSC35) {
						iniLantern->kalaujumpalanterncepat = iniLantern->kalaujumpalanterncepat + 10000;
						esp_task_wdt_reset();
						if (millis() - iniLantern->_masaTungguLantern >= 60000) {
							iniLantern->kalaujumpalanterncepat = 0;
							iniLantern->jumpaSC35 = false;
						}

					}else {
						log_i("!!!!!!!!!!!!!!!!!!!!! NOT FOUND SC35");
						iniLantern->jumpaSC35 = false;
						iniLantern->_lanternTaskStat = lt_SC35_nf;
						iniLantern->_lanternTask = lt_LANTERNreader;
						iniLantern->kalaujumpalanterncepat = millis() + 60000;
						iniLantern->_masaTungguLantern = millis();
						iniLantern->alert = "Not found SC35. Query Lantern Reader";
					}


				}
			}
			else if (iniLantern->_lanternTask == lt_LANTERNreader) {
				if (millis() < iniLantern->kalaujumpalanterncepat) {
//					log_i("SINIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII");
					if (iniLantern->handleLR()) {
						iniLantern->_lanternTaskStat = lt_LANTERNreader;
						iniLantern->_lanternTask = lt_CheckLanternLost;
					}

				}
				else {
					if (iniLantern->jumpaLR) {
						iniLantern->kalaujumpalanterncepat = iniLantern->kalaujumpalanterncepat + 10000;
						esp_task_wdt_reset();
						if (millis() - iniLantern->_masaTungguLantern >= 160000) {
							iniLantern->kalaujumpalanterncepat = 0;
							iniLantern->jumpaLR = false;
						}
					}else {
						log_i("!!!!!!!!!!!!!!!!!!!!! NOT FOUND LR");



						if (iniLantern->lanternlockB & !iniLantern->lanternlockC) { //Buoy dapat. Lighthouse tak dapat
							iniLantern->alert = "Not found Lighthouse System";
							iniLantern->LanternLongStatus = "Not found Lighthouse System";
							iniLantern->_oMando->M6data.LVin = 0;
							if (iniLantern->_oMando->SpiffsData.RACON_Mon != "No") {
								iniLantern->_oMando->M6data.RVin = 0;
							}

							iniLantern->_oMando->M6data.LNyala = iniLantern->_oMando->M6data.ELStat;
//							iniLantern->normalize();
//							iniLantern->_oMando->M6data.ELStat = iniLantern->_oMando->M6data.LNyala;


						}
						else if (!iniLantern->lanternlockB & iniLantern->lanternlockC) { //Buoy takdapat. Lighthouse dapat
							iniLantern->alert = "Not found Emergency System";
							iniLantern->LanternLongStatus = "Not found Emergency System";
							iniLantern->_oMando->M6data.ELCond = 0;
							iniLantern->_oMando->M6data.ELStat = 0;
							if (iniLantern->_oMando->SpiffsData.RACON_Mon == "No") {
								iniLantern->_oMando->M6data.RVin = 0;
							}
							iniLantern->findEmergency = true;
							//ha kat sini boleh kata not found lantern emergency
						}
						else {
							iniLantern->findEmergency = true;
							iniLantern->alert = "Not found Lantern";
							iniLantern->jumpaLR = false;
							iniLantern->lanternRespond = 0;
							iniLantern->_oMando->M6data.LVin = 0;
							iniLantern->_oMando->M6data.LDRStatus = 0;
							iniLantern->_oMando->M6data.LNyala = 0;
							iniLantern->_oMando->M6data.SLNyala = 0;
							iniLantern->_oMando->M6data.ELCond = 1;
							iniLantern->_oMando->M6data.ELStat = 0;
							iniLantern->_oMando->M6data.RVin = 0;
						}


						iniLantern->_lanternTaskStat = lt_LANTERNreader_nf;
						iniLantern->_lanternTask = lt_CheckLanternLost;

					}
				}
			}
			else if (iniLantern->_lanternTask == lt_CheckLanternLost) {
				iniLantern->checkLanternLost();
			}
			else if (iniLantern->_lanternTask == lt_Decision) {
				if (iniLantern->lanternRespond == 4) {
					iniLantern->normalize();
				}

				iniLantern->makeDecision();

				delay(5000);

				if (iniLantern->_mulai) {
					iniLantern->reInit();
				}
			}
		}

		esp_task_wdt_reset();
		delay(iniLantern->_loopDelay);
	}

}

void Lantern::normalize() {
	if (_lrNyala == 1) {
		if (_nyalaNormal < 5) {
			_nyalaNormal++;
		}
	}else if (_lrNyala == 0) {
		if (_nyalaNormal > 0) {
			_nyalaNormal--;
		}
	}

	if (_nyalaNormal == 5) {
		_prevNyala = 1;
		_nyalaNormal = 3;
	}else if (_nyalaNormal == 0) {
		_prevNyala = 0;
		_nyalaNormal = 3;
	}

	_oMando->M6data.LNyala = _prevNyala;
}

bool Lantern::handleLR() {

	bool res = false;

	if (lanternlockB && lanternlockC) {
		lanternlock = true;
		LanternLongStatus = "Data locked.";
		_oMando->M6data.ELStat = _lrNyala;
	}
	if (lanternlock) {
		res = true;
		if (_oMando->SpiffsData.Light_Detect_Method == "AI") {

			String _8bit;
			for (int x = 0; x < 2; x ++) {
				byte c = _oMando->ProcAtonbit_prev[x];
				if (c >= 65) {
					int a = c - 55;
					String t = String(a, BIN);
					for (int y = t.length(); y < 4; y++) {
						t = "0" + t;
					}
					_8bit += t;
				}
				else {
					int b = c - 48;
					String t = String(b, BIN);
					for (int y = t.length(); y < 4; y++) {
						t = "0" + t;
					}
					_8bit += t;
				}
			}

			lightStatus = String(_8bit[5]) + String(_8bit[6]);
			if (_oMando->SpiffsData.Format == "GF-LR-BUOY") {
				if (_oTiming->ZoneTime == e_nite) { //Night
					float Thres1 = Primthresholdamp - (Primthresholdamp * 0.5);
					if (PrimMaxMinDiff >= Thres1) {
						DoNotChange = 0;
						_oMando->M6data.LNyala = 1;
					}

					else {
						CounterN += 1;
						if (CounterN > 2) {
							_oMando->M6data.LNyala = 0;
							CounterN = 0;
						}
						else
							DoNotChange = 1;
					}
				}
				else {
					float Thres2 = Primthresholdamp + (Primthresholdamp * 0.3);
					float Thres3 = Primthresholdamp + (Primthresholdamp * 0.5);
					if (PrimMaxMinDiff > Thres3) {
						ThresApp = Thres3;
						Weight += 3;
						CounterD += 1;
					}
					else if (PrimMaxMinDiff > Thres2) {
						ThresApp = Thres2;
						Weight += 2;
						CounterD += 1;
					}
					else if (PrimMaxMinDiff > Primthresholdamp) {
						ThresApp = Primthresholdamp;
						Weight += 1;
						CounterD += 1;
					}
					else {
						Weight = 0;
						DoNotChange = 0;
						_oMando->M6data.LNyala = 0;
						CounterD = 0;
					}
					//k tamat dah bahagian comparison Diff/Thres

					if (Weight != 0) {
						if (Weight >= 4) {
							_oMando->M6data.LNyala = 1;
							CounterD = 0;
							Weight = 0;
							DoNotChange = 0;
						}
						else {
							if (CounterD <= 1) {
								DoNotChange = 1;
							}
							else {
								_oMando->M6data.LNyala = 0;
								CounterD = 0;
							}
						}
					}
				}

				if (DoNotChange) {
					if (lightStatus == "00")         _oMando->M6data.LNyala = 0;
					else if (lightStatus == "01")    _oMando->M6data.LNyala = 1;
					else if (lightStatus == "10")    _oMando->M6data.LNyala = 0;
					else if (lightStatus == "11")    _oMando->M6data.LNyala = 0;
				}
			}

		}
	}
	return res;
}

void Lantern::reInit() {
	lanternlock = false;
	lanternlockB = false;
	lanternlockC = false;
	askLantern1 = false;
	askLantern2 = false;
	askLantern3 = false;
	_lanternTaskStat = lt_NONE;
	_lanternTask = lt_SC35;
	kalaujumpalanterncepat = millis() + 20000;
	iniLantern->_masaTungguLantern = millis();
}

bool Lantern::handleSC35() {
	bool res = false;
	if (!iniLantern->askLantern3){
		iniLantern->lanternRead();
	}
	else {
		iniLantern->lanternRespond = 2;
		bool ada = false;
		if (strcmp(iniLantern->_oMando->SpiffsData.Format.c_str(), "GF-SC35")) {
			iniLantern->_oMando->SpiffsData.Format = "GF-SC35";
			ada = true;
		}

		if (!strcmp(iniLantern->_oMando->SpiffsData.DAC.c_str(), "533")) {
			if (strcmp(iniLantern->_oMando->SpiffsData.FI.c_str(), "4")) {
				iniLantern->_oMando->SpiffsData.FI = "4";
				ada = true;
			}
		}

		if (ada) {
			JsonHandler *jsonHandler;
			jsonHandler = new JsonHandler(iniLantern->_oMando);
			if (jsonHandler->simpanConfData()) {
				delete jsonHandler;
			}
		}
		res = true;
	}

	return res;
}



void Lantern::makeDecision() {
	// part SUHAIMI

	// lantern : 1=ON, 2=OFF, 3=ERROR
	// aton bit : E2=L.ON, E4=L.OFF, E7=L.ERROR
	// LDR : 1=Dark, 2=Dim, 3=Bright
	if (!_oMando->M6data.LNyala || !lanternRespond) {      // jika lantern taknyala (due to some reason), or lantern not respond within given time
		if (lanternlock) {
			if (!strcmp(_oMando->SpiffsData.Use_LDR.c_str(), "Yes")) { //kalau dia menggunakan LR drpd Lighthouse dan Beacon, kene auto Use LDR = Yes.
				if (_oMando->M6data.LDRStatus == 1) {
					if (_oMando->M6data.SLNyala) {
						_oMando->AtonBit.lantern = 1;
						_oMando->AtonBit.alarmX = 0;
						_debugLDR = 11;
					}
					else {
						_oMando->AtonBit.lantern = 3;
						_oMando->AtonBit.alarmX = 1;
						_debugLDR = 12;
					}
				}
				else {
					if (_oMando->M6data.SLNyala || _oMando->M6data.SLStat) {
						_oMando->AtonBit.lantern = 1;
						_oMando->AtonBit.alarmX = 0;
						_debugLDR = 21;
					}
					else {
						_debugLDR = 22;
						_oMando->AtonBit.lantern = 2;
						_oMando->AtonBit.alarmX = 0;
					}
				}
			}
			else {
				_oMando->M6data.LDRStatus = 0;
				if (_oTiming->ZoneTime == e_nite) {
					_oMando->AtonBit.lantern = 3;
					_oMando->AtonBit.alarmX = 1;
				}
				else {
					_oMando->AtonBit.lantern = 2;
					_oMando->AtonBit.alarmX = 0;
				}
			}
		}
		else if (_oTiming->ZoneTime == e_nite) {  // malam
			_oMando->AtonBit.lantern = 3;
			_oMando->AtonBit.alarmX = 1;
		}
		else {
			_oMando->AtonBit.lantern = 2;              // siang
			_oMando->AtonBit.alarmX = 0;
		}
	}
	else {
		_oMando->AtonBit.lantern = 1;
		_oMando->AtonBit.alarmX = 0;
	}

	//tempat update RACON START
	// AtonBit.racon : 0=No, 2=Operational, 3=Error
	if (_oMando->SpiffsData.RACON_Mon == "No") {
		_oMando->AtonBit.racon = 0;
	}
	else if (_oMando->SpiffsData.RACON_Mon == "10Vin") {
		if (_oMando->M6data.RVin >= 10)       _oMando->AtonBit.racon = 2;
		else                  _oMando->AtonBit.racon = 3;
	}
	else if (_oMando->SpiffsData.RACON_Mon == "Contact ON") {
		if (FreeContact == 1) _oMando->AtonBit.racon = 2;
		else                  _oMando->AtonBit.racon = 3;
	}
	else if (_oMando->SpiffsData.RACON_Mon == "Contact Fail") {
		if (FreeContact == 0) _oMando->AtonBit.racon = 2;
		else                  _oMando->AtonBit.racon = 3;
	}


	if (_oMando->SpiffsData.Format == "GF-LR-LIGHTHOUSE") {

//		if (_oMando->M6data.LDRStatus == 1) {
//			_oMando->M6data.ELCond = !_oMando->M6data.ELStat; //jika lampu off, error. jika lampu on, no error
//		}
//		else {
//			_oMando->M6data.ELCond = 0;
//		}

		if (_oMando->M6data.LVin == 0) { //Lighthouse mati
			_oMando->M6data.MLCond = 1;
			_oMando->M6data.MLStat = 0;
			_oMando->M6data.SLCond = 1;
			_oMando->M6data.SLStat = 0;
			_oMando->M6data.D1Stat = 0;
			_oMando->M6data.D1Cond = 1;
			_oMando->M6data.D2Stat = 0;
			_oMando->M6data.D2Cond = 1;
			_oMando->M6data.Door = 0;
			_oMando->M6data.ACPower = 0;
			_oMando->M6data.BMS = 1;
			if (_oTiming->ZoneTime == e_nite) {
				if (lanternlockB) {
//					_oMando->M6data.LNyala = _oMando->M6data.ELStat;
					if (_oMando->M6data.ELStat) {
						_oMando->M6data.ELCond = 0;
						_oMando->AtonBit.alarmX = 0;
						_oMando->AtonBit.lantern = 1;
					}
					else {
						_oMando->M6data.ELCond = 1;
						_oMando->AtonBit.alarmX = 1;
						_oMando->AtonBit.lantern = 3;
					}
				}
				else {
					_oMando->M6data.ELCond = 1;
					_oMando->M6data.ELStat = 0;
					_oMando->AtonBit.alarmX = 1;
					_oMando->AtonBit.lantern = 3;
				}
			}
			else {
				_oMando->AtonBit.alarmX = 0;
				if (lanternlockB) {
//					_oMando->M6data.LNyala = _oMando->M6data.ELStat;
					if (_oMando->M6data.ELStat) {
						_oMando->AtonBit.lantern = 1;
						_oMando->M6data.ELCond = 0;
					}
					else {
						_oMando->AtonBit.lantern = 2;
						_oMando->M6data.ELCond = 0;
					}
				}
				else {
					_oMando->M6data.ELCond = 0;
					_oMando->M6data.ELStat = 0;
					_oMando->AtonBit.lantern = 2;
					_oMando->AtonBit.alarmX = 0;
				}
			}
		}
		else { //status lighthouse found
			if (_oMando->M6data.MLStat == 1 || _oMando->M6data.SLStat == 1) {
				_oMando->AtonBit.alarmX = 0;
				_oMando->AtonBit.lantern = 1;
				_oMando->M6data.ELCond = 0;
			}
			else {
				if (_oMando->M6data.LDRStatus == 1) {
					if (lanternlockB) {
//						_oMando->M6data.LNyala = _oMando->M6data.ELStat;
						if (_oMando->M6data.ELStat) {
							_oMando->M6data.ELCond = 0;
							_oMando->AtonBit.alarmX = 0;
							_oMando->AtonBit.lantern = 1;
						}
						else {
							_oMando->M6data.ELCond = 1;
							_oMando->AtonBit.alarmX = 1;
							_oMando->AtonBit.lantern = 3;
						}
					}
					else {
						_oMando->M6data.ELCond = 1;
						_oMando->AtonBit.alarmX = 1;
						_oMando->AtonBit.lantern = 3;
					}
				}
				else {
					_oMando->AtonBit.alarmX = 0;
					_oMando->M6data.ELCond = 0;
					if (lanternlockB) {
//						_oMando->M6data.LNyala = _oMando->M6data.ELStat;
						if (_oMando->M6data.ELStat)
							_oMando->AtonBit.lantern = 1;
						else
							_oMando->AtonBit.lantern = 2;
					}
					else
						_oMando->AtonBit.lantern = 2;
				}
			}
		}
	}
	//tempat update RACON END
	_lanternTaskStat = lt_Decision;
	_lanternTask = lt_NONE;


}


void Lantern::preTransmission() {
	digitalWrite(OUT_DR, HIGH);
}

void Lantern::postTransmission() {
	digitalWrite(OUT_DR, LOW);
}

void Lantern::lanternRead() {
	uint8_t result;
	uint16_t data[6];

	int temp0;

	if (millis() - _timeCtr >= 5000) {
		log_i("????????????????????????????????????????????????????? ask lantern :: %d", millis());
		if (!askLantern1) {                      //first 2 (16bit) data
			_timeCtr = millis();
			result = node.readInputRegisters(6, 2);
			if (result == node.ku8MBSuccess) {
				jumpaSC35 = true;
				_oMando->M6data.LPhotoCurrentValue = node.getResponseBuffer(0);
				_oMando->M6data.LVin = node.getResponseBuffer(1) / 10;

				if (_oMando->M6data.LPhotoCurrentValue < 400) {
					_oMando->M6data.LDRStatus = 1;
				}
				else if (_oMando->M6data.LPhotoCurrentValue < 800) {
					_oMando->M6data.LDRStatus = 2;
				}
				else {
					_oMando->M6data.LDRStatus = 3;
				}
				log_i("LVin: %f",_oMando->M6data. LVin);
				log_i("LDRStatus: %d", _oMando->M6data.LDRStatus);

				askLantern1 = true;
			}
			else if (result == node.ku8MBResponseTimedOut) {
				log_i("askLantern1 ku8MBResponseTimedOut :: at %d", millis());
			}
			log_i("result askLantern1 ===== %x", result);
		}
		else if (!askLantern2) {
			_timeCtr = millis();
			result = node.readInputRegisters(9, 3);
			if (result == node.ku8MBSuccess) {
				jumpaSC35 = true;
				temp0 = node.getResponseBuffer(0);
				_oMando->M6data.LNyala = temp0 & 0xff;

				temp0 = node.getResponseBuffer(2);
				_oMando->M6data.LLedTemp = temp0 & 0xff;

				askLantern2 = true;

				log_i("LLedTemp: %d", _oMando->M6data.LLedTemp);
				log_i("LNyala: %d", _oMando->M6data.LNyala);

			}
			else if (result == node.ku8MBResponseTimedOut) {
				log_i("askLantern2 ku8MBResponseTimedOut :: at %d", millis());
			}
			log_i("result askLantern2 ===== %x", result);
		}


		else if (!askLantern3) {
			_timeCtr = millis();
			result = node.readInputRegisters(17, 2);
			if (result == node.ku8MBSuccess) {
				jumpaSC35 = true;
				temp0 = node.getResponseBuffer(0);
				_oMando->M6data.LAlarmActive = bitRead(temp0, 0);
				_oMando->M6data.LOffLedPowThres = bitRead(temp0, 1);
				_oMando->M6data.LOffLowVin = bitRead(temp0, 2);
				_oMando->M6data.LOffLDR = bitRead(temp0, 3);
				_oMando->M6data.LOffTemp = bitRead(temp0, 5);
				_oMando->M6data.LOffForce = bitRead(temp0, 7);

				_oMando->M6data.LErrLedShort = bitRead(temp0, 8);
				_oMando->M6data.LErrLedOpen = bitRead(temp0, 9);
				_oMando->M6data.LErrLedVLow = bitRead(temp0, 10);
				_oMando->M6data.LErrVinLow = bitRead(temp0, 11);
				_oMando->M6data.LErrLedPowThres = bitRead(temp0, 12);
				_oMando->M6data.LLEDAdjMaxAvgPow = bitRead(temp0, 13);
				_oMando->M6data.LGsenIntOccur = bitRead(temp0, 14);
				_oMando->M6data.LSolarChargingOn = bitRead(temp0, 15);

				temp0 = node.getResponseBuffer(1);
				_oMando->M6data.LIsNight = bitRead(temp0, 2);
				if (_oMando->M6data.LIsNight == 0)  _oTiming->ZoneTime = e_day;
				else                _oTiming->ZoneTime = e_nite;

				log_i("LIsNight: %d", _oMando->M6data.LIsNight);

				askLantern3 = true;
			}
			else if (result == node.ku8MBResponseTimedOut) {
				log_i("askLantern3 ku8MBResponseTimedOut :: at %d", millis());

			}
			log_i("result askLantern3 ===== %x", result);
		}
	}




}

void Lantern::checkLanternLost() {
	if (_oMando->M6data.LVin == 0) {
		if (_oMando->SpiffsData.Lantern_Lost != "1") {
			_oMando->SpiffsData.Lantern_Lost = "1";
		}
	}
	else {
		if (_oMando->SpiffsData.Lantern_Lost != "0") {
			_oMando->SpiffsData.Lantern_Lost = "0";
		}

	}

	iniLantern->_lanternTaskStat = lt_CheckLanternLost;
	iniLantern->_lanternTask = lt_Decision;
}


