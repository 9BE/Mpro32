/*
 * LocLittleFS.cpp
 *
 *  Created on: Nov 2, 2020
 *      Author: suhaimi
 */

#include "LocLittleFS.h"



LocLittleFS::~LocLittleFS() {
	// TODO Auto-generated destructor stub
	log_i("musnah .............. dari %s", this->_dari.c_str());
}

LocLittleFS::LocLittleFS(const char* dari) {
	log_i("bina .............. dari %s", dari);
	this->_dari = dari;
}

bool LocLittleFS::saveFile(const char* path, const char* data) {
//	LocTicker * tick = new LocTicker(30000);

    Serial.printf("Writing file: %s\r\n", path);

    if (LITTLEFS.begin(true)) {
        File file = LITTLEFS.open(path, FILE_WRITE);
        if(!file){
            Serial.println("- failed to open file for writing");
            return false;
        }
        if(file.print(data)){
            log_i("- file written %d oleh %s", file.size(), this->_dari.c_str());
        } else {
            Serial.println("- write failed");
        }
        file.close();
//        LITTLEFS.end();
        return true;
    }

    return false;
}

String LocLittleFS::readFile(const char* path) {
	String res = "";
    log_i("Reading file: %s oleh %s", path, this->_dari.c_str());

    if (LITTLEFS.begin(true)) {
        File file = LITTLEFS.open(path);
        if(!file || file.isDirectory()){
            Serial.println("- failed to open file for reading");
            return "";
        }

        while(file.available()){
            res.concat((char) file.read());
        }
        file.close();
//        LITTLEFS.end();
    }
    return res;
}

String LocLittleFS::getValue(String data, char separator, int index) {
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
