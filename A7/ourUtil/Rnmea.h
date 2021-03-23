/*
 * Rnmea.h
 *
 *  Created on: Dec 1, 2020
 *      Author: suhaimi
 */

#ifndef OURUTIL_RNMEA_H_
#define OURUTIL_RNMEA_H_

#include "Arduino.h"

typedef struct {
	int data;
	int size;
} NmeaData_t;

class Rnmea {
private:
	String char2bin(char z, char size);
	char sixChar(char data);
	int checkSum(String& data);
public:
	Rnmea();
	virtual ~Rnmea();

	std::vector<double> exploade(String load, int arr[], int size);
	String compacte(const std::vector<NmeaData_t> &load);
	void appendCheckSum(String& load);
	String extractCol(String data, int index);
	double toDegree(String snum);
	bool validCheckSum(String word);
	void addData(std::vector<NmeaData_t> &load, int data, int size);
	String toAISstd(String& snum, bool lat);




};

#endif /* OURUTIL_RNMEA_H_ */
