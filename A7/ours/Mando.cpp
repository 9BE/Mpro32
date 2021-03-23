/*
 * Mando.cpp
 *
 *  Created on: Jan 4, 2021
 *      Author: suhaimi
 */

#include <Mando.h>



Mando::Mando(HardwareSerial * serial, DataHandler * ndata) {
	this->_serial = serial;
	this->_nData = ndata;
	this->_nmea = new Rnmea();
	this->_xTask = AAR;

	xTaskCreatePinnedToCore(this->loop, "Mando", 5000, this, 1, &this->_handle, 1);
}

void Mando::loop(void * param) {
	Mando * ini = (Mando*) param;

	N_ticker * tickArahan = new N_ticker(10000,1000);
	N_ticker * tickMessage = new N_ticker(60000, 0);
	char cr;

	while (1) {
		ini->_nData->nTiming->ticktock();

		if (ini->_nData->_nakConfigMando) {
			ini->_nData->_nakConfigMando = false;
			ini->_nData->_configuringMando = true;
			ini->_xTask = cAID1;
			ini->_nData->_confProgress = 0;
		}

		if (ini->_nData->_configuringMando && ini->_xTask == mt_none) {
			ini->_nData->_configuringMando = false;
		}

		if(tickArahan->Update() && ini->_xTask != mt_none) {
			if (ini->_nData->_mandoJaga) {
				ini->arahkan();
			}
		}

		if (tickMessage->Update()) {
			log_i("mando jaga = %d", ini->_nData->_mandoJaga);
			if (ini->_nData->_mandoJaga && ini->_xTask == mt_none) {
				if (ini->hantarMesej()) {
					tickMessage->Reset(180000);
				}
				else {
					tickMessage->Reset(60000);
				}
			}
		}

		while(ini->_serial->available()) {
			cr = ini->_serial->read();
			if (cr >= 32 && cr <= 126) {
				ini->_raw.concat(cr);
			}

			if (cr == 13) {
				ini->processData();
				ini->_raw = "";
			}

		}

		if (((millis()/1000UL) - ini->_nData->_masaVdo21 > 1800) || (millis()/1000UL) - ini->_nData->_masaSerial > 1800) {
			// asal 900s=15min
			ini->_nData->_masaSerial = millis()/1000UL;
			ini->_nData->_masaVdo21 = millis()/1000UL;
			ini->_xTask = mt_reboot;
		}

		if (ini->_xTask == mt_reboot) {
			ini->_serial->println("$PDBG,1,88,,,,,,*20");
			ini->_serial->flush();
			ini->_xTask = mt_none;
		}

		delay(10);
	}
}



void Mando::processData() {
	bool ada = false;
	if (this->_raw.indexOf("RF IC") == 0) {
		this->_timeout = false;
	}

	if (this->_raw.charAt(0) == '$' || this->_raw.charAt(0) == '!') {
		if (this->_nmea->validCheckSum(this->_raw)) {
			ada = true;
			this->_nData->_mandoJaga = true;
//			log_i(" sini == %s", this->_raw.c_str());

		}
	}

	if (ada) {
		this->_nData->_masaSerial = millis()/1000UL;
		String header = this->_nmea->extractCol(this->_raw, 0).substring(1);
		String tempY;
		if (header.indexOf("RMC") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			// $GPRMC,041539.00,A,0305.36953,N,10132.76635,E,0.002,,021220,,,D*72
			this->_nData->_header = header;
			if (this->_nmea->extractCol(this->_raw, 2) == "A") {
				this->_nData->_jumpaGPS = true;

				this->_nData->_LAT = this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 3));
				if (this->_nmea->extractCol(this->_raw, 4) == "S") {
					this->_nData->_LAT *= -1;
				}

				this->_nData->_LNG = this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 5));
				if (this->_nmea->extractCol(this->_raw, 6) == "W") {
					this->_nData->_LNG *= -1;
				}

				String sTime = this->_nmea->extractCol(this->_raw, 1);
				String sDate = this->_nmea->extractCol(this->_raw, 9);

//				setTime(sTime.substring(0,2).toInt(), sTime.substring(2,4).toInt(), sTime.substring(4,6).toInt(), sDate.substring(0,2).toInt(), sDate.substring(2,4).toInt(), sDate.substring(4,6).toInt());
//				adjustTime(3600*8);

				this->_nData->nTiming->setTimeNow(sTime.substring(0,2).toInt(), sTime.substring(2,4).toInt(), sTime.substring(4,6).toInt(), sDate.substring(0,2).toInt(), sDate.substring(2,4).toInt(), sDate.substring(4,6).toInt(), this->_nData->_LAT, this->_nData->_LNG);

			}


		}
		else if (header == "AIABK") {
			log_i(" sini == %s", this->_raw.c_str());
			// AIABK,995338888,B,06,0,2*1F
			// $AIABK,995338888,A,06,0,2*1C
			// $AIABK,995338888,B,06,0,3*1E
			// $AIABK,995338888,A,06,0,3*1D
			this->_nData->_header = header;

			this->_jumpaABK = true;

			tempY = this->_nmea->extractCol(this->_raw, 5).substring(0,1);

			if (tempY.equals("0") || tempY.equals("3")) {

			}


		}
		else if (header == "AIVDO") {
			log_i(" sini == %s", this->_raw.c_str());
			// !AIVDO,1,1,,A,E>m>bR2b2ab<<<00000000000003`Jrk0pTP`20@@@T>@0,4*29
			this->_nData->_header = header;
			String v = this->_nmea->extractCol(this->_raw, 5);

			if (v.indexOf("6") == 0) {
				this->_nData->_masaVdo6 = millis()/1000UL;
			}
			else if (v.indexOf("E") == 0) {
				this->_nData->_masaVdo21 = millis()/1000UL;
				int type21[17] = {6,2,30,5,120,1,28,27,30,4,6,1,8,1,1,1,1};

				std::vector<double>  jj = _nmea->exploade(this->_nmea->extractCol(this->_raw, 5), type21, 17);
				this->_nData->Msg6Data.ProcOffPositionStatus = jj[11];
			}
		}
		else if (header.indexOf("ACE") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			// $ANACE,995338888,E4,1000,0,0,0,TESTXXX,0020020202,R*0D
			this->_nData->fullACE = this->_raw;

			this->_tempMMSI = this->_nmea->extractCol(this->_raw, 1);
			this->_nData->MandoConf.MMmsi = this->_tempMMSI;
			this->_nData->MachineData.id = this->_tempMMSI;

			this->_nData->procAtonBit_prev = this->_nmea->extractCol(this->_raw, 2);

			this->_nData->MandoConf.MOffThres = this->_nmea->extractCol(this->_raw, 3);

			this->_nData->MandoConf.MName = this->_nmea->extractCol(this->_raw, 7);
			this->_nData->MachineData.name = this->_nData->MandoConf.MName;

			tempY = this->_nmea->extractCol(this->_raw, 8);
			this->_nData->MandoConf.MLengthA = tempY.substring(0,3);
			this->_nData->MandoConf.MLengthB = tempY.substring(3,6);
			this->_nData->MandoConf.MLengthC = tempY.substring(6,8);
			this->_nData->MandoConf.MLengthD = tempY.substring(8,10);

			if (this->_tempMMSI != "") {
				this->_xTask = mt_none;
				this->_nData->_xTaskComplete = true;
			}

		}
		else if (header.indexOf("ACF") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			// $ANACF,995338888,1,9100.0000,N,18100.0000,E,0,2087,2088,2087,2088,0,05,0,R*3F
			this->_fullACF = this->_raw;

			this->_nData->MandoConf.MEpfs = this->_nmea->extractCol(this->_raw, 2);

			this->_nData->MandoConf.MConfigLat = String(this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 3)), 4);
			this->_nData->MandoConf.MRegCardinalLat = this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 4));

			this->_nData->MandoConf.MConfigLng = String(this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 5)), 4);
			this->_nData->MandoConf.MRegCardinalLng = this->_nmea->toDegree(this->_nmea->extractCol(this->_raw, 6));

			this->_nData->MandoConf.MPosAcc = this->_nmea->extractCol(this->_raw, 7);

			this->_nData->MandoConf.MAtonType = this->_nmea->extractCol(this->_raw, 13);

			this->_xTask = ACE;

		}
		else if (header.indexOf("VER") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			// $ANVER,4,1,5,AN,AMC,995338888,A1K300040,,,*37
			// $ANVER,4,2,5,AN,AMC,995338888,,AIS-ATON-T3,,*20
			// $ANVER,4,3,5,AN,AMC,995338888,,,ATON-1.4.13.12 (TYPE 3),*30
			// $ANVER,4,4,5,AN,AMC,995338888,,,,CTLNC3RFNR2*51
			this->_xTask = ACF;

			if (this->_nmea->extractCol(this->_raw, 2) == "1") {
				this->_nData->MandoConf.MSerialNumber = this->_nmea->extractCol(this->_raw, 7);
			}



		}
		else if (header.indexOf("AAR") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			// $ANAAR,995338888,21,01,00,00,,180,1,00,00,,180,R*25
			// $ANAAR,995338888,06,01,00,00,,180,1,00,00,,180,R*20
			this->_xTask = VER;

			tempY = this->_nmea->extractCol(this->_raw, 2);
			if (tempY == "21") {
				this->_nData->MandoConf.M21UTChourA = this->_nmea->extractCol(this->_raw, 4);
				this->_nData->MandoConf.M21UTCminA = this->_nmea->extractCol(this->_raw, 5);
				this->_nData->MandoConf.M21IntervalA = this->_nmea->extractCol(this->_raw, 7).toInt();

				this->_nData->MandoConf.M21UTChourB = this->_nmea->extractCol(this->_raw, 9);
				this->_nData->MandoConf.M21UTCminB = this->_nmea->extractCol(this->_raw, 10);
				this->_nData->MandoConf.M21IntervalB = this->_nmea->extractCol(this->_raw, 12).toInt();

			}
			else if (tempY == "06") {
				this->_nData->MandoConf.M06UTChourA = this->_nmea->extractCol(this->_raw, 4);
				this->_nData->MandoConf.M06UTCminA = this->_nmea->extractCol(this->_raw, 5);
				this->_nData->MandoConf.M06IntervalA = this->_nmea->extractCol(this->_raw, 7).toInt();

				this->_nData->MandoConf.M06UTChourB = this->_nmea->extractCol(this->_raw, 9);
				this->_nData->MandoConf.M06UTCminB = this->_nmea->extractCol(this->_raw, 10);
				this->_nData->MandoConf.M06IntervalB = this->_nmea->extractCol(this->_raw, 12).toInt();

			}
		}
		else if (header.indexOf("PAMC") >= 0) {
			log_i(" sini == %s", this->_raw.c_str());
			if (this->_raw.indexOf("LanternLight_DetectStop") > 0 || this->_raw.indexOf("AtoN run into power saving mode") > 0) {
				this->_nData->_mandoJaga = false;
				log_i("Mando dah tidurrrr zzzzzzzzzzzzzzzz");
			}
			else if (this->_raw.indexOf("Timeout") > 0) {
				String amc = this->_nmea->extractCol(this->_raw, 3);
				amc = amc.substring(amc.indexOf('r') + 2, amc.indexOf('/'));
				if (amc.toInt() > 2400) {
					if (this->_timeout == false) {
						this->_timeout = true;
						this->_xTask = mt_reboot;
					}
				}


			}
		}

	}

}

bool Mando::hantarMesej() {
	bool res = false;

	if (this->_nData->_masaVdo21 > 0) {


		this->_nData->procesAtonBit();
		if (this->_nData->AtonBitLainTak()) {
			// $ANACE,995338888,E4,1000,0,0,0,TESTXXX,0020020202,R*0D
			// $AIACE,995338888,E7,1000,0,0,0,TESTXXX,0020020202,C*19
			// $AIACE,995338888,E7,1000,0,0,0,TESTXXX,0020020202,C*18
			String m21 = this->_nData->nakMsg21(this->_nmea);
			log_i("ni %s", m21.c_str());
			this->_serial->println(m21);
			this->_serial->flush();
			delay(10000);
			if (this->_xTask == mt_none) {
				this->_xTask = ACE;
			}
		}

		if (!this->_jumpaABK) {
			this->_msgSq++;
			if (this->_msgSq > 3) this->_msgSq = 0;
		}
		this->_jumpaABK = false;
		// !AIABM,1,1,0,701,2,6,@b0:2P001w060;80,4*79
		String temp6 = this->_nData->nakMsg6(_nmea, 701, this->_chan, this->_msgSq);
		log_i(" - ");
		log_i(" - ");
		log_i(" - sending msg 6 - - - %s", temp6.c_str());
		log_i(" - Beat = %d", this->_nData->SpiffsData.Beat);
		log_i(" - ");



		this->_serial->println(temp6);
		this->_serial->flush();

		if (this->_chan == 1) {
			this->_chan = 2;
		}
		else {
			this->_chan = 1;
			if (this->_nData->SpiffsData.Beat == 0) {
				this->_nData->SpiffsData.Beat = 1;
			}
			else {
				this->_nData->SpiffsData.Beat = 0;
			}
		}

		res = true;

	}

	return res;
}

void Mando::arahkan() {
	if (this->_xTask == ACE) {
		this->_serial->println("$ECAIQ,ACE*34");
		this->_serial->flush();
		log_i("sending $ECAIQ,ACE*34");

	}
	else if (this->_xTask == ACF) {
		this->_serial->println("$ECAIQ,ACF*37");
		this->_serial->flush();
		log_i("sending $ECAIQ,ACF*37");

	}
	else if (this->_xTask == VER) {
		this->_serial->println("$ECAIQ,VER*32");
		this->_serial->flush();
		log_i("sending $ECAIQ,VER*32");

	}
	else if (this->_xTask == AAR) {
		this->_serial->println("$ECAIQ,AAR*21");
		this->_serial->flush();
		log_i("sending $ECAIQ,AAR*21");
	}
	else if (this->_xTask == cAID1) {
		if (this->_tempMMSI != this->_nData->MandoConf.MMmsi) {
			this->_nData->_conFull = 6;


			String tempX = "$AIAID,";
			tempX += this->_tempMMSI;
			tempX += ",0,";
			tempX += this->_tempMMSI;
			tempX += ",R,C";

			this->_nmea->appendCheckSum(tempX);

			log_i("cAID1 === \n %s \n at %d", tempX.c_str(), millis());
			this->_serial->println(tempX);
			this->_serial->flush();
			this->_xTask = cAID2;
			this->_nData->_confProgress++;
			log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);

		}else {
			this->_xTask = cACE;
			this->_nData->_conFull = 4;
		}
	}
	else if (this->_xTask == cAID2) {

		String tempX = "$AIAID,000000000,1,";
		tempX += this->_nData->MandoConf.MMmsi;
		tempX += ",R,C";

		this->_nmea->appendCheckSum(tempX);

		log_i("cAID2 === \n %s \n at %d", tempX.c_str(), millis());
		this->_serial->println(tempX);
		this->_serial->flush();
		this->_xTask = cACE;
		this->_nData->_confProgress++;
		log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);
	}
	else if (this->_xTask == cACE) {

		String ConfOffPosThres = String(this->_nData->MandoConf.MOffThres);
		if (ConfOffPosThres.length() == 3)       ConfOffPosThres = "0" + ConfOffPosThres;
		else if (ConfOffPosThres.length() == 2)  ConfOffPosThres = "00" + ConfOffPosThres;
		else if (ConfOffPosThres.length() == 1)  ConfOffPosThres = "000" + ConfOffPosThres;
		else if (ConfOffPosThres.length() == 0)  ConfOffPosThres = "0010";
		else if (ConfOffPosThres.length() > 4)   ConfOffPosThres = "1000";

		if (this->_nData->MandoConf.MLengthA.length() == 2)		this->_nData->MandoConf.MLengthA = "0" + this->_nData->MandoConf.MLengthA;
		else if (this->_nData->MandoConf.MLengthA.length() == 1)	this->_nData->MandoConf.MLengthA = "00" + this->_nData->MandoConf.MLengthA;
		else if (this->_nData->MandoConf.MLengthA.length() == 0)	this->_nData->MandoConf.MLengthA = "001";
		else if (this->_nData->MandoConf.MLengthA.length() > 3)	this->_nData->MandoConf.MLengthA = "999";

		if (this->_nData->MandoConf.MLengthB.length() == 2)           this->_nData->MandoConf.MLengthB = "0" + this->_nData->MandoConf.MLengthB;
		else if (this->_nData->MandoConf.MLengthB.length() == 1)      this->_nData->MandoConf.MLengthB = "00" + this->_nData->MandoConf.MLengthB;
		else if (this->_nData->MandoConf.MLengthB.length() == 0)      this->_nData->MandoConf.MLengthB = "001";
		else if (this->_nData->MandoConf.MLengthB.length() > 3)       this->_nData->MandoConf.MLengthB = "999";

		if (this->_nData->MandoConf.MLengthC.length() == 1)           this->_nData->MandoConf.MLengthC = "0" + this->_nData->MandoConf.MLengthC;
		else if (this->_nData->MandoConf.MLengthC.length() == 0)      this->_nData->MandoConf.MLengthC = "01";
		else if (this->_nData->MandoConf.MLengthC.length() > 2)       this->_nData->MandoConf.MLengthC = "99";


		if (this->_nData->MandoConf.MLengthD.length() == 1)           this->_nData->MandoConf.MLengthD = "0" + this->_nData->MandoConf.MLengthD;
		else if (this->_nData->MandoConf.MLengthD.length() == 0)      this->_nData->MandoConf.MLengthD = "01";
		else if (this->_nData->MandoConf.MLengthD.length() > 2)       this->_nData->MandoConf.MLengthD = "99";
		//MName //MMmsi //MAtonType //MLengthA //MLengthB //MLengthC //MLengthD //MOffThres (int) //MEpfs //MPosAcc
		//MTempStrRegLat //MRegCardinalLat //MTempStrRegLng //MRegCardinalLng
		this->_nData->MandoConf.MName.toUpperCase();

		String tempX = "$AIACE,";
		tempX += this->_nData->MandoConf.MMmsi;
		tempX += ",";
		tempX += this->_nData->procAtonBit_prev;
		tempX += ",";
		tempX += ConfOffPosThres;
		tempX += ",0,0,0,";
		tempX += this->_nData->MandoConf.MName;
		tempX += ",";
		tempX += this->_nData->MandoConf.MLengthA;
		tempX += this->_nData->MandoConf.MLengthB;
		tempX += this->_nData->MandoConf.MLengthC;
		tempX += this->_nData->MandoConf.MLengthD;
		tempX += ",C";

		this->_nmea->appendCheckSum(tempX);


		log_i("cACE === \n %s \n at %d", tempX.c_str(), millis());
		this->_serial->println(tempX);
		this->_serial->flush();
		this->_xTask = cACF;
		this->_nData->_confProgress++;
		log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);

	}
	else if (this->_xTask == cACF) {
		String tempX = "$AIACF,";
		tempX += this->_nData->MandoConf.MMmsi;
		tempX += ",";
		tempX += this->_nData->MandoConf.MEpfs;
		tempX += ",";
		tempX += this->_nmea->toAISstd(this->_nData->MandoConf.MConfigLat, true);
		tempX += ",";
		tempX += this->_nData->MandoConf.MRegCardinalLat;
		tempX += ",";
		tempX += this->_nmea->toAISstd(this->_nData->MandoConf.MConfigLng, false);
		tempX += ",";
		tempX += this->_nData->MandoConf.MRegCardinalLng;
		tempX += ",";
		tempX += this->_nData->MandoConf.MPosAcc;
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 8);
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 9);
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 10);
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 11);
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 12);
		tempX += ",";
		tempX += this->_nData->MandoConf.MAtonType;
		tempX += ",";
		tempX += this->_nmea->extractCol(this->_fullACF, 14);
		tempX += ",C";

		this->_nmea->appendCheckSum(tempX);

		log_i("cACF === \n %s \n at %d", tempX.c_str(), millis());

		this->_serial->println(tempX);
		this->_serial->flush();
		this->_xTask = cAAR;
		this->_nData->_confProgress++;
		log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);

	}
	else if (this->_xTask == cAAR) {



		String temp1, temp2, temp3, temp4;

		//AIAAR 06
		//$AIAAR,995339999,06,01,02,01,,180,1,03,04,,180,C*32
		if (this->_nData->MandoConf.M06UTChourA.toInt() < 10)        temp1 = "0" + this->_nData->MandoConf.M06UTChourA;
		else if (this->_nData->MandoConf.M06UTChourA.toInt() > 99)   temp1 = "99";
		else                              temp1 = this->_nData->MandoConf.M06UTChourA;

		if (this->_nData->MandoConf.M06UTCminA.toInt() < 10)         temp2 = "0" + this->_nData->MandoConf.M06UTCminA;
		else if (this->_nData->MandoConf.M06UTCminA.toInt() > 99)    temp2 = "99";
		else                              temp2 = this->_nData->MandoConf.M06UTCminA;

		if (this->_nData->MandoConf.M06UTChourB.toInt() < 10)        temp3 = "0" + this->_nData->MandoConf.M06UTChourB;
		else if (this->_nData->MandoConf.M06UTChourB.toInt() > 99)   temp3 = "99";
		else                              temp3 = this->_nData->MandoConf.M06UTChourB;

		if (this->_nData->MandoConf.M06UTCminB.toInt() < 10)         temp4 = "0" + this->_nData->MandoConf.M06UTCminB;
		else if (this->_nData->MandoConf.M06UTCminB.toInt() > 99)    temp4 = "99";
		else                              temp4 = this->_nData->MandoConf.M06UTCminB;

		if (this->_nData->MandoConf.M06IntervalA > 86400) {
			this->_nData->MandoConf.M06IntervalA = 86400;
		}
		else if (this->_nData->MandoConf.M06IntervalA == 0) {
			this->_nData->MandoConf.M06IntervalA = 60;
		}

		if (this->_nData->MandoConf.M06IntervalB > 86400) {
			this->_nData->MandoConf.M06IntervalB = 86400;
		}
		else if (this->_nData->MandoConf.M06IntervalB == 0) {
			this->_nData->MandoConf.M06IntervalB = 60;
		}


		String tempX = "$AIAAR,";
		tempX += this->_nData->MandoConf.MMmsi;
		tempX += ",06,01,";
		tempX += temp1;
		tempX += ",";
		tempX += temp2;
		tempX += ",,";
		tempX += this->_nData->MandoConf.M06IntervalA;
		tempX += ",1,";
		tempX += temp3;
		tempX += ",";
		tempX += temp4;
		tempX += ",,";
		tempX += this->_nData->MandoConf.M06IntervalB;
		tempX += ",C";

		this->_nmea->appendCheckSum(tempX);

		log_i("cAAR 06 === \n %s \n at %d", tempX.c_str(), millis());
		this->_serial->println(tempX);
		this->_serial->flush();
		delay(100);

		//AIAAR 21
		//$AIAAR,995339999,06,01,02,01,,180,1,03,04,,180,C*32
		if (this->_nData->MandoConf.M21UTChourA.toInt() < 10)        temp1 = "0" + this->_nData->MandoConf.M21UTChourA;
		else if (this->_nData->MandoConf.M21UTChourA.toInt() > 99)   temp1 = "99";
		else                              temp1 = this->_nData->MandoConf.M21UTChourA;

		if (this->_nData->MandoConf.M21UTCminA.toInt() < 10)         temp2 = "0" + this->_nData->MandoConf.M21UTCminA;
		else if (this->_nData->MandoConf.M21UTCminA.toInt() > 99)    temp2 = "99";
		else                              temp2 = this->_nData->MandoConf.M21UTCminA;

		if (this->_nData->MandoConf.M21UTChourB.toInt() < 10)        temp3 = "0" + this->_nData->MandoConf.M21UTChourB;
		else if (this->_nData->MandoConf.M21UTChourB.toInt() > 99)   temp3 = "99";
		else                              temp3 = this->_nData->MandoConf.M21UTChourB;

		if (this->_nData->MandoConf.M21UTCminB.toInt() < 10)         temp4 = "0" + this->_nData->MandoConf.M21UTCminB;
		else if (this->_nData->MandoConf.M21UTCminB.toInt() > 99)    temp4 = "99";
		else                              temp4 = this->_nData->MandoConf.M21UTCminB;


		if (this->_nData->MandoConf.M21IntervalA > 86400) {
			this->_nData->MandoConf.M21IntervalA = 86400;
		}
		else if (this->_nData->MandoConf.M21IntervalA == 0) {
			this->_nData->MandoConf.M21IntervalA = 60;
		}

		if (this->_nData->MandoConf.M21IntervalB > 86400) {
			this->_nData->MandoConf.M21IntervalB = 86400;
		}
		else if (this->_nData->MandoConf.M21IntervalB == 0) {
			this->_nData->MandoConf.M21IntervalB = 60;
		}

		//$AIAAR,995339999,21,01,01,02,,180,1,04,05,,360,C*3D
		tempX = "$AIAAR,";
		tempX += this->_nData->MandoConf.MMmsi;
		tempX += ",21,01,";
		tempX += temp1;
		tempX += ",";
		tempX += temp2;
		tempX += ",,";
		tempX += this->_nData->MandoConf.M21IntervalA;
		tempX += ",1,";
		tempX += temp3;
		tempX += ",";
		tempX += temp4;
		tempX += ",,";
		tempX += this->_nData->MandoConf.M21IntervalB;
		tempX += ",C";



		this->_nmea->appendCheckSum(tempX);

		log_i("cAAR 21 === \n %s \n at %d", tempX.c_str(), millis());

		this->_serial->println(tempX);
		this->_serial->flush();
		this->_xTask = cPAMC;
		this->_nData->_confProgress++;
		log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);

	}
	else if (this->_xTask == cPAMC) {
		this->_serial->println("$PDBG,029,0,,,,,,*1A"); //disable dummy VDO
		log_i("$PDBG hantar ============== $PDBG,029,0,,,,,,*1A :: at %d", millis());
		delay(50);
		this->_serial->println("$$PDBG,051,5,,,,,,*10"); //set GPS rate to 5sec interval
		log_i("$$PDBG hantar ============== $$PDBG,051,5,,,,,,*10 :: at %d", millis());
		delay(50);
		this->_serial->println("$PAMC,C,ATN,20,1,0,*04"); //set interface type lantern monitoring
		log_i("$PAMC hantar ============== $PAMC,C,ATN,20,1,0,*04 :: at %d", millis());

		this->_xTask = AAR;
		this->_nData->_confProgress++;
		log_i("%d/%d", this->_nData->_confProgress, this->_nData->_conFull);
	}

}

