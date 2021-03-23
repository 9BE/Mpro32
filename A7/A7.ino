#include "Arduino.h"
#include "LocWireless.h"

#include "OccasHandler.h"


LocWireless * o_WiFi;
OccasHandler * o_occas;


//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	Serial.begin(	115200,	SERIAL_8N1,  3,  1);
	Serial1.begin(	2400,	SERIAL_8N1,  22,  23);			//begin serial for rs485
	Serial2.begin(	115200, SERIAL_8N1,  26,  25);		//begin serial for rs232
	o_WiFi = new LocWireless(0, 15000);

	o_occas = new OccasHandler();




}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}
