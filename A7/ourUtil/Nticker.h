/*
 * Nticker.h
 *
 *  Created on: Nov 19, 2020
 *      Author: suhaimi
 */

#ifndef OURUTIL_NTICKER_H_
#define OURUTIL_NTICKER_H_

#include "Arduino.h"

class N_ticker {
private:
	unsigned long _gap = 0;
	unsigned long _lock = 0;
	bool _overflow = false;
public:
	bool set;

	N_ticker(unsigned long gap, unsigned long lock){
		this->_gap = gap;
		this->_lock = lock + millis() + gap;
		this->set = false;
	}

	bool Update(){
		bool ret = false;
		if(this->_overflow == false){
			if(millis() >= this->_lock){
				this->_lock = millis() + this->_gap;
				this->_overflow = false;
				if(this->_lock < millis()){
					this->_overflow = true;
				}
				ret = true;
				this->set = true;
			}
		}
		else{
			if(millis() <= this->_lock){
				this->_overflow = false;
			}
		}

		return ret;
	}

//	void Reset(){
//		this->_lock = millis()+ this->_gap;
//		this->_overflow = false;
//		if(this->_lock < millis()){
//			this->_overflow = true;
//		}
//		this->set = false;
//	}

	void Reset(uint32_t gap = 0, uint32_t nextStart = 0) {

		if (gap > 0) this->_gap = gap;

		this->_lock = millis() + this->_gap + nextStart;
		this->_overflow = false;
		if(this->_lock < millis()){
			this->_overflow = true;
		}
		this->set = false;

//		if (gap > 0) this->_gap = gap;
//		this->_lock = nextStart + millis();
	}


};

#endif /* OURUTIL_NTICKER_H_ */
