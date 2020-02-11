#include "Arduino.h"
#include "LReader.h"
#include "Mando.h"
#include "Lantern.h"
#include "JsonHandler.h"
#include "LocSpiff.h"
#include "ServerTempatan.h"
#include <driver/adc.h>
#include "timing.h"
#include "board.h"
#include "LocWiFi.h"
#include "esp_task_wdt.h"

extern "C" int rom_phy_get_vdd33();

LReader * _oLreader;
Lantern * _oLantern;
Mando * _oMando;
ServerTempatan * _oServer;
timing * _oTiming;
LocWiFi		*locWiFi;

uint64_t masaLantern = 0;
uint64_t masaPower = 0;
uint64_t masaTestMando = 0;
uint64_t masaReboot= 0;

int xValWiFi = 0;

//averaging
float MVinX = 15;
float adcBattery = 0;
float _vcc = 0;

String alert = "Booting controller";
String binFile;
bool jumpaAIS = true;
bool powerOK = false;
bool tungguLantern = false;
bool tungguVDO = false;

int txPowerRangers = 78;

uint8_t malam = 1;

// *************************** PROTOTYPING *************************************

void setupSPIFFiles(bool freshSetup);

void mainPower();
float rollAverage(float prev, float newData, float roll);
void espReboot();
void mainLampu(uint16_t berapaKali, uint64_t delayTime);
void urusAlert();
void urusConfig();
void urusAIS();
void getVcc();


//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	Serial.begin(921600);
	Serial1.begin(2400, SERIAL_8N1, RX2, TX2);    //begin serial for rs485
	Serial2.begin(9600, SERIAL_8N1, RX3, TX3);    //begin serial for rs232

	pinMode(OUT_DR, OUTPUT);            //set pin driver/receiver enable for rs485 as output
	pinMode(LED_PIN, OUTPUT);

	digitalWrite(LED_PIN, HIGH);
//	mainLampu(5, 500);
	log_i("\n\n\n\nSalam Dunia dari %s\n\n\n\n", __FILE__);
	log_i("MULAI");

	binFile = __FILE__;
	binFile = binFile.substring(3, binFile.length());
	binFile = binFile.substring(0, binFile.length()-4);
	binFile += "_3 (beta)";

	log_i("binFile :: %s\n", binFile.c_str());

	getVcc();					// Get internal VCC

	_oTiming = new timing();
	_oMando = new Mando(1, 50, _oTiming);
	_oLantern = new Lantern(1, 50, _oTiming, _oMando);
	_oLreader = new LReader(_oLantern, _oMando);

	_oMando->SpiffsData.Beat = "2";


	setupSPIFFiles(false);

	xValWiFi = lw_wifi_apsta;
	locWiFi = new LocWiFi(0,60000, &xValWiFi);


	delay(1000);

	_oServer = new ServerTempatan(0, 100, _oMando, _oLantern, _oLreader, _oTiming);
	_oServer->setBinFile(binFile);

//	mainLampu(3, 500);
	digitalWrite(LED_PIN, LOW);

//	WiFi.setTxPower(WIFI_POWER_19_5dBm); // test kene buang utk sebenar

	if (!_oMando->isMulai()) {
		_oMando->setMulai(true);
	}

	if (!_oLantern->isMulai()) {
		_oLantern->setMulai(true);
	}

	_oMando->_masaMando = millis();

	esp_task_wdt_init(180, true);
	enableLoopWDT();
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SIMULATOR START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// key '3' = mando
	// key '5' = lantern
//	char cr;
//	if (Serial.available()) {
//		while (Serial.available()) {
//			cr = Serial.read();
//			if (cr == '3') {
//				_oMando->setMandoTaskStat(_oMando->getMandoTaskStat() + 1);
//				_oMando->setMandoTask(MandoTask (_oMando->getMandoTaskStat() + 1));
//			}else if (cr == '5') {
//				_oLantern->setLanternTaskStat(3);
//			}else if (cr == '9') {
//				txPowerRangers++;
//				WiFi.setTxPower((wifi_power_t)txPowerRangers);
//				log_i("Setting tx power to >> %d", txPowerRangers);
//			}else if (cr == '0') {
//				txPowerRangers--;
//				WiFi.setTxPower((wifi_power_t)txPowerRangers);
//				log_i("Setting tx power to >> %d", txPowerRangers);
//			}
//		}
//	}
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SIMULATOR END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	// ==> TIMING
	if (_oTiming->tickTock()) {
		_oTiming->susunMasa(_oMando->lat, _oMando->lng);
		mainLampu(1, 25);
//		log_i("  ticTock :::::::::: %d", millis()/1000);
//		log_i(" Mando task ::::: %d", _oMando->getMandoTaskStat());
//		log_i("Lantern task :: %d", _oLantern->getLanternTaskStat());
//		log_i("TX POWER ************************** %d", (int) WiFi.getTxPower());
		if (_oTiming->ZoneTime == e_nite) {
			// malam
			if (!malam) {
				malam = 1;
				WiFi.setTxPower(WIFI_POWER_7dBm);
			}
		}else if (_oTiming->ZoneTime == e_day) {
			if (malam) {
				malam = 0;
				WiFi.setTxPower(WIFI_POWER_19_5dBm);
			}
		}

	}

	// ==> MAIN POWER
	if (millis() - masaPower >= 50) {
		masaPower = millis();
		mainPower();
	}

	// ==> SET ALERT
	int MinConnection = 0;
	if (_oMando->SpiffsData.Format == "GF-LR-LIGHTHOUSE") {
		if (_oLantern->lanternlockB & _oLantern->lanternlockC)
			MinConnection = 3;
		else if ((_oLantern->lanternlockB && !_oLantern->lanternlockC) | (!_oLantern->lanternlockB && _oLantern->lanternlockC))
			MinConnection = 1;
		else
			MinConnection = 2;
	}
	else {
		if (_oLantern->jumpaLR)
			MinConnection = 2;
		else
			MinConnection = 1;
	}
	if (WiFi.softAPgetStationNum() >= MinConnection || WiFi.isConnected()) {
		urusAlert();
	}

	// ==> CONFIGURE
	if (_oMando->isNakConfig()) {
		urusConfig();
	}
	else {
		urusAIS();
	}



	// ==> BIIT
	if (millis() - masaReboot >= 3600000) {
		_oMando->setMulai(false);
		_oLantern->setMulai(false);
		delay(3000);
		// repeat;
		espReboot();
		delay(3000);
		ESP.restart();


//		if (millis() - _oMando->getMasaNmea() >= 3600000) {
//			ESP.restart();
//			delay(10);
//		}
//
//		_oMando->reInit();
//		_oLantern->reInit();
//		_oMando->setMulai(true);
//		_oLantern->setMulai(true);
//		masaReboot = millis();

	}

	feedLoopWDT();


	delay(50);
}

inline void getVcc() {
	btStart();

	int internalBatReading = rom_phy_get_vdd33();
	Serial.println(internalBatReading);
	_vcc = (float)(((uint32_t)internalBatReading*3.288)/6491);
	log_i("Vin ????????????????????? === %f", _vcc);
	btStop();

	if (_vcc > 3.29) {
		_vcc = 3;
	}else if (_vcc < 3.19){
		_vcc = 3.6;
	}
//	_vcc = 3.6;
}


void mainLampu(uint16_t berapaKali, uint64_t delayTime) {
	for (int i=0; i<berapaKali; i++) {
		digitalWrite(LED_PIN, HIGH);
		delay(delayTime);
		digitalWrite(LED_PIN, LOW);
		delay(delayTime);
	}
}

void mainPower() {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
	_oMando->rawBattery = adc1_get_raw(ADC1_CHANNEL_0);
//	log_i("MVin raw === %g", _oMando->rawBattery);
	adcBattery = rollAverage(adcBattery, _oMando->rawBattery, 100);

	_oMando->M6data.MVin = adcBattery/4095;
	_oMando->M6data.MVin *= _vcc;

	_oMando->M6data.MVin = _oMando->M6data.MVin * 22.6309669918;
	_oMando->M6data.MVin += 3.3371091445;
//	log_i("MVin voltage >>>>>>>>>>>>>>>>>>> %g", _oMando->M6data.MVin);

//	_oMando->M6data.MVin = adcBattery * 0.0147229551;
//	_oMando->M6data.MVin += 4.0259366755;
//	_oMando->M6data.MVin = adcBattery / 560;
//	_oMando->M6data.MVin *= 13.12;

	if (_oMando->M6data.MVin > MVinX) {
		powerOK = true;
//		log_i("**********************************************************************************************");
//		log_i("**********************************************************************************************");
//		log_i("************************************** POWER OK at %d **************************************", millis()/1000);
//		log_i("************************************** %f **************************************", _oMando->M6data.MVin);
//		log_i("**********************************************************************************************");
//		log_i("**********************************************************************************************");

	}else {
		powerOK = false;
		MVinX -= 0.01;

	}
}

float rollAverage(float prev, float newData, float roll)
{
	float X;
	X = prev * roll;
	X -= prev;
	X += newData;
	X /= roll;
	return X;
}

inline void setupSPIFFiles(bool freshSetup) {
	//aza
	LocSpiff 	*locSpiff;
	JsonHandler *jsonHandler;
	FileInfo_t info;

	locSpiff = new LocSpiff("setupSPIFFiles");
	jsonHandler = new JsonHandler(_oMando, "setupSPIFFiles");

//	locSpiff->format(); 	// ! ! ! ! ! ! ! ! ! Danger ! ! ! ! ! ! ! ! ! !

	locSpiff->listAllFiles();

	delay(50);

	String jData = locSpiff->readFile("/config.json");
	log_i("jData :: %s", jData.c_str());
	jData = jsonHandler->checkConfigValue(jData, freshSetup);
	if (jData == "OK") {
		log_i("configValue OK");
	}else {
		log_i("writing new config.json");
		locSpiff->writeFile("/config.json", jData.c_str());
	}

	//Setup SSID----------------------------------------------------------------
	info = locSpiff->getInfo("/ssid.txt");
	if(!freshSetup && info.filename == "/ssid.txt"){
		log_i("info /ssid.txt = EXIST");
		log_i("Exist");
	}
	else{
		log_i("Not Exist - Creating default credentials");
		locSpiff->deleteFile("/ssid.txt");
		locSpiff->appendFile("/ssid.txt", "sta,ideapad,sawabatik1\n");
		locSpiff->appendFile("/ssid.txt", "sta,AndroidAP,efdx6532\n");
		locSpiff->appendFile("/ssid.txt", "sta,GF_Wifi_2.4GHz,Gr33nF1nd3r2018\n");
//		locSpiff->appendFile("/ssid.txt", "ap,TestZippy,123qweasd\n");
		locSpiff->appendFile("/ssid.txt", "ap,GreenFinderIOT,0xadezcsw1\n");
	}

	info = locSpiff->getInfo("/n.txt");
	if(info.filename == "/n.txt" && !freshSetup){
		log_i("info /n.txt = EXIST");
	}
	else{
		log_i("Not Exist - Creating default mName");
		locSpiff->writeFile("/n.txt", "MandoPro Test");
	}

	info = locSpiff->getInfo("/m.txt");
	if(info.filename == "/m.txt" && !freshSetup){
		log_i("info /m.txt = EXIST");
	}
	else{
		log_i("Not Exist - Creating default mId");
		locSpiff->writeFile("/m.txt", "XXXXXX");
	}

	delay(10);

	_oMando->_machine.id = locSpiff->readFile("/m.txt");
	_oMando->_machine.name = locSpiff->readFile("/n.txt");

	delay(10);

	delete locSpiff;

	delay(1000);


	jsonHandler->extractConfData();

	delay(1000);

	delete jsonHandler;

	log_i("Beat >>>>>>>>>>>>>>>>>> setupSPIFFiles >>>>>>>>>>>>>>>>>>>>>>> %s", _oMando->SpiffsData.Beat.c_str());


}

inline void espReboot() {
	JsonHandler *jsonHandler;
	LocSpiff 	*locSpiff;

	locSpiff = new LocSpiff("espReboot");


	if (_oMando->_machine.id != locSpiff->readFile("/m.txt")) {
		locSpiff->writeFile("/m.txt", _oMando->_machine.id.c_str());
	}
	delay(1000);
	if (_oMando->_machine.name = locSpiff->readFile("/n.txt")) {
		locSpiff->writeFile("/n.txt", _oMando->_machine.name.c_str());
	}
	delay(1000);
	delete locSpiff;

	delay(1000);

	jsonHandler = new JsonHandler(_oMando);

	jsonHandler->simpanConfData();

	delay(1000);

	delete jsonHandler;

//	ESP.restart();
}

inline void urusAlert() {
	// ==> SET ALERT
	if (!jumpaAIS) {
		alert = "AIS device not found. Please connect the device.";
	}else {
		alert = "Query AIS device";
		if (_oMando->getMandoTaskStat() == mt_none) {
			if (millis() > 60000) {
				alert = "AIS information not found. Please configure the device.";
			}
		}
		else if (tungguVDO) {
			alert = "Ready to send M6. \n Waiting for VDO21";
		}
		else if (tungguLantern) {
			alert = "Ready to send M6. \n Waiting for lantern respond";
		}
		else {
			if (_oMando->getMandoTaskStat() == AAR21) {
				alert = "Searching GPS";
			}
			else if (_oMando->getMandoTaskStat() == RMC) {
				alert = "Found GPS";
			}
			else if (_oMando->getMandoTaskStat() == VDO21) {
				alert = "Found VDO 21";
			}
			else if (_oMando->getMandoTaskStat() == SendBIT || _oMando->getMandoTaskStat() == Send06) {
				alert = "Updating AtoN Bit & sending M6";
			}
		}

	}
	// ==> SET ALERT TO SERVER #### MUST DO
	_oServer->setAlert(alert);
}

inline void urusConfig() {
	_oLantern->setMulai(false);

	alert = "Configuring AIS device (";
	alert += _oMando->_confProgress;
	alert += "/";
	alert += _oMando->_confFull;
	alert += ")";

	log_i("%s", alert.c_str());

	_oServer->setAlert(alert);

	if (_oMando->_confProgress > 0 && (_oMando->_confFull / _oMando->_confProgress == 0)) {
		alert = "Completing the configuration";
		_oServer->setAlert(alert);
	}

	if ((_oMando->getMandoTaskStat() <= RMC) && (_oMando->getMandoTaskStat() >= AAR06)) {
		alert = "Configuration AIS device completed";
		log_i("%s", alert.c_str());
		_oServer->setAlert(alert);
		delay(1000);
		_oMando->setNakConfig(false);
		_oLantern->reInit();
		_oLantern->setMulai(true);
	}

	_oMando->_masaMando = millis();
}

inline void urusAIS() {
	// ==> CHECK AIS DEVICE AVAILABILITY

	if (millis() - _oMando->getMasaNmea() >= 60000) {
		_oServer->setAppCommPort("Not found");
		jumpaAIS = false;
		_oMando->reInit();
	}
	else {
		_oServer->setAppCommPort("Found");
		jumpaAIS = true;
	}

//	if (jumpaAIS) {
//		if (_oLantern->getLanternTaskStat() == lt_Decision ) {
//			_oLantern->setMulai(false);
//			_oMando->checkAtonBit();
//			delay(100);
//			if (_oMando->_inputMandoVdo21 && powerOK) {
//				_oMando->hantarM6();
//
//				long hantar=0;
//				while(true){
//					hantar ++;
//					if(_oMando->getMandoTaskStat() == ABK06){
//						break;
//					}
//					if(hantar > 1000){
//						break;
//					}
//					delay(10);
//				}
//
//				_oMando->agoM6 = millis();
//				_oMando->lastM06 = _oTiming->now.sMasa.c_str();
//				_oMando->_inputMandoVdo06 = true;
//				_oMando->_masaMando = millis();
//
//				alert = "Message 6 sent";
//				_oServer->setAlert(alert);
//
//				mainLampu(1, 250);
//			}
//
//			_oMando->sambung();
//			_oLantern->reInit();
//			_oLantern->setMulai(true);
//
//		}
//	}

//	if (_oLantern->getLanternTaskStat() == lt_Decision ) {
//		_oLantern->setMulai(false);
//		_oMando->checkAtonBit();
//		delay(15000);
//		_oMando->sambung();
//		_oLantern->reInit();
//		_oLantern->setMulai(true);
//	}

//	 ==> SEND MSG 6
	if (jumpaAIS && millis() - _oMando->_masaMando >= 60000) { //600000 // 180000
		tungguVDO = true;
		if (_oMando->_inputMandoVdo21 && powerOK) {
			tungguLantern = true;
			tungguVDO = false;
			if (_oLantern->getLanternTaskStat() == lt_Decision ) { // && _oMando->_inputMandoVdo21 == true
				tungguLantern = false;
				_oLantern->setMulai(false);
				_oMando->checkAtonBit();

				long hantar=0;
				while(true){
					hantar ++;
					if(_oMando->getMandoTaskStat() == ABK06){
						mainLampu(2, 250);
						break;
					}
					if(hantar > 1000){
						break;
					}
					delay(10);
				}

				_oMando->agoM6 = millis();
				_oMando->lastM06 = _oTiming->now.sMasa.c_str();
				_oMando->_inputMandoVdo06 = true;
				_oMando->_masaMando = millis();

				alert = "Message 6 sent";
				_oServer->setAlert(alert);



				_oMando->sambung();
				_oLantern->reInit();
				_oLantern->setMulai(true);

			}
		}
	}
}

