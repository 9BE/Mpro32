/*
 * OccasHandler.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: suhaimi
 */

#include "OccasHandler.h"



OccasHandler::OccasHandler() {
	this->o_data = new DataHandler();
	this->o_sc35 = new SC35(this->o_data, &Serial1);
}


