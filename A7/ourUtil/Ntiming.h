/*
 * Ntiming.h
 *
 *  Created on: Dec 1, 2020
 *      Author: suhaimi
 */

#ifndef OURUTIL_NTIMING_H_
#define OURUTIL_NTIMING_H_

#include "Arduino.h"

#define UTCOFFSET	8
#define START_YEAR 	1970
#define START_HARI 	5

typedef struct {
	unsigned char saat;
	unsigned char minit;
	unsigned char jam;
}Waktu;

typedef struct {
	Waktu time;
	unsigned int hari;
	unsigned int bulan;
	uint32_t tahun;
	String sMasa;

}DateTime;

typedef enum {
	e_nite,
	e_dawn,
	e_day,
	e_dusk
}Ketika;

class Ntiming {
private:
	const char days_month[2][12] = {
	  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	  { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	    };


	Waktu sunrise;
	Waktu sundown;
	Waktu dawn;
	Waktu dusk;

	uint32_t tunggu = 0;
	double _lat, _lng;

	void localDateTime(DateTime *p, long utc, char offsetHour);
	int isleap(int year);
	double J(int year,char month, char day);
	double sun(int year, char month, char day, double lat, double lng, double ketika, double zone, double alt);
	void regSun(Waktu *m, double kini);
	void susunMasa(double lat, double lng);
	char dayZone(void);
	long seconds(Waktu *m);
	String twoDigits(int digits);
	void toSecond();


public:
	Ntiming();
	virtual ~Ntiming();

	void ticktock();
	void setTimeNow(uint8_t jam, uint8_t minit, uint8_t saat, uint32_t hari, uint32_t bulan, uint32_t tahun, double lat, double lng);
	String getZoneTimeStr();
	char ZoneTime = 0;
	DateTime now;
	uint32_t utc = 0;
};

#endif /* OURUTIL_NTIMING_H_ */
