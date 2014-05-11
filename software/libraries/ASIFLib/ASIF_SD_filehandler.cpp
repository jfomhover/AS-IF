#include <Arduino.h>
#include <SdFat.h>
#include <STK500Lib.h>
#include <STK_FileWrapper.h>
#include "ASIF_Control.h"
#include "ASIF_SD_filehandler.h"
#include "ASIF_FS.h"
#include "asif_debug.h"
#include <avr/pgmspace.h>


// *** RAM OPTIMIZATION FOR DEBUG (too much strings in this class)

const prog_uchar string_0[] PROGMEM = "can't create file";
#define PGM_STRING_CANTCREATEFILE	string_0
const prog_uchar string_1[] PROGMEM = "can't open file";
#define PGM_STRING_CANTOPENFILE		string_1



void ASIF_SD_filehandler::setSourceId(uint8_t srcid) {
	this->srcid = srcid;
}


void ASIF_SD_filehandler::setFileId(ASIF_FS_filekey id) {
	this->file_id = id;
}

// *************************
// *** PROGRAM FUNCTIONS ***
// *************************

// processing file_id of programs
// cleverly forwards to the private functions

boolean ASIF_SD_filehandler::createProgram() {
	this->file_id = ASIF_FS.allocFile(srcid);

	if (file_id == ASIF_FILEID_NULL)
		return (false);

	if (!create(ASIF_FS_ID2PGM(file_id)))
		return(false);

	if (!create(ASIF_FS_ID2DAT(file_id)))
		return(false);

	return(true);
}


boolean ASIF_SD_filehandler::openProgram() {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return (open());
}


void ASIF_SD_filehandler::closeProgram() {
	// TODO : write something here...
	if (ASIF_Control.canPushProgram(srcid))
		ASIF_Control.pushProgram(srcid, file_id);
}


void ASIF_SD_filehandler::eraseProgram() {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(erase());
}


uint32_t ASIF_SD_filehandler::getProgramSize() {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(getFileSize());
}


uint32_t ASIF_SD_filehandler::readProgramPage(uint8_t * buffer, uint32_t addr,
		uint32_t length) {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(read(buffer, addr, length));
}


uint32_t ASIF_SD_filehandler::writeProgramPage(uint8_t * buffer, uint32_t addr,
		uint32_t length) {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(write(buffer, addr, length));
}


uint32_t ASIF_SD_filehandler::appendProgramPage(uint8_t * buffer, uint32_t length) {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(append(buffer, length));
}


boolean ASIF_SD_filehandler::verifyProgramPage(uint8_t * buffer, uint32_t addr,
		uint32_t length) {
	this->file_id = ASIF_FS_ID2PGM(this->file_id);
	return(verify(buffer, addr, length));
}


// **********************
// *** DATA FUNCTIONS ***
// **********************

// processing file_id of data files
// cleverly forwards to the private functions

boolean ASIF_SD_filehandler::createData() {
	return(create(ASIF_FS_ID2DAT(this->file_id)));
}

void ASIF_SD_filehandler::eraseData() {
	this->file_id = ASIF_FS_ID2DAT(this->file_id);
	erase();
}

uint32_t ASIF_SD_filehandler::getDataSize() {
	this->file_id = ASIF_FS_ID2DAT(this->file_id);
	return(getFileSize());
}

uint32_t ASIF_SD_filehandler::appendData(uint8_t * buffer, uint32_t length) {
	this->file_id = ASIF_FS_ID2DAT(this->file_id);
	return(append(buffer, length));
}


// *************************
// *** PRIVATE FUNCTIONS ***
// *************************

// processing any file_id

boolean ASIF_SD_filehandler::create(ASIF_FS_filekey id) {
	if (file_id == ASIF_FILEID_NULL)
		return (false);

	this->file_id = id;

	ASIF_Control.lockSD();

	ASIF_DEBUG_DEBUGOP("create", ASIF_FS.getFilename(file_id));

	// creating (erasing) file
	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id),
			O_RDWR | O_CREAT | O_TRUNC)) {
		ASIF_DEBUGPGM_ERRORSRC(srcid, PGM_STRING_CANTCREATEFILE);
		ASIF_Control.unlockSD();
		return(false);
	} else {
		ASIF_CurrentFile.close();
		ASIF_Control.unlockSD();
		return(true);
	}
}

boolean ASIF_SD_filehandler::open() {
	// TODO : should we write something here ?
	return (true);
}


void ASIF_SD_filehandler::close() {
	// TODO : should we write something here ?
}


void ASIF_SD_filehandler::erase() {
	// TODO : should we write something here ?
}


uint32_t ASIF_SD_filehandler::getFileSize() {
	ASIF_Control.lockSD();
	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id), O_READ)) {
		ASIF_Control.unlockSD();
		return 0;
	}
	uint32_t t_ret = ASIF_CurrentFile.fileSize();
	ASIF_CurrentFile.close();
	ASIF_Control.unlockSD();
	return (t_ret);
}


uint32_t ASIF_SD_filehandler::read(uint8_t * buffer, uint32_t offset,
		uint32_t length) {
	ASIF_Control.lockSD();

	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id), O_READ)) {
		ASIF_Control.unlockSD();
		return(0);
	}

	ASIF_CurrentFile.seekSet(offset);
	for (uint32_t i = 0; i < length; i++) {
		int16_t t_ret = ASIF_CurrentFile.read();
		if (t_ret < 0) {
			ASIF_CurrentFile.close();
			ASIF_Control.unlockSD();
			return(i);
		} else {
			buffer[i] = t_ret & 0x00FF;
		}
	}
	ASIF_CurrentFile.close();
	ASIF_Control.unlockSD();
	return(length);
}


uint32_t ASIF_SD_filehandler::write(uint8_t * buffer, uint32_t offset,
		uint32_t length) {

	ASIF_Control.lockSD();

	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id), O_RDWR | O_CREAT | O_AT_END)) {
		ASIF_DEBUGPGM_WARNINGSRC(file_id, PGM_STRING_CANTOPENFILE);
		ASIF_Control.unlockSD();
		return(0);
	}

	ASIF_CurrentFile.seekSet(offset);

	size_t t_s = 0;
	for (uint32_t i = 0; i < length; i++) {
		t_s += ASIF_CurrentFile.write(buffer[i]);
		if (t_s <= i) {
			ASIF_CurrentFile.close();
			ASIF_Control.unlockSD();
			return(t_s);
		}
	}
	ASIF_CurrentFile.close();
	ASIF_Control.unlockSD();
	return(length);
}


uint32_t ASIF_SD_filehandler::append(uint8_t * buffer, uint32_t length) {
	ASIF_DEBUG_DEBUGOP("append", ASIF_FS.getFilename(file_id));

	unsigned int t_len = 0;

	ASIF_Control.lockSD();

	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id),
			O_WRITE | O_CREAT | O_AT_END)) {
		ASIF_DEBUGPGM_ERRORSRC(file_id, PGM_STRING_CANTOPENFILE);
		ASIF_Control.unlockSD();
		return(0);
	}

	t_len = ASIF_CurrentFile.fileSize();
	//      ASIF_CurrentFile.seekSet(offset);
	size_t t_s = 0;
	for (uint32_t i = 0; i < length; i++) {
		t_s += ASIF_CurrentFile.write(buffer[i]);
		if (t_s <= i) {
			ASIF_CurrentFile.close();
			ASIF_Control.unlockSD();
			return(t_s);
		}
	}

	ASIF_CurrentFile.close();
	ASIF_Control.unlockSD();
	return(t_s);

//      if ((t_len + length) > ASIF_DATA_LIMIT) {
//        ASIF_Control.endProgram(srcid, ASIF_FILEID_PGM(file_id));
//      }
}


boolean ASIF_SD_filehandler::verify(uint8_t * buffer, uint32_t offset,
		uint32_t length) {
	ASIF_Control.lockSD();

	if (!ASIF_CurrentFile.open(ASIF_FS.getFilename(file_id), O_READ)) {
		ASIF_DEBUGPGM_ERRORSRC(file_id, PGM_STRING_CANTOPENFILE);
		ASIF_Control.unlockSD();
		return (false);
	}

	ASIF_CurrentFile.seekSet(offset);

	for (uint32_t i = 0; i < length; i++) {
		if (buffer[i] != ASIF_CurrentFile.read()) {
//          Serial.print("error at byte ");
//          Serial.println(i);
			ASIF_CurrentFile.close();
			ASIF_Control.unlockSD();
			return (false);
		}
	}
	ASIF_CurrentFile.close();
	ASIF_Control.unlockSD();
	return (true);
}


