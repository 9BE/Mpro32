/*
 * Data.h
 *
 *  Created on: Nov 8, 2019
 *      Author: suhaimi
 */

#ifndef DATA_H_
#define DATA_H_

struct machineData {
	String id = "";
	String name = "";
};

struct mandoConfig {
	String MName = "0";
	String MMmsi = "0";           //9digit + 1null character
	char MAtonType[3] = "0";        //2char + 1 null character
	String MLengthA = "0";
	String MLengthB = "0";
	String MLengthC = "0";
	String MLengthD = "0";
	int MOffThres = 0;
	char MEpfs[2]  = "0";            //1char + 1 null character
	char MPosAcc[2] = "0";
	String MConfigLat = "0";
	char MRegCardinalLat[2] = "0";  //1char + 1 null character
	String MConfigLng = "0";
	char MRegCardinalLng[2] = "0";  //1char + 1 null character
	String M06UTChourA = "0";
	String M06UTCminA = "0";
	String M06IntervalABahagi60 = "0";
	String M06UTChourB = "0";
	String M06UTCminB = "0";
	String M06IntervalBBahagi60 = "0";
	String M21UTChourA = "0";
	String M21UTCminA = "0";
	String M21IntervalABahagi60 = "0";
	String M21UTChourB = "0";
	String M21UTCminB = "0";
	String M21IntervalBBahagi60 = "0";
};

struct msg6Data {
	float MVin;
	float LVin;             // 4,7
	float RVin = 0;
	int ProcOffPositionStatus = 0;
	//atonbit kt sini
	//beat kt sini
	int LDRStatus = 0;
	int LNyala=0;             // 4,9,kanan
	int SLNyala = 0;
	bool Door = false;
	bool MLCond = false;
	bool MLStat = false;
	bool SLCond = false;
	bool SLStat = false;
	bool ELCond = false;
	bool ELStat = false;
	bool D1Cond = false;
	bool D1Stat= false;
	bool D2Cond = false;
	bool D2Stat = false;
	bool ACPower = false;
	bool BMS = false;
	int LLedTemp=0;           // 4,11,kanan.


	//sc35
	int LPhotoCurrentValue = 0; // 4,6
	int LAlarmActive=0;       // 4,17,bit0
	int LOffLedPowThres=0;    // 4,17,bit1
	int LOffLowVin=0;         // 4,17,bit2
	int LOffLDR=0;            // 4.17,bit3
	int LOffTemp=0;           // 4,17,bit5
	int LOffForce=0;          // 4,17,bit7
	int LErrLedShort=0;       // 4,17,bit8
	int LErrLedOpen=0;        // 4,17,bit9
	int LErrLedVLow=0;        // 4,17,bit10
	int LErrVinLow=0;         // 4,17,bit11
	int LErrLedPowThres=0;    // 4,17,bit12
	int LLEDAdjMaxAvgPow=0;   // 4,17,bit13
	int LGsenIntOccur=0;      // 4,17,bit14
	int LSolarChargingOn=0;   // 4,17,bit15
	int LIsNight;



};

struct confData {
	String Beat;
	String Reboot_Mode;
	String Light_Detect_Method;
	String Format;
	String DAC;
	String FI;
	String Dest_ID;
	String UTC_Offset;
	String Sec_Mon;
	String Lantern_Lost;
	String Use_LDR;
	String Calibrate;
	String Calib_Prim_M;
	String Calib_Sec_M;
	String RACON_Mon;
};






#endif /* DATA_H_ */
