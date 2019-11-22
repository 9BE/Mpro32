/*
 * timing.h
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#ifndef TIMING_H_
#define TIMING_H_

#include "Arduino.h"

#define UTCOFFSET	8
#define START_YEAR 	1970
#define START_HARI 	5

struct waktu{
	unsigned char saat;
	unsigned char minit;
	unsigned char jam;
};

struct dateTime{
	struct waktu time;
	unsigned int hari;
	unsigned int bulan;
	uint32_t tahun;
	String sMasa;

};

enum {
	e_nite,
	e_dawn,
	e_day,
	e_dusk
};


class timing {
public:
	timing();
	virtual ~timing();

	const char days_month[2][12] = {
	  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	  { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	    };

	void localDateTime(struct dateTime *p, long utc, char offsetHour);
	int isleap(int year);
	double J(int year,char month, char day);
	double sun(int year, char month, char day, double lat, double lng, double ketika, double zone, double alt);
	void regSun(struct waktu *m, double kini);
	void susunMasa(double lat, double lng);
	char dayZone(void);
	long seconds(struct waktu *m);
	bool tickTock();
	void toSecond();
	String twoDigits(int digits);

	const struct dateTime& getNow() const {
		return now;
	}

	char ZoneTime = 0;
	struct dateTime now;

	struct waktu sunrise;
	struct waktu sundown;
	struct waktu dawn;
	struct waktu dusk;
	unsigned long utc = 0;
	unsigned long waitUntil = 0;


};


#endif /* TIMING_H_ */
