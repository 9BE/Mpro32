/*
 * Nindicator.h
 *
 *  Created on: Nov 19, 2020
 *      Author: suhaimi
 */

#ifndef OURUTIL_NINDICATOR_H_
#define OURUTIL_NINDICATOR_H_

#include "Arduino.h"

class N_indicator {
private:
	uint16_t _pattern = 1;
	uint16_t _mod = 0;
	uint32_t _masaLed = 0;

public:
	N_indicator(){}
	virtual ~N_indicator(){}

	void mainLampu() {
		if (millis() - this->_masaLed >= 62) {
			this->_masaLed = millis();




			uint16_t set = (this->_pattern>>this->_mod)&1;
			digitalWrite(12, set);

//			log_i("bit baru = %d :: set = %d",this->_mod, set);

			this->_mod++;

			if (this->_mod == 16) {
				this->_mod = 0;
//				log_i("saat ::::::::::::::::::::: %d", millis()/1000);
			}

		}
	}

	void setPattern(uint16_t pattern) {
		this->_pattern = pattern;
		this->_mod = 0;
	}
};

#endif /* OURUTIL_NINDICATOR_H_ */
