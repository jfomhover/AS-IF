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

#include <Arduino.h>
#include <SdFat.h>
#include "STK500Lib.h"
#include "STK_FileWrapper.h"
#include "STK_SDFileWrapper.h"

    void STK_SDFileWrapper::setFilename(const char * filename) {
      this->filename = filename;
    };

    boolean STK_SDFileWrapper::createProgram() {
      if (!file.open(filename, O_RDWR | O_CREAT | O_TRUNC)) {

        return(false);
      } else {
        file.close();
        return(true);
      }
    };

    boolean STK_SDFileWrapper::openProgram() {
      if (!file.open(filename, O_RDWR)) {
        return(false);
      } else {
        file.close();
        return(true);
      }
    };
    
    void STK_SDFileWrapper::closeProgram() {
    	STK_DEBUG_PRINTLN("program finished.");
    };
    
    void STK_SDFileWrapper::eraseProgram() {
    	STK_DEBUG_PRINTLN("program erased.");
    };

    uint32_t STK_SDFileWrapper::getProgramSize() {
      if (!file.open(filename, O_READ))
        return 0;
      uint32_t t_ret = file.fileSize();
      file.close();
      return(t_ret);
    };
    
    uint32_t STK_SDFileWrapper::readProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length) {
      if (!file.open(filename, O_READ))
        return(0);
      file.seekSet(addr);
      for (int i=0; i<length; i++) {
        buffer[i] = file.read(); // TODO / error in READ
      }
      file.close();
      return(length);
    };
    
    uint32_t STK_SDFileWrapper::writeProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length) {
      if (!file.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
        return(0);
      }
      file.seekSet(addr);
      for(int i=0; i<length; i++)
        file.write(buffer[i]);
      file.close();
      return(length);
    };


    uint32_t STK_SDFileWrapper::appendProgramPage(uint8_t * buffer, uint32_t length) {
      if (!file.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
        return(0);
      }
//      file.seekSet(offset);
      for(int i=0; i<length; i++)
        file.write(buffer[i]);
      file.close();
      return(length);
    };

    
    boolean STK_SDFileWrapper::verifyProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length) {
      if (!file.open(filename, O_READ))
        return(false);
      file.seekSet(addr);
      for (int i=0; i<length; i++) {
        if (buffer[i] != file.read()) {
          file.close();
          return(false);
        }
      }
      file.close();
      return(true);
    };

