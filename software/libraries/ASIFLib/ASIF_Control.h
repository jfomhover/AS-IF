#ifndef _ASIF_CONTROL_H_
#define _ASIF_CONTROL_H_

#include "asif_config.h"
#include "asif_enode_program.h"
#include "ASIF_SD_filehandler.h"
#include "ASIF_FS.h"

/*
 void asif_control_start();

 void asif_control_allocbuffer(uint8_t srcid, uint16_t size);
 void asif_control_lockbuffer(uint8_t srcid);
 void asif_control_unlockbuffer(uint8_t srcid);


 boolean asif_control_hasmessage();
 void asif_control_popmessage(uint8_t srcid, uint8_t * destination);
 void asif_control_pushmessage(uint8_t srcid, uint8_t * source, uint16_t length);
 */

struct _asif_control_eeprom {
	uint8_t marker; // '#'
	ASIF_FS_filekey pgm_key;
	uint8_t slots;
	ASIF_ENodeProgram pgm_slots[ASIF_PROGRAM_AVAILABLESLOTS];
	uint16_t crc16;
};

class ASIF_ControlClass {
public:

	// *** CONTROL FUNCTIONS ***
	void start(ASIF_FS_filekey key = 0);		// start the Control, reads the config file and loads the previous state
	void halt();								// locks everything and halts, power is shut down
	void resumeState();							// resume state from currentState ("cleans" values in currentState)
	void blankState(ASIF_FS_filekey key = 0);	// creates a "blank" configuration and store it on SD
	boolean saveState();							// saves the currentState on SD
												// WARNING : if impossible to save, system halts !
	void displayState();						// display the current state on DEBUG

	// *** MEMORY MANAGEMENT ***
	byte * allocBuffer(uint8_t srcid, int size);// requires allocation of a buffer
	void lockBuffer(uint8_t srcid);				// requires access to a buffer
	void unlockBuffer(uint8_t srcid);			// release access to a buffer

	// *** PORT MANAGEMENT ***
	void startSystemPorts();			// starts the ports that are absolutely necessary (SD, DEBUG)
	void startUserPorts();				// starts the ports that are needed by operations (IDE, ENODE, CAMERA)
	void haltUserPorts();				// halts the ports (?)
	void lockPort(uint8_t portid);		// requires access to the port
	void unlockPort(uint8_t portid);	// release access to the port

	// *** SD/FILE MANAGEMENT ***
	void lockSD();						// = lockPort(ASIF_PORTID_SPI);
	void unlockSD();					// = unlockPort(ASIF_PORTID_SPI);

	void lockSnapshot();
	void unlockSnapshot();

	// *** PROGRAMS MANAGEMENT ***
//  boolean isProgramSystem(uint16_t pgm_id);
	boolean canPushProgram(uint8_t srcid);
	boolean pushProgram(uint8_t srcid, ASIF_FS_filekey pgm_id);
	boolean hasProgram();
	uint16_t popProgram();
	void cleanPrograms();

	void beginProgram(uint8_t srcid, ASIF_FS_filekey pgm_id);
	uint16_t getRunningProgram();
	void endProgram(uint8_t srcid, ASIF_FS_filekey pgm_id);

};

extern ASIF_ControlClass ASIF_Control;

#endif /* _ASIF_CONTROL_H_ */
