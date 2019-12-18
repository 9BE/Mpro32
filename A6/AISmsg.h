/*
 * AISmsg.h
 *
 *  Created on: Nov 5, 2019
 *      Author: suhaimi
 */

#ifndef AISMSG_H_
#define AISMSG_H_

#include "Arduino.h"
#include "Data.h"
#include "timing.h"

class AIS_msg {
public:
	AIS_msg();
	virtual ~AIS_msg();
	void ConstructM21(String& MFullTextAce, String& ProcAtonbit_prev);
	void makeAtonBit(String& ProcAtonbit_now, byte alarmX, byte lantern, byte racon, byte page);
	float getGps(String& location, const char * cardinal);
	String gpstoAISlatstandard(String& posisi);
	String gpstoAISlngstandard(String& posisi);
	void ConstructMsg06(char * memory, long dest, int channel, msg6Data &data, byte alarmX, byte lantern, byte racon, byte page, timing * tim, confData& ConfData);
	void decodeM21(String myText, int & ProcOffPositionStatus);
	void checkSum(String& mesej);
	String getPara(String& data, char index);
	bool validate(String& word);

	int _debLamp = 0;
	int _debLDR = 0;

private:
	void message6bits(long data, char nums, char * memory);
	void genEncData(char *memory);
	char sixChar(char data);

	String ProcOff_pos;


};



#endif /* AISMSG_H_ */
