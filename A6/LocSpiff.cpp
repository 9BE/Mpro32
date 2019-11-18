/*
 * LocSpiff.cpp
 *
 *  Created on: Aug 4, 2019
 *      Author: annuar
 */

#include "LocSpiff.h"
#include "SPIFFS.h"
#include "FS.h"
#include <vector>
#include <string>

LocSpiff *iniSpiff;

LocSpiff::LocSpiff(String created) {
	iniSpiff = this;

	iniSpiff->_mana = created;

	log_i("LocSpiff ---------------------------- bina @ %s", iniSpiff->_mana.c_str());
}

LocSpiff::~LocSpiff() {
	log_i("LocSpiff ---------------------------- musnah @ %s", iniSpiff->_mana.c_str());

}

void LocSpiff::listAllFiles() {

	if(SPIFFS.begin(true)){
		File root = SPIFFS.open("/");

		File file = root.openNextFile();

		while(file){
//			il.setMsg("File: " + String(file.name()));
			log_i("File: %s",String(file.name()).c_str());

			file = root.openNextFile();
		}

		root.close();
		file.close();

		SPIFFS.end();
	}


}



bool isDelim(const char& query, const String& delimList){
    for(int i = 0; i < delimList.length(); i++)
        if(query == delimList[i])
            return true;
    return false;
}

std::vector<String> split(const String& line, const String& delimList){
    int len = line.length();
    String temp;
    std::vector<String> output;
    for(int i = 0; i < len; i++){
        if(isDelim(line[i], delimList)){
            if(temp.length() > 0)
                output.push_back(temp);
            temp="";//.clear();
        }
        else
            temp += line[i];
    }
    if(temp.length() > 0)
        output.push_back(temp);
    return output;
}

std::vector<std::vector<String>> LocSpiff::readCSV(const char* path) {


	std::vector<std::vector<String>> rows;
	std::vector<String> cols;

    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path);
        if(!file || file.isDirectory()){
            log_e("- failed to open file for reading");
            file.close();
            SPIFFS.end();
            return rows;
        }

        log_i("- read from file:");
        while(file.available()){
        	String ss = file.readStringUntil('\n');

        	cols = split(ss, ",");
        	rows.push_back(cols);
        }
        file.close();
        SPIFFS.end();
    }

    return rows;

}





void LocSpiff::renameFile(const char* path1, const char* path2) {
    if(SPIFFS.begin(true)){

        if (SPIFFS.rename(path1, path2)) {
        	log_i("- file renamed");
        } else {
        	log_e("- rename failed");
        }
        SPIFFS.end();
    }


}

void LocSpiff::appendFile(const char* path, const char* message) {
    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path, FILE_APPEND);
        if(!file){
        	log_e("- failed to open file for appending");
        	file.close();
        	SPIFFS.end();
            return;
        }

        file.print(message);
        file.close();
        SPIFFS.end();
    }
}

/*
 * 	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		err = true;
	}

	if (!err) {
		File file;
		String fileName = "/";
		whatToSave.toLowerCase();
		fileName += whatToSave;
		fileName += ".txt";
		file = SPIFFS.open(fileName, FILE_WRITE);

		if (!file) {
			Serial.println("There was an error opening the file for writing");
			err = true;
		}

		if (!err && file.print(value)) {
			Serial.println("File was written");

//			if(whatToSave == "min"){
//				Ais.min = value.toInt();
//			}
//			else{
//				Ais.max = value.toInt();
//			}
		} else {
			Serial.println("File write failed :(   huuuuuu");
			err = true;
		}

		file.close();
	}
	SPIFFS.end();
 */

bool LocSpiff::writeFile(const char* path, const char* message) {
	bool res= false;
    if(SPIFFS.begin(true)){

    	log_i("Path = %s", path);

        File file;

//        file.clearWriteError();
        log_i("SINI");
        file  = SPIFFS.open(path, FILE_WRITE);


        if(!file){
        	log_e("- failed to open file for writing");
        	file.close();
        	SPIFFS.end();
            return res;
        }

        log_i("Kat sini %s", message);

        if(file.print(message)){
        	yield();
        	delay(0);
        	log_i("- file written");
        	res = true;
        } else {
        	log_e("- write failed");
        	res = false;
        }
        file.close();
        SPIFFS.end();


    }
    else{
    	log_i("~~~~~~spiff begin fail");
    }

    SPIFFS.end();

    return res;
}

/*
 * 	String res = "";
	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		err = true;
	}

	if (!err) {
		File file;
		String fileName = "/";
		whatToRead.toLowerCase();
		fileName += whatToRead;
		fileName += ".txt";

		file = SPIFFS.open(fileName);

		if(!file){
			Serial.println("Failed to open file for reading");
			err = true;
		}

		if (!err) {
//			Serial.println("File Content::");

			while(file.available()){

				res += file.readString();
			}
//			Serial.print(whatToRead + "::");
//			Serial.println(res);
			file.close();
		}
	}
	SPIFFS.end();
 */

String LocSpiff::readFile(const char* path) {
	String res = "";
    if(SPIFFS.begin(true)){
        File file = SPIFFS.open(path);
        if(!file || file.isDirectory()){
            log_e("- failed to open file for reading");
            res = "NAN";

//            file = SPIFFS.open(path, FILE_WRITE);
//            if(file.print("")){
//            	log_i("- file written");
//            	res = true;
//            } else {
//            	log_e("- write failed");
//            }

//            file.close();
            SPIFFS.end();
            return res;
        }

        log_i("- read from file:");
        while(file.available()){
        	res += file.readString();
        }
        file.close();
        SPIFFS.end();
    }
    return res;
}
void LocSpiff::listDir(const char* dirname, uint8_t levels) {


    if(SPIFFS.begin(true)){
        File root = SPIFFS.open(dirname);
        if(!root){
            Serial.println("Failed to open directory");
//            root.close();
            return;
        }
        if(!root.isDirectory()){
            Serial.println("Not a directory");
//            root.close();
            return;
        }

        File file = root.openNextFile();
        while(file){
            if(file.isDirectory()){
                Serial.print("  DIR : ");
                Serial.print (file.name());
                time_t t= file.getLastWrite();
                struct tm * tmstruct = localtime(&t);
                Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
                if(levels){
                    listDir(file.name(), levels -1);
                }
            } else {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.print(file.size());
                time_t t= file.getLastWrite();
                struct tm * tmstruct = localtime(&t);
                Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            }
            file = root.openNextFile();
        }


        root.close();
        file.close();

        SPIFFS.end();
    }
}

FileInfo_t LocSpiff::getInfo(const char* path) {
	FileInfo_t res;

	res.filename = "";
	res.lastWrite = 0;
	res.size = -1;

    if(SPIFFS.begin(true)){

    	if(SPIFFS.exists(path)){
            File file = SPIFFS.open(path, FILE_READ);
            if(!file){
            	log_e("- failed to open file for writing");
            	file.close();
            	SPIFFS.end();
                return res;
            }

            res.filename = file.name();// path;
            res.lastWrite = file.getLastWrite();
            res.size = file.size();
            file.close();
    	}
    	else{
    		res.filename = "NOFILE";
    		res.lastWrite = 0;
    		res.size = -1;
    	}
    }
	SPIFFS.end();


	return res;

}

void LocSpiff::deleteFile(const char* path) {
    if(SPIFFS.begin(true)){

        if(SPIFFS.remove(path)){
        	log_i("- file deleted");
        } else {
        	log_e("- delete failed");
        }
        SPIFFS.end();
    }
}

//void LocSpiff::createDir(const char* path) {
//    if(SPIFFS.begin(true)){
//
//        Serial.printf("Creating Dir: %s\n", path);
//        if(SPIFFS.mkdir(path)){
//        	il.setMsg("Dir created");
//        } else {
//        	il.setMsg("mkdir failed");
//        }
//        SPIFFS.end();
//    }
//
//
//}
//
//void LocSpiff::removeDir(const char* path) {
//    if(SPIFFS.begin(true)){
//
//        if(SPIFFS.rmdir(path)){
//            Serial.println("Dir removed");
//        } else {
//            Serial.println("rmdir failed");
//        }
//        SPIFFS.end();
//    }
//
//
//
//}

void LocSpiff::format() {
    if(SPIFFS.begin(true)){

    	SPIFFS.format();
        SPIFFS.end();
    }

}


