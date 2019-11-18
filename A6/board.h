/*
 * board.h
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#ifndef BOARD_H_
#define BOARD_H_

#include 	"Arduino.h"
#include	<WiFi.h>

#define TX3   		25    // tx rs232
#define RX3   		26    // rx rs232
#define TX2   		23    // tx rs485
#define RX2   		22    // rx rs485
#define OUT_DR  	27    // rs485 driver/receiver enable pin : HIGH = driver(tx), LOW = receiver(rx)
#define LED_PIN		2

class board {
public:
	board();

	String getMAC();

	int getChannel();
	virtual ~board();

//	String macID;
};


#endif /* BOARD_H_ */
