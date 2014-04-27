/*
 * ASIF_FS.cpp
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#include <Arduino.h>
#include <SdFat.h>
#include "ASIF_FS.h"
#include "asif_debug.h"
#include "asif_enode_program.h"
#include <avr/pgmspace.h>

void ASIF_FSClass::setKey(ASIF_FS_filekey lastkey) {
	this->file_key = lastkey;
	ASIF_DEBUG_GENERALOP("init key", lastkey);
}

ASIF_FS_filekey ASIF_FSClass::getKey() {
	return(file_key);
}

ASIF_FS_filekey ASIF_FSClass::allocFile(uint8_t srcid) {
	file_key++;
	if (file_key >= ASIF_FILEID_MAX)
		file_key = 0;
	ASIF_DEBUG_GENERALOP("alloc key", file_key);
	return (file_key);
}

boolean ASIF_FSClass::isSystem(ASIF_FS_filekey file_id) {
	if ((file_id & ASIF_FS_HEXMASK) == ASIF_FILEID_BLANK)
		return (true);
	if ((file_id & ASIF_FS_HEXMASK) == ASIF_FILEID_SHUTDOWN)
		return (true);
	if ((file_id & ASIF_FS_HEXMASK) == ASIF_FILEID_TEST)
		return (true);
	return (false);
}

const prog_uchar string_0[] PROGMEM = "BLANK.HEX";
#define PGM_STRING_FILEBLANK	string_0
const prog_uchar string_1[] PROGMEM = "SHUTDOWN.HEX";
#define PGM_STRING_FILESHUTDOWN		string_1
const prog_uchar string_2[] PROGMEM = "TEST.HEX";
#define PGM_STRING_FILETEST	string_2
const prog_uchar string_3[] PROGMEM = "SYSTEM.MEM";
#define PGM_STRING_FILESYSTEMMEM	string_3
const prog_uchar string_4[] PROGMEM = "SYSTEM.LOG";
#define PGM_STRING_FILESYSTEMLOG	string_4

void _copy_pgm(const prog_uchar *str, char * destination) {
	char c;
	int t_len = 0;
	while((c = pgm_read_byte(str++)))
		destination[t_len++] = c;
}

const char * ASIF_FSClass::getFilename(ASIF_FS_filekey file_id) {
	memset(filenamebuffer, 0x00, 16);
	int t_len = 0;

	if ((file_id) == ASIF_FILEID_BLANK)
	{
		_copy_pgm(PGM_STRING_FILEBLANK, filenamebuffer);
		return(filenamebuffer);
	}
	if ((file_id) == ASIF_FILEID_SHUTDOWN)
	{
		_copy_pgm(PGM_STRING_FILESHUTDOWN, filenamebuffer);
		return(filenamebuffer);
	}
	if ((file_id) == ASIF_FILEID_TEST)
	{
		_copy_pgm(PGM_STRING_FILETEST, filenamebuffer);
		return(filenamebuffer);
	}
	if ((file_id) == ASIF_FILEID_SYSTEM)
	{
		_copy_pgm(PGM_STRING_FILESYSTEMMEM, filenamebuffer);
		return(filenamebuffer);
	}
	if ((file_id) == ASIF_FILEID_LOG)
	{
		_copy_pgm(PGM_STRING_FILESYSTEMLOG, filenamebuffer);
		return(filenamebuffer);
	}


	filenamebuffer[t_len++] = 'F';
//	filenamebuffer[t_len++] = 'L';
	filenamebuffer[t_len++] = '_';

	if ((file_id & ASIF_FS_HEXMASK) < 0x1000)
		filenamebuffer[t_len++] = '0';
	if ((file_id & ASIF_FS_HEXMASK) < 0x0100)
		filenamebuffer[t_len++] = '0';
	if ((file_id & ASIF_FS_HEXMASK) < 0x0010)
		filenamebuffer[t_len++] = '0';
	itoa(file_id & ASIF_FS_HEXMASK, filenamebuffer + t_len, 16);
	t_len = strlen(filenamebuffer);

	filenamebuffer[t_len++] = '.';
	if ((file_id & ASIF_FS_DATMASK) != 0) {
		// DATA FILE
		filenamebuffer[t_len++] = 'D';
		filenamebuffer[t_len++] = 'A';
		filenamebuffer[t_len++] = 'T';
	} else if ((file_id & ASIF_FS_JPGMASK) != 0) {
		filenamebuffer[t_len++] = 'J';
		filenamebuffer[t_len++] = 'P';
		filenamebuffer[t_len++] = 'G';
	} else {
		// HEX FILE
		filenamebuffer[t_len++] = 'H';
		filenamebuffer[t_len++] = 'E';
		filenamebuffer[t_len++] = 'X';
	}

	return (filenamebuffer);
}

ASIF_FSClass ASIF_FS;
SdFile ASIF_CurrentFile;
