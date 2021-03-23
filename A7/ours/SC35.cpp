/*
 * SC35.cpp
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#include <SC35.h>



SC35::SC35(DataHandler * data, HardwareSerial * serial) {
	this->_oData = data;
	this->_serial = serial;
	this->_node = new ModbusMaster();

	xTaskCreatePinnedToCore(this->loop, "SC35task", 2048, this, 1, &this->_handle, 1);
}

void SC35::loop(void * param) {
	SC35 * ini = (SC35*) param;
	ini->_node->begin(1, *ini->_serial);
	ini->_node->preTransmission(ini->preTransmission);
	ini->_node->postTransmission(ini->postTransmission);

	N_ticker * tick = new N_ticker(5000, 0);
	while (1) {
		if (tick->Update()) {
			ini->askSC35();
			tick->Reset();
		}
		delay(10);
	}
}

void SC35::askSC35() {
	int result;
	int temp0;

	if (this->_task == ask1) {
		result = this->_node->readInputRegisters(6, 2);
		if (result == this->_node->ku8MBSuccess) {
			this->_oData->LRactivate(f_sc35ada);
			this->_oData->SC35LiveData.masaUpdate = millis()/1000;

			this->_oData->SC35LiveData.PhotoCurrentValue = this->_node->getResponseBuffer(0);
			this->_oData->SC35LiveData.Vin = this->_node->getResponseBuffer(1) / 10;

			if (this->_oData->SC35LiveData.PhotoCurrentValue < 400) {
				this->_oData->SC35LiveData.LDRStatus = 1;
			}
			else if (this->_oData->SC35LiveData.PhotoCurrentValue < 800) {
				this->_oData->SC35LiveData.LDRStatus = 2;
			}
			else {
				this->_oData->SC35LiveData.LDRStatus = 3;
			}
			log_i("Vin: %f",this->_oData->Msg6Data. LVin);
			log_i("LDRStatus: %d", this->_oData->SC35LiveData.LDRStatus);

			this->_task = ask2;
		}
		else if (result == this->_node->ku8MBResponseTimedOut) {
			log_i("askLantern1 ku8MBResponseTimedOut :: at %d", millis());
		}
		log_i("result askLantern1 ===== %x", result);
	}
	else if (this->_task == ask2) {
		result = this->_node->readInputRegisters(9, 3);
		if (result == this->_node->ku8MBSuccess) {
			this->_oData->LRactivate(f_sc35ada);
			this->_oData->SC35LiveData.masaUpdate = millis()/1000;

			temp0 = this->_node->getResponseBuffer(0);
			this->_oData->SC35LiveData.Nyala = temp0 & 0xff;

			temp0 = this->_node->getResponseBuffer(2);
			this->_oData->SC35LiveData.LedTemp = temp0 & 0xff;

			this->_task = ask3;

			log_i("LLedTemp: %d", this->_oData->SC35LiveData.LedTemp);
			log_i("LNyala: %d", this->_oData->SC35LiveData.Nyala);

		}
		else if (result == this->_node->ku8MBResponseTimedOut) {
			log_i("askLantern2 ku8MBResponseTimedOut :: at %d", millis());
		}
		log_i("result askLantern2 ===== %x", result);
	}
	else if (this->_task == ask3) {
		result = this->_node->readInputRegisters(17, 2);
		if (result == this->_node->ku8MBSuccess) {
			this->_oData->LRactivate(f_sc35ada);
			this->_oData->SC35LiveData.masaUpdate = millis()/1000;

			temp0 = this->_node->getResponseBuffer(0);
			this->_oData->SC35LiveData.AlarmActive = bitRead(temp0, 0);
			this->_oData->SC35LiveData.OffLedPowThres = bitRead(temp0, 1);
			this->_oData->SC35LiveData.OffLowVin = bitRead(temp0, 2);
			this->_oData->SC35LiveData.OffLDR = bitRead(temp0, 3);
			this->_oData->SC35LiveData.OffTemp = bitRead(temp0, 5);
			this->_oData->SC35LiveData.OffForce = bitRead(temp0, 7);

			this->_oData->SC35LiveData.ErrLedShort = bitRead(temp0, 8);
			this->_oData->SC35LiveData.ErrLedOpen = bitRead(temp0, 9);
			this->_oData->SC35LiveData.ErrLedVLow = bitRead(temp0, 10);
			this->_oData->SC35LiveData.ErrVinLow = bitRead(temp0, 11);
			this->_oData->SC35LiveData.ErrLedPowThres = bitRead(temp0, 12);
			this->_oData->SC35LiveData.LEDAdjMaxAvgPow = bitRead(temp0, 13);
			this->_oData->SC35LiveData.GsenIntOccur = bitRead(temp0, 14);
			this->_oData->SC35LiveData.SolarChargingOn = bitRead(temp0, 15);

			temp0 = this->_node->getResponseBuffer(1);
			this->_oData->SC35LiveData.IsNight = bitRead(temp0, 2);
//			if (this->_oData->SC35LiveData.IsNight == 0)  _oTiming->ZoneTime = e_day;
//			else                _oTiming->ZoneTime = e_nite;

			log_i("LIsNight: %d", this->_oData->SC35LiveData.IsNight);

			this->_task = askComplete;
			this->_dataComplete = true;
		}
		else if (result == this->_node->ku8MBResponseTimedOut) {
			log_i("askLantern3 ku8MBResponseTimedOut :: at %d", millis());

		}
		log_i("result askLantern3 ===== %x", result);
	}

}

void SC35::preTransmission() {
	digitalWrite(OUT_DR, HIGH);
}

void SC35::postTransmission() {
	digitalWrite(OUT_DR, LOW);
}

bool SC35::isComplete() {
	return this->_dataComplete;
}

void SC35::resetData() {
	this->_dataComplete = false;
	this->_task = ask1;
}
