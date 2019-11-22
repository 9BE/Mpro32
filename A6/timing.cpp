/*
 * timing.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: suhaimi
 */

#include "timing.h"



timing::timing() {
	// TODO Auto-generated constructor stub
}

timing::~timing() {
	// TODO Auto-generated destructor stub
}

String timing::twoDigits(int digits) {
	if (digits < 10) {
		String i = '0' + String(digits);
		return i;
	} else {
		return String(digits);
	}
}

void timing::localDateTime(dateTime* p, long utc, char offsetHour) {

	long days, j;
	int i = START_YEAR;
	int k;
	char hb;

	waktu *jj;

	jj = &p->time;


	utc += offsetHour * 3600;				// Correct for TZ/DST offset	//1450153545
	days = utc / 86400;						// Integer number of days	//16784
	j = utc % 86400;						// Fraction of a day			//15945
	jj->jam = j /3600;						// Integer number of hours
	j %= 3600;								// Fraction of hour
	jj->minit = j / 60;						// Integer number of minutes
	jj->saat = j % 60;						// Remainder seconds

	// Count the number of days per year taking in account leap years
	// to determine the year number and remaining days
	while(days >= (j = isleap(i) ? 366 : 365) ) {
		i++;
		days -= j;
	}



	p->tahun = i;							// We have the year

	i = isleap(i);
	// Count the days for each month in this year to determine the month


	k = 0;
	while(1){
		hb = days_month[i][k];
		if(days >= hb){
			days = days - hb;
		}
		else{
			break;
		}
		k = k + 1;
	}

	k = k + 1;

	p->bulan = (char)k;							// We have the month
	p->hari = days + 1;						// And the day of the month

}

int timing::isleap(int year) {
	char y;

	y = ((year % 4 == 0) && (year % 100 != 0 || year % 400 == 0));
	return (y);
}

double timing::J(int year, char month, char day) {
	//=367*B5-INT(7*(B5+INT((B6+9)/12))/4)+INT(275*B6/9)+B7-730531.5
	double jawapan = 0;
	double temp;

	jawapan = 367L * year;

	temp = (int) (7 * (year + (int) ((month + 9) / 12)) / 4);
	jawapan -= temp;

	//INT(275*month/9)
	temp = 275 * month;
	temp /= 9;
	jawapan += (int) temp;

	jawapan += (int) day;
	jawapan -= 730531.5;

	return jawapan;
}

double timing::sun(int year, char month, char day, double lat, double lng,
		double ketika, double zone, double alt) {

	double daysSince;
	double centuries;
	double L, G, ec, lamda, E, obl, delta, GHA, cosc, correction, utnew,
			EventUT;
	double temp, temp1;

	daysSince = J(year, month, day);
	centuries = daysSince / 36525;

	//  L = MOD(4.8949504201433+628.331969753199*$E$8,6.28318530718);
	L = 628.331969753199 * centuries;
	L += 4.8949504201433;
	L /= 6.28318530718;
	L -= (int) L;
	L *= 6.28318530718;

	//  G = MOD(6.2400408+628.3019501*$E$8,6.28318530718)
	G = 628.3019501 * centuries;
	G += 6.2400408;
	G /= 6.28318530718;
	G -= (int) G;
	G *= 6.28318530718;

	//  ec =0.033423*SIN(B18)+0.00034907*SIN(2*B18)
	ec = 0.033423 * sin(G);
	temp = 2 * G;
	temp = sin(temp);
	temp *= 0.00034907;
	ec += temp;

	lamda = ec + L;

	//  E = 0.0430398*SIN(2*B20) - 0.00092502*SIN(4*B20) - B19
	E = 2 * lamda;
	E = sin(E);
	E *= 0.0430398;
	temp = 4 * lamda;
	temp = sin(temp);
	temp *= 0.00092502;
	E -= temp;
	E -= ec;

	//  obl =0.409093-0.0002269*$E$8
	obl = 0.409093;
	temp = 0.0002269 * centuries;
	obl -= temp;

	//  delta = ASIN(SIN(B22)*SIN(B20))
	delta = sin(obl);
	temp = sin(lamda);
	delta *= temp;
	delta = asin(delta);

	//  GHA = 3.14159265358979 - 3.14159265358979 + B21
	GHA = E;
	//  Serial.print("GHA = ");
	//  Serial.println(GHA);

	//  cosc =(SIN(0.017453293*$B$11) - SIN(0.017453293*$B$8)* SIN(B23) )/ (COS(0.017453293*$B$8)*COS(B23) )
	//                                                                     (COS(0.017453293*$B$8)*COS(B23)
	temp = 0.017453293 * lat;
	temp = cos(temp);
	temp *= cos(delta);
	//                                  SIN(0.017453293*$B$8)* SIN(B23)
	temp1 = 0.017453293 * lat;
	temp1 = sin(temp1);
	temp1 *= sin(delta);
	//        (SIN(0.017453293*$B$11)
	cosc = alt * 0.017453293;
	cosc = sin(cosc);

	cosc -= temp1;
	cosc /= temp;

	//  correction = ACOS(B25)
	correction = acos(cosc);
	//  Serial.print("LHA = ");
	//  Serial.println(correction);

	//  utnew = 3.14159265358979 - (B24 + 0.017453293*$B$9 + $B$12*B26)
	temp = ketika * correction;
	temp1 = 0.017453293 * lng;
	temp += temp1;
	temp += GHA;
	utnew = 3.14159265358979;
	utnew -= temp;

	//  EventUT = B27*57.29577951/15
	EventUT = 57.29577951 * utnew;
	EventUT /= 15;
	EventUT += zone;
	//  Serial.print("EventUT = ");
	//  Serial.println((String)EventUT);
	//  Serial.println();

	return EventUT;
}

void timing::regSun(waktu* m, double kini) {
	double temp;
	m->jam = (int) kini;
	temp = kini - (int) kini;
	temp *= 60;
	m->minit = (int) temp;
	temp -= (int) temp;
	temp *= 60;
	m->saat = (int) temp;
}

void timing::susunMasa(double lat, double lng) {
	double waktu2;

	localDateTime(&now, utc, UTCOFFSET); //--inside sun.c : susunMasa

	waktu2 = sun(now.tahun, now.bulan, now.hari, lat, lng, 1.0, 8.0,
			-0.833);
	regSun(&sunrise, waktu2);
	waktu2 = sun(now.tahun, now.bulan, now.hari, lat, lng, -1.0, 8.0,
			-0.833);
	regSun(&sundown, waktu2);
	waktu2 = sun(now.tahun, now.bulan, now.hari, lat, lng, 1.0, 8.0,
			-6.0);
	regSun(&dawn, waktu2);
	waktu2 = sun(now.tahun, now.bulan, now.hari, lat, lng, -1.0, 8.0,
			-6.0);
	regSun(&dusk, waktu2);

	ZoneTime = dayZone();

	now.sMasa = (String(
			twoDigits(now.time.jam) + ":" + twoDigits(now.time.minit) + ":"
					+ twoDigits(now.time.saat))).c_str();
}

char timing::dayZone(void) {
	long lNow, lSunRise, lSunDown, lSunDusk, lSunDawn;
	waktu *n;

	n = &now.time;

	lNow = seconds(n);
	lSunRise = seconds(&sunrise);
	lSunDown = seconds(&sundown);
	lSunDusk = seconds(&dusk);
	lSunDawn = seconds(&dawn);

	if (lNow > lSunRise) {
		if (lNow > lSunDown) {
			if (lNow > lSunDusk) {
				return e_nite; // nite    //error: '0' was not declared in this scope
			} else {
				return e_dusk; //dusk   //error: '3' was not declared in this scope
			}
		} else {
			return e_day; //day   //error: '2' was not declared in this scope
		}
	} else {
		if (lNow > lSunDawn) {
			return e_dawn; //dawn   //error: '1' was not declared in this scope
		} else {
			return e_nite; //nite   //error: '0' was not declared in this scope
		}
	}
}

long timing::seconds(waktu* m) {
	long inSec;

	inSec = m->jam * 3600L;
	inSec += m->minit * 60L;
	inSec += m->saat;

	return inSec;
}

bool timing::tickTock() {
	bool res = false;
	if ((unsigned long) (millis() - waitUntil) >= 1000) {  // check for rollover
		waitUntil = waitUntil + 1000;
		utc++;
//		localDateTime(&now, utc, 8);
//		now.sMasa = (String(
//				twoDigits(now.time.jam) + ":" + twoDigits(now.time.minit) + ":"
//				+ twoDigits(now.time.saat))).c_str();
//		log_i("****************************************************************************** UTC :: %d", utc);
//		log_i("****************************************************************************** TaHUN :: %d", now.tahun);
		res = true;
	}
	return res;
}

void timing::toSecond() {
	unsigned int i;
	unsigned long temp;

	temp = 0;
	for (i = START_YEAR; i < now.tahun; i++) {
		temp += isleap(i) ? 366 : 365;
	}

	for (i = 0; i < now.bulan - 1; i++) {
		temp += days_month[isleap(now.tahun)][i];
	}
	temp += now.hari - 1;
	temp *= 86400;
	temp += now.time.jam * 3600;
	temp += now.time.minit * 60;
	temp += now.time.saat;

	utc = temp;
}
