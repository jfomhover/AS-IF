#ifndef _ASIF_SD_FILEHANDLER_H_
#define _ASIF_SD_FILEHANDLER_H_

#include "asif_config.h"
#include <STK500Lib.h>
#include <STK_FileWrapper.h>
#include "ASIF_FS.h"

#ifdef ASIF_PORTID_SPI

#include <SdFat.h>

class ASIF_SD_filehandler: public STK_FileWrapper {
private:
	ASIF_FS_filekey file_id;
	uint8_t srcid;

public:
	void setSourceId(uint8_t srcid);
	void setFileId(ASIF_FS_filekey pgm_id);

	boolean createProgram();
	boolean openProgram();
	void closeProgram();
	void eraseProgram();
	uint32_t getProgramSize();
	uint32_t readProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);
	uint32_t writeProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);
	uint32_t appendProgramPage(uint8_t * buffer, uint32_t length);
	boolean verifyProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);

	boolean createData();
	void eraseData();
	uint32_t getDataSize();
	uint32_t appendData(uint8_t * buffer, uint32_t length);

private:
	boolean create(ASIF_FS_filekey file_id);
	boolean open();
	void close();
	void erase();
	uint32_t getFileSize();
	uint32_t read(uint8_t * buffer, uint32_t offset, uint32_t length);
	uint32_t write(uint8_t * buffer, uint32_t offset, uint32_t length);
	uint32_t append(uint8_t * buffer, uint32_t length);
	boolean verify(uint8_t * buffer, uint32_t offset, uint32_t length);

	friend class ASIF_ControlClass;
};

#endif /* #ifdef ASIF_PORTID_SPI */

#endif /* _ASIF_SD_ACCESSHANDLER_H_ */
