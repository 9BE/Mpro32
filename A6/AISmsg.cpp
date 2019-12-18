/*
 * AISmsg.cpp
 *
 *  Created on: Nov 5, 2019
 *      Author: suhaimi
 */

#include "AISmsg.h"

AIS_msg *iniAISmsg;
//String MFullTextAce = "0";
//String ProcAtonbit_prev = "XX";
//String ProcAtonbit_now;

AIS_msg::AIS_msg() {
	// TODO Auto-generated constructor stub
	iniAISmsg = this;
}

AIS_msg::~AIS_msg() {
	// TODO Auto-generated destructor stub
}

void AIS_msg::ConstructM21(String& MFullTextAce, String& ProcAtonbit_prev) {
	MFullTextAce.setCharAt(2, 'I');
	for (int i = 0; i < 2; i++) {
		MFullTextAce.setCharAt(17 + i, ProcAtonbit_prev[i]);
	}
	int bintang = MFullTextAce.indexOf('*');
	MFullTextAce.setCharAt(bintang - 1, 'C');
	checkSum(MFullTextAce);
}

/*
 * byte alarmX;
		byte lantern;
		byte racon;
		byte page = 7;
 */
void AIS_msg::makeAtonBit(String& ProcAtonbit_now, byte alarmX, byte lantern, byte racon, byte page) {

	byte semua;
	byte out[2];
	semua = alarmX;
	semua += lantern << 1;
	semua += racon << 3;
	semua += page << 5;

	out[0] = semua >> 4;
	out[1] = semua << 4;
	out[1] = out[1] >> 4;

	char baru;

	baru = out[0];
	baru = baru << 4;
	baru += out[1];

	//ProcAtonbit_prev compare with ProcAtonbit_now
	ProcAtonbit_now = String(baru, HEX);
	ProcAtonbit_now.toUpperCase();
}

float AIS_msg::getGps(String& location, const char * cardinal) {
	float x, temp;
	double ret;

	x = location.toFloat();
	temp = (x / 100); //3.05
	x = (int)temp;   //3
	temp = temp - x;
	temp = temp / 0.6;
	x = x + temp;

	if (!strcmp(cardinal, "S") || !strcmp(cardinal, "W")) {
		x = x * -1;
	}

	ret = x;
	return ret;
}

String AIS_msg::gpstoAISlatstandard(String& posisi) {
	float x;
	String ret;
	int dec;

	x = posisi.toFloat();
	if (x > 90.0) {
		x = 90;
	}
	dec = int(x);        // degree
	x = (x - dec) * 60;  // minute

	if (dec < 10){
		ret =   "0" + String(dec);
	}
	else  {
		ret =   String(dec);
	}

	if (x < 10)  {
		ret +=  "0" + String(x, 4);
	}
	else    {
		ret +=  String(x, 4);
	}
	return ret;
}

String AIS_msg::gpstoAISlngstandard(String& posisi) {
	float x;
	String ret;
	int dec;

	x = posisi.toFloat();
	if (x > 180.0) {
		x = 180;
	}
	dec = int(x);        // degree
	x = (x - dec) * 60;  // minute

	if (dec < 10) {
		ret =   "00" + String(dec);
	}
	else if (dec < 100) {
		ret =   "0" + String(dec);
	}
	else {
		ret =   String(dec);
	}
	if (x < 10) {
		ret +=  "0" + String(x, 4);
	}
	else {
		ret +=  String(x, 4);
	}
	return ret;
}

void AIS_msg::ConstructMsg06(char* memory, long dest, int channel, msg6Data &data, byte alarmX, byte lantern, byte racon, byte page, timing * tim, confData& ConfData) {

	int temp;
	char *p;
	char hh[12];

	memory[0] = 0x00;

	String ayy = ConfData.DAC;
	char lmao[5];
	ayy.toCharArray(lmao, sizeof(lmao));
	int hehe = atoi(lmao);

	iniAISmsg->message6bits(hehe, 10, memory);  //DAC // patutnya 533 A126: 235
	//Serial.print("DAC: "); Serial.println(hehe);

	ayy = ConfData.FI;
	ayy.toCharArray(lmao, sizeof(lmao));
	hehe = atoi(lmao);


	iniAISmsg->message6bits(hehe, 6, memory);     //FI // patutnya 4   A126

	hehe = ConfData.Beat.toInt();
	//Serial.print("FI: "); Serial.println(hehe);
	//dalam ni
	String caramba = ConfData.Format;
	if (caramba == "GF-LR-BUOY" || caramba == "GF-LR-BEACON" ) {
		temp = data.MVin * 20;                         //Aton V
		iniAISmsg->message6bits(temp, 10, memory);           //ADC1  ================================?? mando battery

		temp = data.LVin * 20;                         //Lantern V
		iniAISmsg->message6bits(temp, 10, memory);           //ADC2  ================================?? lantern battery

		temp = data.RVin * 20;                            //RACON V
		iniAISmsg->message6bits(temp, 10, memory);           //ADC3  ================================??


		iniAISmsg->message6bits(racon, 2, memory);           //RACON ================================??
		iniAISmsg->message6bits(lantern, 2, memory);         //LIGHT ================================??
		iniAISmsg->message6bits(alarmX, 1, memory);           //ALARM ================================??

//		char* end;
		//	long hehe = strtol(ConfData.Beat, &end, 10);


		//	if (*end) {
		//		hehe = 0;
		//	}
		iniAISmsg->message6bits(hehe, 1, memory);

		if ((10 <= data.LVin) & (data.LVin < 11) || (17 <= data.LVin) & (data.LVin < 22))       temp = 1; //bad
		else if ((11 <= data.LVin) & (data.LVin < 12) || (22 <= data.LVin) & (data.LVin < 24))  temp = 2; //low
		else if ((12 <=data.LVin) & (data.LVin < 16) || (24 <= data.LVin) & (data.LVin < 29))  temp = 3; //good
		else                                                  temp = 0; //unknown
		int battStat;
		battStat = temp;
		message6bits(temp, 2, memory);

		int Lamp = 0;		// malam tp secondary x menyala

		// Lamp : 0=noLight, 1=ON, 2=OFF, 3=Emergency

		if (data.LNyala) {
			// Primary nyala
			Lamp = 1;
		}
		else {
			if (tim->ZoneTime != 0) {		// siang gps
				Lamp = 0;
				if (data.LDRStatus == 1) {	// LDR dark
					if (data.SLNyala) {
						Lamp = 2;
					}else {
						Lamp = 3;
					}
				}
			}
			else {							// malam gps
				Lamp = 3;
				if (data.LDRStatus == 1) {	// LDR dark
					if (data.SLNyala) {
						Lamp = 2;
					}
				}
			}
		}

//		if (data.LNyala) {
//			// Primary nyala
//			Lamp = 1;
//		}
//		else {
//			// Primary x nyala
//			if (ConfData.Use_LDR == "Yes") {
//				if (data.LDRStatus == 3 || data.LDRStatus == 2) { 	// siang
//					Lamp = 0;
//				}
//				else if (data.SLNyala) {		// malam tp secondary menyala
//					Lamp = 2;
//				}
//			}
//			else {
//				if (tim->ZoneTime != 0) {
//					Lamp = 0;
//				}
//			}
//		}

//		if((!data.LNyala && (data.LDRStatus == 3 || data.LDRStatus == 2)) || (!data.LNyala && tim->ZoneTime != 0)) {
//			Lamp = 0;
//		}
//		else if (data.LNyala){
//			Lamp = 1;
//		}
//		else if (data.SLNyala)  {
//			Lamp = 2;
//		}
//		else  {
//			Lamp = 3;
//		}


		_debLamp = Lamp; 	// UTK DEBUG SEC LANTERN
		_debLDR = data.LDRStatus;

		temp = Lamp;
		iniAISmsg->message6bits(temp, 2, memory);

		temp = data.LDRStatus;
		iniAISmsg->message6bits(temp, 2, memory);

		temp = data.Door;
		iniAISmsg->message6bits(temp, 1, memory);            //1 //hatch door  //take reading from digital Door. masa construct msg baru readDigital

		temp = data.ProcOffPositionStatus;     //OffPosition
		iniAISmsg->message6bits(temp, 1, memory);

		temp = 0;
		iniAISmsg->message6bits(temp, 4, memory);

	}
	else if (caramba == "GF-LR-LIGHTHOUSE") {
		//kalau dapat data dari beacon sekalipun, proceed je la. logikal programming utk reject kalau lantern reader salah, terlalu leceh.
		temp = data.MVin * 10;                 //Aton V
		iniAISmsg->message6bits(temp, 9, memory);  //ADC1  ================================?? mando battery

		temp = data.LVin * 10;               //Lantern V
		iniAISmsg->message6bits(temp, 9, memory);

		temp = data.RVin * 10;               //RACON V
		iniAISmsg->message6bits(temp, 9, memory);

		temp = data.ProcOffPositionStatus;     //OffPosition
		iniAISmsg->message6bits(temp, 1, memory);

		temp = data.LDRStatus;               //Ambient
		iniAISmsg->message6bits(temp, 2, memory);

		iniAISmsg->message6bits(racon, 2, memory); //RACON ================================??
		iniAISmsg->message6bits(lantern, 2, memory); //LIGHT ================================??
		iniAISmsg->message6bits(alarmX, 1, memory); //ALARM ================================??

		iniAISmsg->message6bits(hehe, 1, memory);  //Tick/Tock

		//MK2000
		iniAISmsg->message6bits(data.MLCond, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.MLStat, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.SLCond, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.SLStat, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.ELCond, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.ELStat, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.D1Stat, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.D1Cond, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.D2Stat, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.D2Cond, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.Door, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.ACPower, 1, memory);  //Tick/Tock
		iniAISmsg->message6bits(data.BMS, 1, memory);  //Tick/Tock

	}
	else if (caramba == "GF-SC35") {
		temp = data.MVin * 20;                 //Aton V
		iniAISmsg->message6bits(temp, 9, memory);  //ADC1  ================================?? mando battery

		temp = data.LVin * 20;               //Lantern V
		iniAISmsg->message6bits(temp, 9, memory);  //ADC2  ================================?? lantern battery

		temp = data.LLedTemp * 5;           //LED Tempt
		iniAISmsg->message6bits(temp, 9, memory);  //ADC3  ================================??

		temp = data.ProcOffPositionStatus;     //OffPosition
		iniAISmsg->message6bits(temp, 1, memory);

		temp = data.LDRStatus;               //LDR status
		iniAISmsg->message6bits(temp, 2, memory);

		iniAISmsg->message6bits(racon, 2, memory); //RACON ================================??
		iniAISmsg->message6bits(lantern, 2, memory); //LIGHT ================================??
		iniAISmsg->message6bits(alarmX, 1, memory); //ALARM ================================??

		iniAISmsg->message6bits(hehe, 1, memory);

		iniAISmsg->message6bits(data.LAlarmActive, 1, memory);  //ok
		iniAISmsg->message6bits(data.LOffLedPowThres, 1, memory); //ok
		iniAISmsg->message6bits(data.LOffLowVin, 1, memory);    //ok
		iniAISmsg->message6bits(data.LOffLDR, 1, memory);       //ok
		iniAISmsg->message6bits(data.LOffTemp, 1, memory);      //ok
		iniAISmsg->message6bits(data.LOffForce, 1, memory);     //ok
		iniAISmsg->message6bits(data.LIsNight, 1, memory);      //ok
		iniAISmsg->message6bits(data.LErrLedShort, 1, memory);  //ok
		iniAISmsg->message6bits(data.LErrLedOpen, 1, memory);   //ok
		iniAISmsg->message6bits(data.LErrLedVLow, 1, memory);   //ok
		iniAISmsg->message6bits(data.LErrVinLow, 1, memory);    //ok
		iniAISmsg->message6bits(data.LErrLedPowThres, 1, memory); //ok
		iniAISmsg->message6bits(data.LLEDAdjMaxAvgPow, 1, memory); //ok
		iniAISmsg->message6bits(data.LGsenIntOccur, 1, memory); //ok
		iniAISmsg->message6bits(data.LSolarChargingOn, 1, memory); //ok
	}
	else if (caramba == "A126-SC35") {
		temp = data.MVin * 20;                         //Aton V
		message6bits(temp, 10, memory);           //ADC1  ================================?? mando battery

		temp = data.LVin * 20;                         //Lantern V
		message6bits(temp, 10, memory);           //ADC2  ================================?? lantern battery

		temp = 0 * 10;                            //RACON V
		message6bits(temp, 10, memory);           //ADC3  ================================??

		iniAISmsg->message6bits(racon, 2, memory);           //RACON ================================??
		iniAISmsg->message6bits(lantern, 2, memory);         //LIGHT ================================??
		iniAISmsg->message6bits(alarmX, 1, memory);           //ALARM ================================??

		iniAISmsg->message6bits(hehe, 1, memory);            //1
		iniAISmsg->message6bits(data.LAlarmActive, 1, memory);    //2
		iniAISmsg->message6bits(data.LOffLowVin, 1, memory);      //3
		iniAISmsg->message6bits(data.LOffLDR, 1, memory);         //4
		iniAISmsg->message6bits(data.LOffTemp, 1, memory);        //5
		iniAISmsg->message6bits(data.LOffForce, 1, memory);       //6
		iniAISmsg->message6bits(data.LGsenIntOccur, 1, memory);   //7
		iniAISmsg->message6bits(data.LSolarChargingOn, 1, memory);//8

		temp = data.ProcOffPositionStatus;     //OffPosition
		message6bits(temp, 1, memory);

		temp = 0;
		message6bits(temp, 4, memory);
	}
	genEncData(memory);

	p = &memory[512];
	strcpy(p, memory);

	strcpy(memory, "!AIABM,1,1,0,");
	ltoa(dest, hh, 10);      //pembetulan
	strcat(memory, hh);
	strcat(memory, ",");

	ltoa(channel, hh, 10);
	strcat(memory, hh);

	strcat(memory, ",6,");

	strcat(memory , p);
	strcat(memory, "*");
}

void AIS_msg::decodeM21(String myText, int & ProcOffPositionStatus) {
	String binary21 = "";
	for (int i = 0; i < myText.length(); i++) {
		char myChar = myText.charAt(i);
		myChar -= 48;

		if (myChar > 40) {
			myChar -= 8;
		}

		for (int i = 5; i >= 0; i--) {
			byte bytes = bitRead(myChar, i);
			String temp = String(bytes, BIN);
			binary21 += temp;
			//      Serial.print(bytes, BIN);
		}
	}
	ProcOff_pos = binary21[259];

	ProcOffPositionStatus = ProcOff_pos.toInt();
}

void AIS_msg::message6bits(long data, char nums, char* memory) {
	int x, y;
	long titik;

	titik = pow(2, nums);

	x = strlen(memory);

	for (y = x; y < x + nums; y++) {
		data = data << 1;
		memory[y] = 0x30;
		if (titik & data) {
			memory[y] |= 1;
		}
	}
	memory[y] = 0x00;
}

void AIS_msg::genEncData(char* memory) {
	int base = 0;
	int offset = 0;
	char sixbits;
	char bitfill = 0;
	char last = 0;
	char fill;

	while (1) {
		sixbits = 0x00;
		for (offset = 0; offset < 6; offset++) {
			if (memory[(base * 6) + offset] == 0x00) {
				bitfill = 6 - offset;
				last = 1;
				break;    //offset == 4
			}
			sixbits = sixbits << 1;
			if (memory[(base * 6) + offset] == '1') {
				sixbits |= 0x01;
			}
			else {
				sixbits &= 0xFE;
			}

			memory[(base * 6) + offset] = 'x';
		}
		if (last) {
				if (bitfill) {
					for (fill = 0; fill < bitfill; fill++) {
						sixbits = sixbits << 1;
					}
				}
		}
		sixbits = sixChar(sixbits);
		memory[base] = sixbits;
		base ++;
		memory[base] = 0x00;

		if (last) break;
	}
	if (bitfill == 6) bitfill = 0;
	memory[base + 0] = ',';
	memory[base + 1] = sixChar(bitfill);
	memory[base + 2] = 0x00;
}

void AIS_msg::checkSum(String& mesej) {
	int start_with = 0;
	int end_with = 0;
	int CRC = 0;


	for (int index = 0; index < 100; index++) {
		if ( mesej[index] == '$')   start_with = index;
		else if ( mesej[index] == '*')    end_with = index;
	}


	for (int x = start_with + 1; x < end_with; x++) { // XOR every character in between '$' and '*'
		CRC ^= mesej[x];
	}
	String cs = String(CRC, HEX);
	cs.toUpperCase();
	if (cs.length() == 1) {
		cs = "0" + cs;
	}
	for (int i = end_with + 1; i < end_with + 3; i++) {
		mesej.setCharAt(i, cs[i - end_with - 1]);
	}
	mesej = mesej.substring(start_with, end_with + 3);

	CRC = 0;

}

String AIS_msg::getPara(String& data, char index) {
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length();
	char separator = ',';

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool AIS_msg::validate(String& word) {
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
		char t;

		for(int i= 0; i < test.length(); i++){
			t = test.charAt(i);
			if(t == '!'){
				t = 0;
			}
			if(t == '$'){
				t = 1;
			}
			resultCS ^= t;
		}
//		resultCS = iniAISmsg->checkSum(test);

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

char AIS_msg::sixChar(char data) {
	if (data < 0x28) {
		data += 0x30;
	}
	else {
		data += 0x38;
	}
	return data;
}


