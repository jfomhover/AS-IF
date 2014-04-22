/*
********************************************************************

Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com, twitter @jfomhover)

********************************************************************
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************

	Description : handles stk500 communication via serial
				  to receive uploaded sketches from arduino IDE
				  store the HEX file on SD
				  use that HEX file to program an external arduino node
	Last changed : April 22, 2014

********************************************************************
*/

#ifndef _STK_SDFILEWRAPPER_H_
#define _STK_SDFILEWRAPPER_H_

#include <SdFat.h>
#include "STK_FileWrapper.h"



class STK_SDFileWrapper : public STK_FileWrapper {
public:
    SdFile file;
    const char * filename;

    void setFilename(const char * filename);
    boolean createProgram();
    boolean openProgram();
    void closeProgram();
    void eraseProgram();
    uint32_t getProgramSize();
    uint32_t readProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);
    uint32_t writeProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);
    uint32_t appendProgramPage(uint8_t * buffer, uint32_t length);
    boolean verifyProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);
};

#endif /* _STK_SDFILEWRAPPER_H_ */
