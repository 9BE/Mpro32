/*
 * DataHandler.cpp
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#include <DataHandler.h>

DataHandler::DataHandler() {

}

void DataHandler::LRactivate(uint8_t index) {
	this->LRactiv |= (uint8_t) 1 << index;
}

bool DataHandler::isLRactive(uint8_t index) {
//	return this->LRactiv & (1 << index);
	return (this->LRactiv >> index) & 0x01;
}

void DataHandler::savToSpiffs() {
}

void DataHandler::procesAtonBit() {
	// TODO : decide lantern data dari local / aso
	if (!this->Msg6Data.LNyala) {
		if (this->Msg6Data.LDRStatus == ldr_dark) {
			if (this->Msg6Data.SLNyala) {
				this->AtonBitData.lantern = 1;
				this->AtonBitData.alarmX = 0;
			}
			else {
				this->AtonBitData.lantern = 3;
				this->AtonBitData.alarmX = 1;
			}
		}
		else {
			if (this->Msg6Data.SLNyala) {
				this->AtonBitData.lantern = 1;
				this->AtonBitData.alarmX = 0;
			}
			else {
				this->AtonBitData.lantern = 2;
				this->AtonBitData.alarmX = 0;
			}
		}
	}
	else {
		this->AtonBitData.lantern = 1;
		this->AtonBitData.alarmX = 0;
	}


	// TODO : Update RACON MULA

	// AtonBit.racon : 0=No, 2=Operational, 3=Error
	if (this->SpiffsData.RACON_Mon == "No") {
		this->AtonBitData.racon = rac_noRac;
	}
	else if (this->SpiffsData.RACON_Mon == "10Vin") {
		if (this->Msg6Data.RVin >= 10)       this->AtonBitData.racon = rac_operational;
		else                  this->AtonBitData.racon = rac_error;
	}
	else if (this->SpiffsData.RACON_Mon == "Contact ON") {
		if (this->Msg6Data.Racon == rac_contact) this->AtonBitData.racon = rac_operational;
		else                  this->AtonBitData.racon = rac_error;
	}
	else if (this->SpiffsData.RACON_Mon == "Contact Fail") {
		if (this->Msg6Data.Racon == rac_xContact) this->AtonBitData.racon = rac_operational;
		else                  this->AtonBitData.racon = rac_error;
	}

	// Update RACON Habis

	byte semua;
	byte out[2];
	semua = this->AtonBitData.alarmX;
	semua += this->AtonBitData.lantern << 1;
	semua += this->AtonBitData.racon << 3;
	semua += this->AtonBitData.page << 5;

	out[0] = semua >> 4;
	out[1] = semua << 4;
	out[1] = out[1] >> 4;

	char baru;

	baru = out[0];
	baru = baru << 4;
	baru += out[1];

	this->procAtonBit_now = String(baru, HEX);
	this->procAtonBit_now.toUpperCase();
//	log_i("procAtonBit_now = %s", this->procAtonBit_now.c_str());
}

bool DataHandler::AtonBitLainTak() {
	log_i("procAtonBit_prev = %s", this->procAtonBit_prev.c_str());
	log_i("procAtonBit_now = %s", this->procAtonBit_now.c_str());
	bool res = this->procAtonBit_prev != this->procAtonBit_now;
	this->procAtonBit_prev = procAtonBit_now;

	return res;
}

String& DataHandler::nakMsg21(Rnmea* manNMEA) {
	this->fullACE.setCharAt(2, 'I');
	for (int i = 0; i < 2; i++) {
		this->fullACE.setCharAt(17 + i, this->procAtonBit_prev[i]);
	}
	int bintang = this->fullACE.indexOf('*');
	this->fullACE.setCharAt(bintang - 1, 'C');
	this->fullACE = this->fullACE.substring(0, bintang);

	manNMEA->appendCheckSum(this->fullACE);

	return this->fullACE;
}

String DataHandler::nakMsg6(Rnmea* manNMEA, long dest, int channel,
		uint8_t msgSeq) {

	this->nmeaData.clear();

	int temp;

	manNMEA->addData(this->nmeaData, this->SpiffsData.DAC.toInt(), 10);
	manNMEA->addData(this->nmeaData, this->SpiffsData.FI.toInt(), 6);

	uint8_t msgFormat = this->SpiffsData.Format;

	if (msgFormat == f_lrBuoy || msgFormat == f_lrBeacon) {
		temp = this->Msg6Data.MVin * 20;
		manNMEA->addData(this->nmeaData, temp, 10);

		temp = this->Msg6Data.LVin * 20;
		manNMEA->addData(this->nmeaData, temp, 10);

		temp = this->Msg6Data.RVin * 20;
		manNMEA->addData(this->nmeaData, temp, 10);

		manNMEA->addData(this->nmeaData, this->AtonBitData.racon, 2);				// RACON
		manNMEA->addData(this->nmeaData, this->AtonBitData.lantern, 2);				// LIGHT
		manNMEA->addData(this->nmeaData, this->AtonBitData.alarmX, 1);				// ALARM

		manNMEA->addData(this->nmeaData, this->SpiffsData.Beat, 1);
		log_i("Beat = %d", this->SpiffsData.Beat);

		if ((10 <= this->Msg6Data.LVin && this->Msg6Data.LVin < 11) || (17 <= this->Msg6Data.LVin && this->Msg6Data.LVin < 22))       temp = 1; //bad
		else if ((11 <= this->Msg6Data.LVin && this->Msg6Data.LVin < 12) || (22 <= this->Msg6Data.LVin && this->Msg6Data.LVin < 24))  temp = 2; //low
		else if ((12 <=this->Msg6Data.LVin && this->Msg6Data.LVin < 16) || (24 <= this->Msg6Data.LVin && this->Msg6Data.LVin < 29))  temp = 3; //good
		else                                                  																	temp = 0; //unknown

		manNMEA->addData(this->nmeaData, temp, 2);
		int lamp = 0; 			// malam tp secondary x nyala

		if (this->Msg6Data.LNyala) {
			lamp = 1;
		}
		else {
			if (this->Msg6Data.SLNyala) {
				lamp = 2;
			}else {
				if (this->Msg6Data.LDRStatus == ldr_dark || (this->nTiming->now.tahun > 2000 && this->nTiming->ZoneTime == e_nite)) {
					lamp = 3;
				}
			}
	//		if (this->nTiming->ZoneTime == e_nite) {
	//			lamp = 3;
	//			if (this->m6data.LDRStatus == ldr_dark) {
	//				if (!this->confData.LANT_Mon.equals("Local")) {
	//					if (this->m6data.SLNyala) {
	//						lamp = 2;
	//					}
	//				}
	//
	//			}
	//		}
	//		else {
	//			lamp = 0;
	//			if (this->m6data.LDRStatus == ldr_dark) {
	//				if (!this->confData.LANT_Mon.equals("Local")) {
	//					if (this->m6data.SLNyala) {
	//						lamp = 2;
	//					}
	//				}
	//				else {
	//					lamp = 3;
	//				}
	//			}
	//		}
	//		if (this->m6data.LDRStatus == ldr_dark) {
	//			if (this->m6data.SLNyala) {
	//				lamp = 2;
	//			}else {
	//				lamp = 3;
	//			}
	//		}
	//		else {
	//			lamp = 0;
	//		}
		}

		manNMEA->addData(this->nmeaData, lamp, 2);
		manNMEA->addData(this->nmeaData, this->Msg6Data.LDRStatus, 2);
		manNMEA->addData(this->nmeaData, this->Msg6Data.Door, 1);
		manNMEA->addData(this->nmeaData, this->Msg6Data.ProcOffPositionStatus, 1);
		manNMEA->addData(this->nmeaData, 0, 4);






	}
	else if (msgFormat == f_lrLH) {
		temp = this->Msg6Data.MVin * 10;
		manNMEA->addData(this->nmeaData, temp, 9);									// Aton V 	- ADC1

		temp = this->Msg6Data.LVin * 10;
		manNMEA->addData(this->nmeaData, temp, 9);									// Lantern V - ADC2

		temp = 0 * 10;
		manNMEA->addData(this->nmeaData, temp, 9);									// Racon V	- ADC3

		manNMEA->addData(this->nmeaData, this->Msg6Data.ProcOffPositionStatus, 1);	// OffPosition

		manNMEA->addData(this->nmeaData, this->Msg6Data.LDRStatus, 2);				// LDR status

		manNMEA->addData(this->nmeaData, this->AtonBitData.racon, 2);				// RACON
		manNMEA->addData(this->nmeaData, this->AtonBitData.lantern, 2);				// LIGHT
		manNMEA->addData(this->nmeaData, this->AtonBitData.alarmX, 1);				// ALARM

		manNMEA->addData(this->nmeaData, this->SpiffsData.Beat, 1);					// Beat

		manNMEA->addData(this->nmeaData, this->Msg6Data.MLCond, 1);					// MLCond
		manNMEA->addData(this->nmeaData, this->Msg6Data.MLStat, 1);					// MLStat
		manNMEA->addData(this->nmeaData, this->Msg6Data.SLCond, 1);					// SLCond
		manNMEA->addData(this->nmeaData, this->Msg6Data.SLStat, 1);					// SLStat
		manNMEA->addData(this->nmeaData, this->Msg6Data.ELCond, 1);					// ELCond
		manNMEA->addData(this->nmeaData, this->Msg6Data.ELStat, 1);					// ELStat
		manNMEA->addData(this->nmeaData, this->Msg6Data.D1Stat, 1);					// D1Stat
		manNMEA->addData(this->nmeaData, this->Msg6Data.D1Cond, 1);					// D1Cond
		manNMEA->addData(this->nmeaData, this->Msg6Data.D2Stat, 1);					// D2Stat
		manNMEA->addData(this->nmeaData, this->Msg6Data.D2Cond, 1);					// D2Cond
		manNMEA->addData(this->nmeaData, this->Msg6Data.Door, 1);					// Door
		manNMEA->addData(this->nmeaData, this->Msg6Data.ACPower, 1);				// ACPower
		manNMEA->addData(this->nmeaData, this->Msg6Data.BMS, 1);					// BMS


	}
	else if (msgFormat == f_gf_sc35) {

		temp = this->Msg6Data.MVin * 20;
		manNMEA->addData(this->nmeaData, temp, 9);									// Aton V

		SC35data_t sc35DataA = this->SC35LiveData;
		if ((millis()/1000) - this->SC35LiveData.masaUpdate >= 600) {
			sc35DataA = this->SC35DefData;
		}

		temp = sc35DataA.Vin * 20;
		manNMEA->addData(this->nmeaData, temp, 9);									// Lantern V

		temp = sc35DataA.LedTemp *5;
		manNMEA->addData(this->nmeaData, temp, 9);									// LED Temp


		manNMEA->addData(this->nmeaData, this->Msg6Data.ProcOffPositionStatus, 1);	// OffPosition
		manNMEA->addData(this->nmeaData, sc35DataA.LDRStatus, 2);				// LDR status

		manNMEA->addData(this->nmeaData, this->AtonBitData.racon, 2);				// RACON
		manNMEA->addData(this->nmeaData, this->AtonBitData.lantern, 2);				// LIGHT
		manNMEA->addData(this->nmeaData, this->AtonBitData.alarmX, 1);				// ALARM

		manNMEA->addData(this->nmeaData, this->SpiffsData.Beat, 1);					// Beat

		manNMEA->addData(this->nmeaData, sc35DataA.AlarmActive, 1);					// AlarmActive
		manNMEA->addData(this->nmeaData, sc35DataA.OffLedPowThres, 1);				// OffLedPowThres
		manNMEA->addData(this->nmeaData, sc35DataA.OffLowVin, 1);					// OffLowVin
		manNMEA->addData(this->nmeaData, sc35DataA.OffLDR, 1);						// OffLDR
		manNMEA->addData(this->nmeaData, sc35DataA.OffTemp, 1);						// OffTemp
		manNMEA->addData(this->nmeaData, sc35DataA.OffForce, 1);					// OffForce
		manNMEA->addData(this->nmeaData, sc35DataA.IsNight, 1);						// IsNight
		manNMEA->addData(this->nmeaData, sc35DataA.ErrLedShort, 1);					// ErrLedShort
		manNMEA->addData(this->nmeaData, sc35DataA.ErrLedOpen, 1);					// ErrLedOpen
		manNMEA->addData(this->nmeaData, sc35DataA.ErrLedVLow, 1);					// ErrLedVLow
		manNMEA->addData(this->nmeaData, sc35DataA.ErrVinLow, 1);					// ErrVinLow
		manNMEA->addData(this->nmeaData, sc35DataA.ErrLedPowThres, 1);				// ErrLedPowThres
		manNMEA->addData(this->nmeaData, sc35DataA.LEDAdjMaxAvgPow, 1);				// LEDAdjMaxAvgPow
		manNMEA->addData(this->nmeaData, sc35DataA.GsenIntOccur, 1);				// GsenIntOccur
		manNMEA->addData(this->nmeaData, sc35DataA.SolarChargingOn, 1);				// SolarChargingOn

	}
	else if (msgFormat == f_a126_sc35) {

		temp = this->Msg6Data.MVin * 20;
		manNMEA->addData(this->nmeaData, temp, 10);									// Aton V 	- ADC1

		SC35data_t sc35DataA = this->SC35LiveData;
		if ((millis()/1000) - this->SC35LiveData.masaUpdate >= 600) {
			sc35DataA = this->SC35DefData;
		}

		temp = sc35DataA.Vin * 20;
		manNMEA->addData(this->nmeaData, temp, 10);									// Lantern V - ADC2

		temp = 0 * 10;
		manNMEA->addData(this->nmeaData, temp, 10);									// Racon V	- ADC3

		manNMEA->addData(this->nmeaData, this->AtonBitData.racon, 2);				// RACON
		manNMEA->addData(this->nmeaData, this->AtonBitData.lantern, 2);				// LIGHT
		manNMEA->addData(this->nmeaData, this->AtonBitData.alarmX, 1);				// ALARM

		manNMEA->addData(this->nmeaData, this->SpiffsData.Beat, 1);					// Beat

		manNMEA->addData(this->nmeaData, sc35DataA.AlarmActive, 1);					// AlarmActive
		manNMEA->addData(this->nmeaData, sc35DataA.OffLowVin, 1);					// OffLowVin
		manNMEA->addData(this->nmeaData, sc35DataA.OffLDR, 1);						// OffLDR
		manNMEA->addData(this->nmeaData, sc35DataA.OffTemp, 1);						// OffTemp
		manNMEA->addData(this->nmeaData, sc35DataA.OffForce, 1);					// OffForce
		manNMEA->addData(this->nmeaData, sc35DataA.GsenIntOccur, 1);				// GsenIntOccur
		manNMEA->addData(this->nmeaData, sc35DataA.SolarChargingOn, 1);				// SolarChargingOn

		manNMEA->addData(this->nmeaData, this->Msg6Data.ProcOffPositionStatus, 1);	// OffPosition
		manNMEA->addData(this->nmeaData, 0, 4);

	}




	char hh[12];
	String res = "!AIABM,1,1,";
	res += msgSeq;
	res += ",";

	ltoa(dest, hh, 10);
	res += String(hh);
	res += ",";
	ltoa(channel, hh, 10);
	res += String(hh);
	res += ",6,";
	String test = manNMEA->compacte(this->nmeaData);
	res += test;

	log_i("test = %s", test.c_str());

	manNMEA->appendCheckSum(res);

	return res;



}
