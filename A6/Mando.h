/*
 * Mando.h
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#ifndef MANDO_H_
#define MANDO_H_

#include "Arduino.h"
#include "AISmsg.h"
#include "timing.h"
#include "Data.h"
#include "esp_task_wdt.h"

enum MandoTask {
	mt_none = 30,
	mt_ada,
	ACE,	//31
	ACF,	//32
	VER,	//33
	AAR21,	//34
	AAR06,	//35
	RMC,	//36
	VDO21,	//37
	SendBIT,//38
	Send06,	//39
	ABK06,
	mt_cNone,
	cAID1,
	cAID2,
	cACE,
	cACF,
	cAAR,
	cPAMC
};

class Mando {
public:
	Mando(const BaseType_t xCoreID, uint64_t loopDelay, timing * tim);
	virtual ~Mando();
	void nakHantarM06();
	void checkAtonBit();
	void reInit();
	void sambung();

	uint32_t getMandoTaskStat() const {
		return _mandoTaskStat;
	}


	void setMulai(bool mulai = false) {
		_mulai = mulai;
		if (mulai) {
			_mandoTaskStat = mt_none;
			mandoTask = ACE;
		}
	}

	bool isMulai() const {
		return _mulai;
	}

	bool isNakConfig() const {
		return _nakConfig;
	}

	void setNakConfig(bool nakConfig = false) {
		_nakConfig = nakConfig;
		if (nakConfig) {
			mandoTask = cAID1;
			_mandoTaskStat = mt_cNone;
			_confProgress = 0;
			_confFull = 0;
		}
	}

	void setMandoTaskStat(uint8_t mandoTaskStat = 0) {
		_mandoTaskStat = mandoTaskStat;
	}

	void setMandoTask(MandoTask mandoTask) {
		this->mandoTask = mandoTask;
	}

	uint64_t getMasaNmea() const {
		return _masaNMEA;
	}

	bool _inputMandoAce			= false;
	bool _inputMandoRmc     	= false;
	bool _inputMandoVdo06		= false;
	bool _inputMandoVdo21		= false;
	String _nmea = "";
	float tmpLat = 0;
	float tmpLng = 0;
	String ProcAtonbit_prev = "XX";
	String ProcAtonbit_now;
	char MSerialNumber[10] = "0";

	// perlu dikeluarkan

	String MTempStrRegLat = "0";
	String MTempStrRegLng = "0";
	String MFullTextAcf = "0";
	String MFullTextAce = "0";
	String MTemporaryMMSI = "0";
	String M06IntervalA = "0";
	String M06IntervalB = "0";
	String M21IntervalA = "0";
	String M21IntervalB = "0";

	float lat, lng;

	unsigned long agoM6 = 0;
	unsigned long agoM21 = 0;
	const char* lastM06 = "";
	const char* lastM21 = "";
	uint8_t _confFull;
	uint8_t _confProgress;
	float rawBattery = 0;
	uint64_t _masaMando = 0;



	struct atonbit {
		byte alarmX;
		byte lantern;
		byte racon;
		byte page = 7;

	}AtonBit;

	struct msg6Data M6data;
	struct confData SpiffsData;
	struct mandoConfig _mConfig;
	struct machineData _machine;

	uint32_t _kaliHantarM6 = 0;

private:
	static void loop(void * param);
	void processMandoData();
	void hantarDataMando();
	String getLookUpHeader();
	int getBit(String& pass, byte from, byte until);
	void beating();


	timing * _oTiming;

	String _mandoRaw = "";

	uint64_t _loopDelay;
	bool _mulai					= false;
	bool _nakConfig 			= false;


	uint64_t _runCtr = 0;
	uint64_t _hantarDelay = 0;
	uint64_t _masaNMEA = 0;


	char buffer[768];

	uint8_t _mandoTaskStat = mt_none;
	uint8_t _channel = 1;

	MandoTask mandoTask;





};





#endif /* MANDO_H_ */
