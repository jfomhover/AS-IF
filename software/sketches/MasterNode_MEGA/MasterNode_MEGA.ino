#include <Arduino.h>
#include <Wire.h>
#include <SdFat.h>
//#include <EEPROM.h>
#include <ChibiOS_AVR.h>

#ifdef ASIF_SAVECONTROLINEEPROM
#include <EEPROM.h>
#endif

#include <SoftwareSerial.h>
#include <STK500Lib.h>
#include <ASIFLib.h>
#include <asif_config.h>
#include <asif_ports.h>
#include <asif_debug.h>
#include <ASIF_Control.h>
#include <freeMemory.h>

#ifdef ASIF_RTOS
#include <ChibiOS_AVR.h>

static WORKING_AREA(lameThd, 200);
static WORKING_AREA(ideThd, 200);
static WORKING_AREA(progThd, 200);
static WORKING_AREA(sensorThd, 200);
//static WORKING_AREA(camThd, 200);

static msg_t threadFcn(void * obj);
void chibiSetup();
#endif

#include <ASIF_ThreadIDEServer.h>
#include <ASIF_ThreadENODEProg.h>
#include <ASIF_ThreadI2CSensor.h>
#include <ASIF_ThreadCamera.h>
#include <ASIF_LameThread.h>

ASIF_ThreadI2CSensor sensorTh;
ASIF_ThreadIDEServer ideservTh;
ASIF_ThreadENODEProg progTh;
//ASIF_ThreadCamera camTh;
ASIF_LameThread lameTh;

#ifdef ASIF_RTOS
void setup() {

//	asif_port_init_all();

	ASIF_Control.start();

	ASIF_DEBUG_GENERALOP("start mem=", freeMemory());

	chBegin(chibiSetup);
}

void loop() { /* never used */ }

void chibiSetup() {
//	chThdCreateStatic(lameThd, sizeof(lameThd), LOWPRIO, threadFcn,
//			(void*) &lameTh);

//	chThdCreateStatic(camThd, sizeof(camThd), NORMALPRIO, threadFcn,
//			(void*) &camTh);
	chThdCreateStatic(ideThd, sizeof(ideThd), NORMALPRIO, threadFcn,
			(void*) &ideservTh);
	chThdCreateStatic(progThd, sizeof(progThd), NORMALPRIO, threadFcn,
			(void*) &progTh);
	chThdCreateStatic(sensorThd, sizeof(sensorThd), NORMALPRIO, threadFcn,
			(void*) &sensorTh);
//	threadFcn((void*)NULL);
	threadFcn((void*)&lameTh);
//	threadFcn((void*)&sensorTh);
}

static msg_t threadFcn(void * obj) {
	ASIF_Thread * t_thread = (ASIF_Thread *) obj;
	if (t_thread != NULL)
		t_thread->setup();

	while (true) {
		if (t_thread != NULL) {
			if (t_thread->isTriggered())
				t_thread->trigger();
		} else {
			ASIF_DELAY(60000);
		}
	}
}
#endif

