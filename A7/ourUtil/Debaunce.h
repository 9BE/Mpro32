/*
 * Debaunce.h
 *
 *  Created on: Jan 6, 2021
 *      Author: suhaimi
 */

#ifndef OURUTIL_DEBAUNCE_H_
#define OURUTIL_DEBAUNCE_H_

class Debaunce {
private:
	int _state = 0;
	int _lastState = 0;
	int _tingkat = 0;
public:
	Debaunce() {}

	int getState(int x) {
		if (x != _lastState) {
			this->_tingkat = 0;
		}else {
			this->_tingkat++;

			if (this->_tingkat > 2) {
				this->_state = x;
			}
		}

		this->_lastState = x;

		return this->_state;
	}


};



#endif /* OURUTIL_DEBAUNCE_H_ */
