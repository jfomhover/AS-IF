/*
 * ASIF_FS.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_FS_H_
#define ASIF_FS_H_

#define ASIF_FILEID_NULL		0x7FFF
#define ASIF_FILEID_BLANK		0x7FFE
#define ASIF_FILEID_SHUTDOWN	0x7FFD
#define ASIF_FILEID_TEST		0x7FFC
#define ASIF_FILEID_SYSTEM		0x7FFB
#define ASIF_FILEID_LOG			0x7FFA

#define ASIF_FILEID_MAX			0x3000
#define ASIF_FS_HEXMASK			0x3FFF
#define ASIF_FS_DATMASK			0x4000
#define ASIF_FS_JPGMASK			0x8000

#define ASIF_FS_ID2PGM(id)		(id & ASIF_FS_HEXMASK)
#define ASIF_FS_ID2DAT(id)		((id & ASIF_FS_HEXMASK) | ASIF_FS_DATMASK)
#define ASIF_FS_ID2JPG(id)		((id & ASIF_FS_HEXMASK) | ASIF_FS_JPGMASK)

#include <SdFat.h>

typedef uint16_t ASIF_FS_filekey;

class ASIF_FSClass {
private:
	ASIF_FS_filekey file_key;
	char filenamebuffer[16];

public:
	void setKey(ASIF_FS_filekey key);
	ASIF_FS_filekey getKey();

	boolean isSystem(ASIF_FS_filekey file_id);
	ASIF_FS_filekey allocFile(uint8_t srcid);
	const char * getFilename(ASIF_FS_filekey file_id);  // to be used AFTER locking
};

extern ASIF_FSClass ASIF_FS;
extern SdFile ASIF_CurrentFile;

#endif /* ASIF_FS_H_ */
