/*
 * OccasHandler.h
 *
 *  Created on: Mar 22, 2021
 *      Author: suhaimi
 */

#ifndef OURS_OCCASHANDLER_H_
#define OURS_OCCASHANDLER_H_

#include "Arduino.h"
#include "SC35.h"
#include "DataHandler.h"


class OccasHandler {
public:
	OccasHandler();

	DataHandler * o_data;
	SC35 * o_sc35 = NULL;
};


#endif /* OURS_OCCASHANDLER_H_ */
