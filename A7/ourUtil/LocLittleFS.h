/*
 * LocLittleFS.h
 *
 *  Created on: Nov 2, 2020
 *      Author: suhaimi
 */

#ifndef LOCAL_LOCLITTLEFS_H_
#define LOCAL_LOCLITTLEFS_H_

#include "Arduino.h"
#include "LITTLEFS.h"
#include "FS.h"
//#include "LocTicker.h"

class LocLittleFS {
private:
	String _dari;
public:
	LocLittleFS(const char * dari);
	virtual ~LocLittleFS();
	bool saveFile(const char * path, const char * data);
	String readFile(const char * path);
	String getValue(String data, char separator, int index);

};

#endif /* LOCAL_LOCLITTLEFS_H_ */
