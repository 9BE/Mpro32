/*
 * DataHandler.h
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#ifndef OURS_DATAHANDLER_H_
#define OURS_DATAHANDLER_H_

#include "Arduino.h"
#include "Ntiming.h"
#include "Rnmea.h"

enum {
	lrs_booting,
	lrs_calibrating,
	lrs_processing,
	lrs_locked
};


enum {
	f_gf_sc35,
	f_a126_sc35,
	f_lrBuoy,
	f_lrBeacon,
	f_lrLH,
	f_sc35ada
};

enum {
	wdt_mando,
	wdt_measureAton,
	wdt_measureLantern,
	wdt_measureRacon,
	wdt_detect,
	wdt_lamp,
	wdt_oled,
	wdt_aso

};

typedef struct {
	byte alarmX = 0;
	byte lantern = 2;
	byte racon = 0;
	byte page = 7;

} AtonBit_t;

typedef struct {
	String name = "unknown";
	String id = "m1m2m3m4";
	String prog = "";
} MachineData_t;

enum {
	ldr_no,
	ldr_dark,
	ldr_dim,
	ldr_bright
};

enum {
	door_close,
	door_open
};

enum {
	rac_noRac,
	rac_noMonitor,
	rac_operational,
	rac_error,
	rac_NO,
	rac_NC,
	rac_contact,
	rac_xContact
};

typedef struct {

	float LVin = 0;
	float RVin = 0;
	float ELVin = 0;

	int LDRStatus = 0;
	int Door = 0;

	int LNyala = 0;
	int SLNyala = 0;
	int ELNyala = 0;

	float PrimMaxMinDiff;
	float Primthresholdamp;

	float SecMaxMinDiff;
	float Secthresholdamp;

	int Rcontact = 0;

	// Lighthouse
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
	bool PSU = false;
	bool Xtra1 = false;
	bool Xtra2 = false;





} LRData_t;

typedef struct {
	uint32_t masaUpdate = 0;
	float Vin = 0;             // 4,7
	int Nyala = 0;             // 4,9,kanan
	int LDRStatus = 0;
	int LedTemp=0;           // 4,11,kanan.
	int PhotoCurrentValue = 0; // 4,6
	int AlarmActive=0;       // 4,17,bit0
	int OffLedPowThres=0;    // 4,17,bit1
	int OffLowVin=0;         // 4,17,bit2
	int OffLDR=0;            // 4.17,bit3
	int OffTemp=0;           // 4,17,bit5
	int OffForce=0;          // 4,17,bit7
	int ErrLedShort=0;       // 4,17,bit8
	int ErrLedOpen=0;        // 4,17,bit9
	int ErrLedVLow=0;        // 4,17,bit10
	int ErrVinLow=0;         // 4,17,bit11
	int ErrLedPowThres=0;    // 4,17,bit12
	int LEDAdjMaxAvgPow=0;   // 4,17,bit13
	int GsenIntOccur=0;      // 4,17,bit14
	int SolarChargingOn=0;   // 4,17,bit15
	int IsNight;
}SC35data_t;

typedef struct {
	float MVin = 0;
	float LVin = 0;             // 4,7
	float RVin = 0;
	int ProcOffPositionStatus = 0;

	//atonbit kt sini
	//beat kt sini
	int Beat = 0;

	int LDRStatus = 0;
	int LNyala = 0;             // 4,9,kanan
	int SLNyala = 0;
	bool Door = false;



	// Lighthouse
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



	//sc35
	int LLedTemp=0;           // 4,11,kanan.
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

	// lain2
	int Racon = 0;


} Msg6Data_t;

typedef struct {
	String MName = "NDA";
	String MMmsi = "NDA";           //9digit + 1null character
	String MAtonType = "NDA";        //2char + 1 null character
	String MLengthA = "NDA";
	String MLengthB = "NDA";
	String MLengthC = "NDA";
	String MLengthD = "NDA";
	String MOffThres = "NDA";
	String MEpfs  = "NDA";            //1char + 1 null character
	String MPosAcc = "NDA";
	String MConfigLat = "NDA";
	String MRegCardinalLat = "NDA";  //1char + 1 null character
	String MConfigLng = "NDA";
	String MRegCardinalLng = "NDA";  //1char + 1 null character
	String M06UTChourA = "NDA";
	String M06UTCminA = "NDA";
	uint32_t M06IntervalA = 0;
	String M06UTChourB = "NDA";
	String M06UTCminB = "NDA";
	uint32_t M06IntervalB = 0;
	String M21UTChourA = "NDA";
	String M21UTCminA = "NDA";
	uint32_t M21IntervalA = 0;
	String M21UTChourB = "NDA";
	String M21UTCminB = "NDA";
	uint32_t M21IntervalB = 0;

	String MSerialNumber = "NDA";
} MandoConfig_t;

typedef struct {
	uint8_t Beat = 1;
	String DAC = "533";
	String FI = "1";
	String Dest_ID = "701";
	double currentThres = 0.8;
	int flashGap = 10;
	String RACON_Mon = "No";
	String LANT_Mon = "Local";

	String Reboot_Mode;
	String Light_Detect_Method;
	int Format = 1;

//	String UTC_Offset ;
	String Sec_Mon;
//	String Lantern_Lost;
	String Use_LDR;
	String Calibrate;
	String Calib_Prim_M;
	String Calib_Sec_M;

} SpiffsData_t;



class DataHandler {

private:


public:
	DataHandler();
	void LRactivate(uint8_t index);
	bool isLRactive(uint8_t index);
	void savToSpiffs();

	void procesAtonBit();
	bool AtonBitLainTak();
	String& nakMsg21(Rnmea * manNMEA);
	String nakMsg6(Rnmea * manNMEA, long dest, int channel, uint8_t msgSeq);

	String procAtonBit_prev = "NDA";
	String procAtonBit_now = "XX";

	std::vector<NmeaData_t> nmeaData;

	MachineData_t MachineData;
	SpiffsData_t SpiffsData;
	Msg6Data_t Msg6Data;
	MandoConfig_t MandoConf;
	AtonBit_t AtonBitData;

	LRData_t LreaderData;
//	LRData_t BuoyData;
//	LRData_t BeaconData;

	SC35data_t SC35LiveData;
	SC35data_t SC35DefData;

	uint8_t LRactiv = -1;
	int LRstat = -1;
	int LRemcStat = -1;
	String LRlongStat = "";




	// GUNA DALAM MANDO.CPP
	Ntiming * nTiming;
	bool _nakConfigMando = false;
	bool _nakSavSpiffs = false;
	bool _configuringMando = false;
	uint8_t _confProgress = 0;
	uint8_t _conFull = 4;
	bool _mandoJaga = false;
	bool _jumpaGPS = false;
	bool _xTaskComplete = false;
	double _LAT = 2.940236;
	double _LNG = 101.339294;
	String fullACE = "";
	String _header = "";
	uint32_t _masaVdo21 = 0;
	uint32_t _masaVdo6 = 0;
	uint32_t _masaSerial = 0;






};

#endif /* OURS_DATAHANDLER_H_ */
