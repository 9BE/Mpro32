/*
 * LanternReader.h
 *
 *  Created on: Jan 6, 2021
 *      Author: suhaimi
 */

#ifndef OURS_LANTERNREADER_H_
#define OURS_LANTERNREADER_H_

#include "Arduino.h"
#include "DataHandler.h"
#include "WebServer.h"
#include "ArduinoJson.h"

class LanternReader {

private:
	static void loop(void * param);
	int getStat(String & cari);
	TaskHandle_t _handle = NULL;

	DataHandler * _oData;
	WebServer * _server;

	String _tempAtonBit = "XX";

	bool _locked = false;
	bool _lockedB = false;
	bool _lockedC = false;

	String _statList[4] = {"Booting", "Calibrating", "Processing", "Locked"};
	int _statSize = sizeof(_statList)/sizeof(_statList[0]);



public:
	LanternReader(DataHandler * data, WebServer * server);

	void buoy(LanternReader * ini);
	void lighthouse(LanternReader * ini);
	void beacon(LanternReader * ini);

};

#endif /* OURS_LANTERNREADER_H_ */
