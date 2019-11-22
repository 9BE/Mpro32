/*
 * LReader.h
 *
 *  Created on: Nov 7, 2019
 *      Author: suhaimi
 */

#ifndef LREADER_H_
#define LREADER_H_

#include "Arduino.h"
#include "Lantern.h"
#include "Mando.h"
#include <WebServer.h>

class LReader {
public:
	LReader(Lantern * lantern, Mando * mando);
	virtual ~LReader();

	static void Buoy();
	static void Lighthouse();
	static void Beacon();

	uint64_t masaLR = 0;


private:

	Lantern * _oLantern;
	Mando * _oMando;

	//------------------------LANTERN READER----------------------

	bool connectionputus = false;
	bool connection = false;
	bool tukarconnection = false;

	unsigned long connectionputusrestart;
	unsigned long queryinterval;
	unsigned long notconnected;
	unsigned long kalaujumpalanterncepat;

	uint8_t _normal = 0;

	String _tempAtonBit = "XX";


};

extern WebServer _server;

#endif /* LREADER_H_ */
