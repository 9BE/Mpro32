/*
 * Mando.h
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#ifndef OURS_MANDO_H_
#define OURS_MANDO_H_

#include "Arduino.h"
#include "DataHandler.h"
#include "Rnmea.h"
#include "Nticker.h"

typedef enum {
	mt_none = 30,	// 00000000 (0)
	mt_ada,
	ACE,	//31	00000001 (index = 0)
	ACF,	//32	00000010 (index = 1)
	VER,	//33	00000100 (index = 2)
	AAR,
	AAR21,	//34	00001000 (index = 3)
	AAR06,	//35	00010000 (index = 4)
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
	cPAMC,
	mt_reboot
} MandoTask;

class Mando {

private:
	static void loop(void * param);
	TaskHandle_t _handle = NULL;

	String _raw = "";
	String _fullACF = "";
	String _tempMMSI = "";

	HardwareSerial * _serial;
	DataHandler * _nData;
	Rnmea		* _nmea;

	MandoTask _xTask = mt_none;

	bool _jumpaABK = false;
	bool _timeout = false;
	bool _nakResetMando = false;

	int _chan = 2;
	uint8_t _beat = 1;
	uint8_t _msgSq = 0;

	void arahkan();
	void processData();
	bool hantarMesej();

public:
	Mando(HardwareSerial * serial, DataHandler * ndata);

};

#endif /* OURS_MANDO_H_ */
