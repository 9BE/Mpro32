/*
 * NdirectOTA.h
 *
 *  Created on: Nov 18, 2020
 *      Author: suhaimi
 */

#ifndef OURNETWORK_NDIRECTOTA_H_
#define OURNETWORK_NDIRECTOTA_H_

#include "Arduino.h"
#include "WebServer.h"
#include "ESPmDNS.h"
#include "Update.h"
#include "pages.h"

class N_directOTA {
private:
	uint32_t _loopDelay;
	WebServer * _server = NULL;
	String getMAC();
public:
	N_directOTA(uint32_t core, uint32_t loopDelay);
	virtual ~N_directOTA();
	static void loop(void * param);
	static void statusViaWiFi(N_directOTA * ini);

	WebServer* getServer() {
		return _server;
	}
};

#endif /* OURNETWORK_NDIRECTOTA_H_ */
