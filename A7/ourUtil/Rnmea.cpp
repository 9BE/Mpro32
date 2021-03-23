/*
 * Rnmea.cpp
 *
 *  Created on: Dec 1, 2020
 *      Author: suhaimi
 */

#include <Rnmea.h>

Rnmea::Rnmea() {
	// TODO Auto-generated constructor stub
}

Rnmea::~Rnmea() {
	// TODO Auto-generated destructor stub
}

String Rnmea::toAISstd(String& snum, bool lat) {
	float x = snum.toFloat();
	int a = (int) x;
	float b = x-a;
	b *= 60;
	b /= 100;
	b += a;
	x = b*100;

	String ret = String(x,5);
	int patut = lat ? 4 : 5;
	int titik = ret.indexOf('.');
	if (titik < patut) {
		for (int z=titik; z<patut; z++) {
			ret = "0" + ret;
		}
	}


	return ret;

}

double Rnmea::toDegree(String snum) {
	float num = snum.toFloat();

	float temp = num / 100;
	int a = (int) temp;			//holding deg int
	float b = temp - a;			//holding minute
	b *= 100;
	b /= 60;
	b += a;

	return b;
}

String Rnmea::extractCol(String data, int index) {
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==',' || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void Rnmea::appendCheckSum(String& load) {
	int cs;
	String scs;
	char hh[12];

	if (load.indexOf('*') > 0) {
		load = load.substring(0, load.indexOf('*'));
	}

	cs = checkSum(load);
	scs = String(cs,HEX);
	if (scs.length() == 1) {
		scs = "0" + scs;
	}
	scs.toUpperCase();
	scs.toCharArray(hh, 3);
	load += "*";
	load += scs;
}

int Rnmea::checkSum(String& data) {
	int c = 0;
	char t;

	for(int i= 0; i < data.length(); i++){
		t = data.charAt(i);
		if(t == '!' || t == '$'){
			t = 0;
		}
//		if(t == '$'){
//			t = 1;
//		}
		c ^= t;
	}
    return c;
}


std::vector<double> Rnmea::exploade(String load, int arr[], int size) {
	char r;
	String panjang="";
	int start=0;
	String temp="";

	std::vector<double> jawapan;

	double decimal;

	for(int x=0; x < load.length(); x++){
		r = load.charAt(x) - 48;
		if(r > 40){
			r-=8;
		}
		panjang += char2bin(r,6);
	}

//	Serial.println(panjang);
	for(int y = 0; y < size; y++){
//		Serial.print(String(arr[y]));
		temp = panjang.substring(start,start+arr[y]);
		start += arr[y];
		temp.trim();
		decimal = 0;

	    int base = 1;
	    int len = temp.length();
	    for (int i = len - 1; i >= 0; i--) {
	        if (temp[i] == '1')
	        	decimal += base;
	        base = base * 2;
	    }

	    jawapan.push_back(decimal);
//		Serial.println(temp + " " + String(decimal,0));
	}
	return jawapan;
}

String Rnmea::char2bin(char z, char size) {
	String res="";

	for(int x=0; x < size; x++){
		(z % 2)?res = '1' + res:res = '0' + res;
		z/=2;


	}
	return res;
}

char Rnmea::sixChar(char data) {
	  if(data < 0x28){
	    data += 0x30;
	  }
	  else{
	    data += 0x38;
	  }
	  return data;
}


String Rnmea::compacte(const std::vector<NmeaData_t> &load) {

	int x,y;
	long titik;
	int nums, data;

	int base = 0;
	int offset = 0;
	char sixbits;
	char bitfill = 0;
	char last = 0;
	char fill;

	char buffer[1024];
	buffer[0] = 0;

	for(int q=0; q < load.size(); q++){
		nums = load.at(q).size;
		data = load.at(q).data;

		titik = pow(2,nums);

		x = strlen(buffer);

		for(y = x; y < x + nums; y++){
			data = data << 1;
			buffer[y] = 0x30;
			if(titik & data){
				buffer[y] |= 1;
			}
		}
		buffer[y] = 0x00;
	}

	if(String(buffer).length()>0){

		while(1){
			sixbits = 0x00;
			for(offset=0; offset < 6; offset++){
				if(buffer[(base*6)+offset] == 0x00) {
					bitfill = 6 - offset;
					last = 1;
					break;    //offset == 4
				}
				sixbits = sixbits << 1;
				if(buffer[(base*6)+offset] == '1'){
					sixbits |= 0x01;
				}
				else{
					sixbits &= 0xFE;
				}

				buffer[(base*6)+offset] = 'x';
			}
			if(last){
				if(bitfill){
					for(fill=0; fill<bitfill; fill++){
						sixbits = sixbits << 1;
					}
				}
			}
			sixbits = sixChar(sixbits);
			buffer[base] = sixbits;
			base ++;
			buffer[base] = 0x00;

			if(last) break;
		}

		if(bitfill == 6) bitfill = 0;
		buffer[base+0] = ',';
		buffer[base+1] = sixChar(bitfill);
		buffer[base+2] = 0x00;
	}

	return String(buffer);

}

bool Rnmea::validCheckSum(String word) {
	int f, resultCS=0;	//, x=0;
	String calculate="";
	bool result=false, pass=false;

	String testCS="";

	word.trim();
	String test = word.substring(1);
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N$GNRMC,070415.00,A,0305.37072,N,10132.77692,E,0.003,,260419,,,D*6C
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N$GNRMC,072014.00,A,0305.37058,N,10132.77695,E,0.003,,260419,,,D*64
	//!AIVDM,2,1,1,B,58155>p2>ktEKLhSL00<50F0I84pdhTp0000001?C`><=5!AIVDM,2,2,1,B,00000000000,2*26
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N<0LB@;Q3wjPl000,0*6D
	f = word.indexOf("*");
	if(f>=0 && pass==false){
		testCS = test.substring(f);
		test = test.substring(0,f-1);
		resultCS = checkSum(test);

		calculate = String(resultCS,HEX);
		calculate.toUpperCase();

		if(calculate.length()==1){
			calculate = "0" + calculate;
		}

		if(calculate == testCS){
			result = true;
		}
	}
	return result;
}

void Rnmea::addData(std::vector<NmeaData_t>& load, int data, int size) {
	NmeaData_t manData;
	manData.data = data;
	manData.size = size;
	load.push_back(manData);
}



