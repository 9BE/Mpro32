/*
 * SC35.h
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#ifndef OURS_SC35_H_
#define OURS_SC35_H_

#include "Arduino.h"
#include "DataHandler.h"
#include "ModbusMaster.h"
#include "Nticker.h"

#define OUT_DR  	27    // rs485 driver/receiver enable pin : HIGH = driver(tx), LOW = receiver(rx)

typedef enum {
	ask1,
	ask2,
	ask3,
	askComplete
}SC35task_t;



class SC35 {

private:
	static void loop(void * param);
	void askSC35();
	TaskHandle_t _handle = NULL;

	SC35task_t _task = ask1;

	DataHandler * _oData;
	ModbusMaster * _node;
	HardwareSerial * _serial;

	bool _dataComplete = false;

public:
	SC35(DataHandler * data, HardwareSerial * serial);
	static void preTransmission();
	static void postTransmission();
	bool isComplete();
	void resetData();





};

#endif /* OURS_SC35_H_ */
