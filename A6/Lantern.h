/*
 * Lantern.h
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#ifndef LANTERN_H_
#define LANTERN_H_

#include "Arduino.h"
#include "board.h"
#include <ModbusMaster.h>
#include "timing.h"
#include "Mando.h"
#include "esp_task_wdt.h"
#include "JsonHandler.h"

enum LanternTask {
	lt_NONE = 50,
	lt_SC35,
	lt_SC35_nf,
	lt_LANTERNreader,
	lt_LANTERNreader_nf,
	lt_CheckLanternLost,
	lt_Decision
};

class Lantern {
public:
	Lantern(const BaseType_t xCoreID, uint64_t loopDelay, timing * tim, Mando * mando);
	virtual ~Lantern();
	static void preTransmission();
	static void postTransmission();

	void lanternRead();
	void reInit();

	bool isMulai() const {
		return _mulai;
	}

	void setMulai(bool mulai = false) {
		_mulai = mulai;
		if (mulai) _lanternTask = lt_SC35;
		else _lanternTask = lt_NONE;
	}

	uint8_t getLanternTaskStat() const {
		return _lanternTaskStat;
	}

	void setLanternTaskStat(uint8_t lanternTaskStat = 0) {
		_lanternTaskStat = lanternTaskStat;
	}

	const String& getAlert() const {
		return alert;
	}

	bool jumpaSC35 = false;
	char lanternRespond = 0;


	//lantern reader
	bool jumpaLR = false;
	String LanternStat;
	float ThresApp;

	String LanternLongStatus;
	bool lanternlock = false;
	//Beacon
	float PrimMaxMinDiff;
	float SecMaxMinDiff;
	float Primthresholdamp;
	float Secthresholdamp;
	// lighthouse
	bool lanternlockC = false;
	bool lanternlockB = false;
	String emergencyStat;

	int FreeContact;

	bool PSU;
	bool Xtra1;
	bool Xtra2;

	bool findEmergency = false;
	bool askLantern3 = false;

	uint8_t _nyalaNormal = 3;
	int _lrNyala = 0;


private:
	static void loop(void * param);
	bool handleLR();
	bool handleSC35();
	void makeDecision();
	void checkLanternLost();
	void normalize();


	bool _mulai		= false;
	uint64_t _loopDelay;
	uint8_t		_lanternTaskStat = lt_NONE;

	unsigned long kalaujumpalanterncepat;
	uint64_t _masaTungguLantern = 0;

	timing * _oTiming;
	ModbusMaster node;

	uint64_t _timeCtr;
	Mando * _oMando;
	uint16_t counterVrespond;

	LanternTask _lanternTask = lt_NONE;



	//counter isu spike
	bool DoNotChange = false;
	int CounterD = 0;
	int CounterN = 0;
	int Weight = 0;

	String lightStatus;


	//String ffffosOff;
	//char counterVrespond = 0;
	bool askLantern1 = false;
	bool askLantern2 = false;
	String alert = "";
	int _prevNyala = 0;



	//char askLanternTimeUp = 300;





	//int Lflashcode;

	//int SLNyala;
	//bool lanternLostIgnore = false;


};

extern ModbusMaster node;

#endif /* LANTERN_H_ */
