#include <Arduino.h>
#include "asif_config.h"
#include "asif_debug.h"
#include "asif_ports.h"
#include "ASIF_Control.h"
#include "nanosat_message.h"
#include "asif_enode_program.h"
#include "util.h"
#include "power.h"
#include <avr/pgmspace.h>

#ifdef ASIF_RTOS
#include <ChibiOS_AVR.h>
#endif

// CONTROL STATE (so to speak)
static _asif_control_eeprom currentState;
static ASIF_SD_filehandler configFile;

// CENTRAL MEMORY
byte asif_globalbuffer[ASIF_CONTROL_BUFFERLEN];

#ifdef ASIF_RTOS
// if we want to do some threading with ChibiOS, we need to use semaphores
// to regulate access to shared ressources
SEMAPHORE_DECL(asif_buffer_semaphore, 1);	// BUFFER SEMAPHORE

#ifdef ASIF_PORTID_IDE	// don't want to declare too much, if port isn't even there...
SEMAPHORE_DECL(asif_serialide_semaphore, 1);		// SD SEMAPHORE
#endif
#ifdef ASIF_PORTID_ENODE
SEMAPHORE_DECL(asif_serialenode_semaphore, 1);		// SD SEMAPHORE
#endif
#ifdef ASIF_PORTID_DEBUG
SEMAPHORE_DECL(asif_serialdebug_semaphore, 1);		// SD SEMAPHORE
#endif
#ifdef ASIF_PORTID_SPI
SEMAPHORE_DECL(asif_spi_semaphore, 1);		// SD/SPI SEMAPHORE
#endif
#ifdef ASIF_PORTID_I2C
SEMAPHORE_DECL(asif_i2c_semaphore, 1);		// SD/SPI SEMAPHORE
#endif
#ifdef ASIF_PORTID_CAMERA
SEMAPHORE_DECL(asif_serialcam_semaphore, 0);		// CAMERA SEMAPHORE
#endif
#endif


// *** RAM OPTIMIZATION FOR DEBUG (too much strings in this class)

const prog_uchar string_0[] PROGMEM = "opening config file";
#define PGM_STRING_OPENINGCONFIGFILE	string_0
const prog_uchar string_1[] PROGMEM = "can't open config file";
#define PGM_STRING_CANTOPENCONFIGFILE		string_1
const prog_uchar string_2[] PROGMEM = "config file doesn't match current config";
#define PGM_STRING_CONFIGFILENOMATCHCURRENTCONFIG	string_2
const prog_uchar string_3[] PROGMEM ="config file corrupted";
#define PGM_STRING_CONFIGFILECORRUPTED	string_3
const prog_uchar string_4[] PROGMEM = "resume from config file";
#define PGM_STRING_RESUMEFROMCONFIGFILE	string_4
const prog_uchar string_5[] PROGMEM = "halting...";
#define PGM_STRING_HALTING		string_5
const prog_uchar string_6[] PROGMEM = "create blank state";
#define PGM_STRING_CREATEBLANKSTATE	string_6
const prog_uchar string_7[] PROGMEM = "saving config file";
#define PGM_STRING_SAVINGCONFIGFILE			string_7
const prog_uchar string_8[] PROGMEM = "erasing existing config file";
#define PGM_STRING_ERASINGCONFIGFILE		string_8
const prog_uchar string_9[] PROGMEM = "can't create config file";
#define PGM_STRING_CANTCREATECONFIGFILE		string_9
const prog_uchar string_10[] PROGMEM = "write config file";
#define PGM_STRING_WRITECONFIGFILE		string_10
const prog_uchar string_11[] PROGMEM = "can't write config file";
#define PGM_STRING_CANTWRITECONFIGFILE		string_11
const prog_uchar string_12[] PROGMEM = "config file saved";
#define PGM_STRING_CONFIGFILESAVED		string_12
const prog_uchar string_13[] PROGMEM = "allocBuffer";
#define PGM_STRING_ALLOCBUFFER	string_13
const prog_uchar string_14[] PROGMEM = "lockBuffer";
#define PGM_STRING_LOCKBUFFER	string_14
const prog_uchar string_15[] PROGMEM = "unlockBuffer";
#define PGM_STRING_UNLOCKBUFFER	string_15
const prog_uchar string_16[] PROGMEM = "lockSD";
#define PGM_STRING_LOCKSD	string_16
const prog_uchar string_17[] PROGMEM = "unlockSD";
#define PGM_STRING_UNLOCKSD	string_17

const prog_uchar string_18[] PROGMEM = "pushProgram";
#define PGM_STRING_PUSHPROGRAM	string_18
const prog_uchar string_19[] PROGMEM = "popProgram";
#define PGM_STRING_POPPROGRAM	string_19
const prog_uchar string_20[] PROGMEM = "beginProgram";
#define PGM_STRING_BEGINPROGRAM	string_20
const prog_uchar string_21[] PROGMEM = "endProgram";
#define PGM_STRING_ENDPROGRAM	string_21

const prog_uchar string_22[] PROGMEM = "lockCamera";
#define PGM_STRING_LOCKCAMERA	string_22
const prog_uchar string_23[] PROGMEM = "unlockCamera";
#define PGM_STRING_UNLOCKCAMERA	string_23



// *************************
// *** CONTROL FUNCTIONS ***
// *************************

// start the Control, reads the config file and loads the previous state
// if not available, creates a "blank" config and save it
void ASIF_ControlClass::start(ASIF_FS_filekey key) {
	// first, has to start ports
	startSystemPorts();

	allocBuffer(ASIF_THREADID_CONTROL, sizeof(currentState));

	lockBuffer(ASIF_THREADID_CONTROL);

	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_OPENINGCONFIGFILE);
	configFile.setFileId(ASIF_FILEID_SYSTEM);
	configFile.setSourceId(ASIF_THREADID_CONTROL);

	uint32_t t_len = configFile.read(asif_globalbuffer, ASIF_CONTROL_BUFFERLEN, ASIF_CONTROL_BUFFERLEN);

	if (t_len == 0) {
		// file can't be accessed... maybe it doesn't exist
		ASIF_DEBUGPGM_ERROR(PGM_STRING_CANTOPENCONFIGFILE);
//		ASIF_DEBUG_ERROR(ASIF_THREADID_CONTROL, "can't open config file");

		blankState(key);
	} else if (t_len < sizeof(currentState)) {
		// the structure stored in the config file doesn't match the control structure
		// (maybe different number of program slots)
		ASIF_DEBUGPGM_ERROR(PGM_STRING_CONFIGFILENOMATCHCURRENTCONFIG);
		blankState(key);
	} else {
		// we need to check the checksum of the config file, maybe it would be corrupted (you never know !)
		memcpy((void*) &currentState, (void*) asif_globalbuffer,
				sizeof(currentState));
		uint16_t t_crc16 = asif_checksum(asif_globalbuffer,
				sizeof(currentState) - 2);
		if (t_crc16 != currentState.crc16) {
			ASIF_DEBUGPGM_ERROR(PGM_STRING_CONFIGFILECORRUPTED);
			blankState(key);
		}
	}

	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_RESUMEFROMCONFIGFILE);
	// resuming from the retrieved (or created) currentState
	resumeState();

	// save current state on SD, so that everything's in place for the next start
	// and all the errors above will be forgotten ^^
	// NOTE : save() will halt() if the SD can't be accessed
	if (!saveState()) {
	//	this->halt();
	}

	// starting ports
	startUserPorts();

	// TODO : starting threads... from where ?

	// everything's ok, we display
	displayState();

	unlockBuffer(ASIF_THREADID_CONTROL);
}

// HALTING !!!
// locks everything and halts, (maybe should shutdown)
void ASIF_ControlClass::halt() {
	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_HALTING);
	ASIF_DEBUG_STREAM_FLUSH();
//	while(1);

	// TODO : lock everything...
	sleepNow();

	while(1); // will not get there anyway... but you never know !
}

// resume state from currentState ("cleans" values in currentState)
void ASIF_ControlClass::resumeState() {
	// start the FS from the retrieve key
	ASIF_FS.setKey(currentState.pgm_key);

	// check every program slot, and realign them
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		// use current time as time stamp
		currentState.pgm_slots[i].started = millis();

		switch (currentState.pgm_slots[i].state) {
		// if the program has been received, consider is as "ready for upload"
		case PGM_READY_FOR_UPLOAD:
		case PGM_UPLOADING:
		case PGM_ERROR_UPLOADING:
			currentState.pgm_slots[i].state = PGM_READY_FOR_UPLOAD;
			break;
			// if the program was terminated or else, consider as a "free" slot
		case PGM_RUNNING:	// if we reupload the running program we may have some perpetual loop
							// in case this program isn't generating any data
			currentState.pgm_slots[i].state = PGM_TERMINATED;
			break;
		case PGM_FREE:
		case PGM_ALLOCATED:
		case PGM_RECEIVING:
		case PGM_TERMINATED:
		default:
			currentState.pgm_slots[i].state = PGM_FREE;
			break;
		};
	}
}

// creates a "blank" configuration and store it on SD
void ASIF_ControlClass::blankState(ASIF_FS_filekey key) {
	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_CREATEBLANKSTATE);

	currentState.marker = '#';	// TODO : useless now that we have CRC
	currentState.pgm_key = key; // we can choose to init with the key we want
	currentState.slots = ASIF_PROGRAM_AVAILABLESLOTS;

	// initialize every program slot
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		currentState.pgm_slots[i].state = PGM_FREE;
		currentState.pgm_slots[i].pgm_id = currentState.pgm_key;// TODO : useless
		currentState.pgm_slots[i].started = millis();		// TODO : useless
	}
}

// saves the currentState on SD
boolean ASIF_ControlClass::saveState() {
	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_SAVINGCONFIGFILE);

	configFile.setSourceId(ASIF_THREADID_CONTROL);

	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_ERASINGCONFIGFILE);
	// create (or blank) the config file on SD
	if (!configFile.create(ASIF_FILEID_SYSTEM)) {
		// if this fails, this is FATAL
		ASIF_DEBUGPGM_ERROR(PGM_STRING_CANTCREATECONFIGFILE);
		ASIF_DEBUG_STREAM_FLUSH();
		return(false);
	}

	ASIF_DEBUG_DEBUGOP(ASIF_THREADID_CONTROL, "getting FS key");
	// gets the current key from FS
	currentState.pgm_key = ASIF_FS.getKey();

	ASIF_DEBUG_DEBUGOP(ASIF_THREADID_CONTROL, "calculating checksum");
	// computes the checksum of the structure, to be stored with it for corruption detection on next resume
	currentState.crc16 = asif_checksum((uint8_t *) &currentState, sizeof(currentState) - 2);

	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_WRITECONFIGFILE);
	// writes the currentState structure into the system file on SD
	if (configFile.write((uint8_t *) &currentState, 0, sizeof(currentState))
			< sizeof(currentState)) {
		// if this fails, this is FATAL
		ASIF_DEBUGPGM_ERROR(PGM_STRING_CANTWRITECONFIGFILE);
		ASIF_DEBUG_STREAM_FLUSH();
		return(false);
	}

	ASIF_DEBUGPGM_GENERALOP(PGM_STRING_CONFIGFILESAVED);
	ASIF_DEBUG_STREAM_FLUSH();
	return(true);
}

// display the current state on DEBUG
void ASIF_ControlClass::displayState() {
	ASIF_DEBUG_GENERALOP("key", currentState.pgm_key);
	ASIF_DEBUG_GENERALOP("slots", currentState.slots);

	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		ASIF_DEBUG_GENERALOP(currentState.pgm_slots[i].pgm_id,
				currentState.pgm_slots[i].state);
	}
//	ASIF_DEBUG_STREAM_FLUSH();
}


// *************************
// *** MEMORY MANAGEMENT ***
// *************************

// requires allocation of a buffer
byte * ASIF_ControlClass::allocBuffer(uint8_t srcid, int size) {
	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_ALLOCBUFFER);
	if (size > ASIF_CONTROL_BUFFERLEN)
		return (NULL);
	return (asif_globalbuffer);
}

// requires access to a buffer
void ASIF_ControlClass::lockBuffer(uint8_t srcid) {
#ifdef ASIF_RTOS
	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_LOCKBUFFER);
//	ASIF_DEBUG_CONTROL_LLOP(srcid,"lockBuffer");
	chSemWait(&asif_buffer_semaphore);
#endif
}

// release access to a buffer
void ASIF_ControlClass::unlockBuffer(uint8_t srcid) {
#ifdef ASIF_RTOS
	chSemSignal(&asif_buffer_semaphore);
	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_UNLOCKBUFFER);
//	ASIF_DEBUG_CONTROL_LLOP(srcid,"unlockBuffer");
#endif
}

// ***********************
// *** PORT MANAGEMENT ***
// ***********************

// starts the ports that are absolutely necessary (SD, DEBUG)
void ASIF_ControlClass::startSystemPorts() {
	if (asif_port_init_debug() < 0)
		halt();

	if (asif_port_init_spi() < 0)
		halt();
}

// starts the ports that are needed by operations (IDE, ENODE, CAMERA)
void ASIF_ControlClass::startUserPorts() {
	asif_port_init_ide();
	asif_port_init_enode();
	asif_port_init_camera();
	asif_port_init_i2c();
}

// halts the ports
void ASIF_ControlClass::haltUserPorts() {
	// TODO : what to do here ?
	asif_port_halt_ide();
	asif_port_halt_enode();
	asif_port_halt_camera();
	asif_port_halt_i2c();
}


// requires access to the port
void ASIF_ControlClass::lockPort(uint8_t portid) {
//  ASIF_DEBUG_CONTROL_LLOP(srcid,"lockPort");
#ifdef ASIF_RTOS
	switch(portid) {
#ifdef ASIF_PORTID_IDE
	case ASIF_PORTID_IDE:
		chSemWait(&asif_serialide_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_ENODE
	case ASIF_PORTID_ENODE:
		chSemWait(&asif_serialenode_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_CAMERA
	case ASIF_PORTID_CAMERA:
		chSemWait(&asif_serialcamera_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_SPI
	case ASIF_PORTID_SPI:
		chSemWait(&asif_spi_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_I2C
	case ASIF_PORTID_I2C:
		chSemWait(&asif_i2c_semaphore);
		break;
#endif
	default:
		break;
	}
#endif
}

// release access to the port
void ASIF_ControlClass::unlockPort(uint8_t portid) {
//  ASIF_DEBUG_CONTROL_LLOP(srcid,"unlockPort");
#ifdef ASIF_RTOS
	switch(portid) {
#ifdef ASIF_PORTID_IDE
	case ASIF_PORTID_IDE:
		chSemSignal(&asif_serialide_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_ENODE
	case ASIF_PORTID_ENODE:
		chSemSignal(&asif_serialenode_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_CAMERA
	case ASIF_PORTID_CAMERA:
		chSemSignal(&asif_serialcamera_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_SPI
	case ASIF_PORTID_SPI:
		chSemSignal(&asif_spi_semaphore);
		break;
#endif
#ifdef ASIF_PORTID_I2C
	case ASIF_PORTID_I2C:
		chSemSignal(&asif_i2c_semaphore);
		break;
#endif
	default:
		break;
	}
#endif
}


// **************************
// *** SD/FILE MANAGEMENT ***
// **************************

void ASIF_ControlClass::lockSD() {
	ASIF_DEBUGPGM_CONTROL_LLOP(ASIF_THREADID_CONTROL, PGM_STRING_LOCKSD);
#if (defined(ASIF_PORTID_SPI) && defined(ASIF_RTOS))
	lockPort(ASIF_PORTID_SPI);
#endif
}

void ASIF_ControlClass::unlockSD() {
#if (defined(ASIF_PORTID_SPI) && defined(ASIF_RTOS))
	unlockPort(ASIF_PORTID_SPI);
#endif
	ASIF_DEBUGPGM_CONTROL_LLOP(ASIF_THREADID_CONTROL, PGM_STRING_UNLOCKSD);
}

// **********************************
// *** CAMERA SNAPSHOT MANAGEMENT ***
// **********************************

void ASIF_ControlClass::lockSnapshot() {
	ASIF_DEBUGPGM_CONTROL_LLOP(ASIF_THREADID_CONTROL, PGM_STRING_LOCKCAMERA);
#if (defined(ASIF_PORTID_CAMERA) && defined(ASIF_RTOS))
	lockPort(ASIF_PORTID_CAMERA);
#endif
}

void ASIF_ControlClass::unlockSnapshot() {
#if (defined(ASIF_PORTID_CAMERA) && defined(ASIF_RTOS))
	unlockPort(ASIF_PORTID_CAMERA);
#endif
	ASIF_DEBUGPGM_CONTROL_LLOP(ASIF_THREADID_CONTROL, PGM_STRING_UNLOCKCAMERA);
}

// ***************************
// *** PROGRAMS MANAGEMENT ***
// ***************************

/*
 uint16_t ASIF_ControlClass::allocProgram(uint8_t srcid) {
 ASIF_DEBUG_GENERALOP(srcid,"allocProgram");

 #ifdef ASIF_PROGRAM_USEBRIDGE
 #ifdef ASIF_CONTROL_FORCEUPLOAD
 currentState.pgm_slots[0].state = PGM_ALLOCATED;
 currentState.pgm_slots[0].pgm_id = ++currentState.pgm_key;
 return(currentState.pgm_slots[0].pgm_id);
 #else
 if (currentState.pgm_slots[0].state != PGM_RUNNING) {
 currentState.pgm_slots[0].state = PGM_ALLOCATED;
 currentState.pgm_slots[0].pgm_id = ++currentState.pgm_key;
 return (currentState.pgm_slots[0].pgm_id);
 } else
 return (ASIF_PGMID_NULL);
 #endif
 #else
 for(int i=0; i<ASIF_PROGRAM_AVAILABLESLOTS; i++) {
 if (currentState.pgm_slots[i].state == PGM_FREE) {
 currentState.pgm_slots[i].state = PGM_ALLOCATED;
 currentState.pgm_slots[i].pgm_id = ++currentState.pgm_key;
 ASIF_DEBUG_CONTROL_LLOP(srcid,"allocKey");
 ASIF_DEBUG_CONTROL_LLOP(srcid,currentState.pgm_slots[i].pgm_id);
 if (currentState.pgm_key > 0x7000)
 currentState.pgm_key = 0;
 return(currentState.pgm_slots[i].pgm_id);
 }
 }
 return(ASIF_PGMID_NULL);
 #endif
 }
 */

boolean ASIF_ControlClass::canPushProgram(uint8_t srcid) {
#ifdef ASIF_PROGRAM_USEBRIDGE
//	currentState.pgm_slots[0].pgm_id = pgmid;
//	currentState.pgm_slots[0].state = PGM_RUNNING;
//	this->save();
	return(true);
#else
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].state == PGM_FREE) {
			currentState.pgm_slots[i].state = PGM_FREE; // TODO : only on UNO
			return (true);
		}
	}
	return (false);
#endif
}

boolean ASIF_ControlClass::pushProgram(uint8_t srcid, ASIF_FS_filekey pgmid) {
	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_PUSHPROGRAM);
	ASIF_DEBUG_DEBUGOP(srcid, pgmid);

#ifdef ASIF_PROGRAM_USEBRIDGE
	currentState.pgm_slots[0].pgm_id = pgmid;
	currentState.pgm_slots[0].state = PGM_RUNNING;
	this->saveState();
	return(true);
#else
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].state == PGM_FREE) {
			currentState.pgm_slots[i].pgm_id = pgmid;
			currentState.pgm_slots[i].state = PGM_READY_FOR_UPLOAD; // TODO : only on UNO
			this->saveState();
			return (true);
		}
	}
	return (false);
#endif
}

boolean ASIF_ControlClass::hasProgram() {
#ifdef ASIF_PROGRAM_USEBRIDGE
	return (false);
#else
	if (getRunningProgram() != ASIF_PGMID_NULL)
		return (false);

	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].state == PGM_READY_FOR_UPLOAD) {
			return (true);
		}
	}
	return (false);
#endif
}

uint16_t ASIF_ControlClass::popProgram() {
#ifdef ASIF_PROGRAM_USEBRIDGE
	return (ASIF_PGMID_NULL);
#else
	ASIF_DEBUGPGM_CONTROL_LLOP(ASIF_THREADID_CONTROL, PGM_STRING_POPPROGRAM);

	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].state == PGM_READY_FOR_UPLOAD) {
			currentState.pgm_slots[i].state = PGM_UPLOADING;
			return (currentState.pgm_slots[i].pgm_id);
		}
	}
	return (ASIF_PGMID_NULL);
#endif
}

uint16_t ASIF_ControlClass::getRunningProgram() {
#ifdef ASIF_PROGRAM_USEBRIDGE
	if (currentState.pgm_slots[0].state == PGM_RUNNING)
	return (currentState.pgm_slots[0].pgm_id);
	else
	return (ASIF_PGMID_NULL);
#else
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].state == PGM_RUNNING) {
			return (currentState.pgm_slots[i].pgm_id);
		}
	}
	return (ASIF_PGMID_NULL);
#endif
}

void ASIF_ControlClass::beginProgram(uint8_t srcid, uint16_t pgm_id) {
	if (ASIF_FS.isSystem(pgm_id))
		return;

	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_BEGINPROGRAM);
	ASIF_DEBUG_DEBUGOP(srcid, ASIF_FS_ID2PGM(pgm_id));

	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].pgm_id == pgm_id) {
			if (currentState.pgm_slots[i].state == PGM_UPLOADING) {
				currentState.pgm_slots[i].state = PGM_RUNNING;
				currentState.pgm_slots[i].started = millis();
				asif_i2cPort.flushInput(); // discard messages stuck between programs
				this->saveState();
				return;
			}
		}
	}
}

void ASIF_ControlClass::endProgram(uint8_t srcid, uint16_t pgm_id) {
	if (ASIF_FS.isSystem(pgm_id))
		return;

	ASIF_DEBUGPGM_CONTROL_LLOP(srcid, PGM_STRING_ENDPROGRAM);
	ASIF_DEBUG_DEBUGOP(srcid, ASIF_FS_ID2PGM(pgm_id));

#ifdef ASIF_PROGRAM_USEBRIDGE
	currentState.pgm_slots[0].state = PGM_TERMINATED;
	digitalWrite(ASIF_ENODE_RST, LOW);
#else
//  if (isProgramSystem(pgm_id))
//    return;
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		if (currentState.pgm_slots[i].pgm_id == pgm_id) {
			currentState.pgm_slots[i].pgm_id = pgm_id;
			currentState.pgm_slots[i].state = PGM_FREE;
		}
	}
#endif
	this->saveState();
}

void ASIF_ControlClass::cleanPrograms() {
#ifndef ASIF_PROGRAM_USEBRIDGE
	for (int i = 0; i < ASIF_PROGRAM_AVAILABLESLOTS; i++) {
		switch (currentState.pgm_slots[i].state) {
//		case PGM_READY_FOR_UPLOAD:
		case PGM_UPLOADING:
		case PGM_ERROR_UPLOADING:
		case PGM_ALLOCATED:
		case PGM_RECEIVING:
		case PGM_RUNNING:
			if ((millis() - currentState.pgm_slots[i].started) > ASIF_TIME_LIMIT)
				currentState.pgm_slots[i].pgm_id = PGM_FREE;
			break;
		case PGM_TERMINATED:
		case PGM_FREE:
			currentState.pgm_slots[i].state = PGM_FREE;
			break;
		default:
			break;
		};
	}
#endif
}

// ******************************
// *** CONTROL CLASS INSTANCE ***
// ******************************

ASIF_ControlClass ASIF_Control;

