#include <Arduino.h>
#include <Wire.h>
#include <SdFat.h>
#include <STK500Lib.h>

#include <ASIFLib.h>
#include <ASIF_Control.h>
#include <freeMemory.h>

#include <ASIF_ThreadI2CSensor.h>
#include <ASIF_ThreadIDEBridge.h>

ASIF_ThreadI2CSensor sensorTh;
ASIF_ThreadIDEBridge idebridgeTh;

void setup() {
	//	Serial.begin(115200);
	Serial.print("start mem=");
	Serial.println(freeMemory());

	ASIF_Control.start();

	Serial.print("after mem=");
	Serial.println(freeMemory());
}

void loop() {
	if (sensorTh.isTriggered()) {
		sensorTh.trigger();
		return;
	}
	if (idebridgeTh.isTriggered())
		idebridgeTh.trigger();
}

