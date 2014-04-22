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

#ifndef _STK_FILEWRAPPER_H_
#define _STK_FILEWRAPPER_H_

// That's all the functions the STK_StreamProgrammer or STK_StreamServer has to be able to manipulate
// in order to store/retrieve prog pages from files, however you do that... (SD ?)

class STK_FileWrapper {
  public:
    // create new content, ERASE IF PREVIOUSLY EXISTING content
    virtual boolean createProgram();

    // open content
    virtual boolean openProgram();

    // close content
    virtual void closeProgram();
    
    // discard content (for instance, if unfinished)
    virtual void eraseProgram();
    
    // gets the size of the current content
    virtual uint32_t getProgramSize();
    
    // reads [length] bytes of data at offset [addr] and put it at [buffer]
    virtual uint32_t readProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);

    // write [length] bytes of data from [buffer], starting at [addr]
    virtual uint32_t writeProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);

    // verify content using [length] bytes of data from [buffer], starting at [offset]
    virtual boolean verifyProgramPage(uint8_t * buffer, uint32_t addr, uint32_t length);

    // append [length] bytes of data from [buffer] at the end of program content
    virtual uint32_t appendProgramPage(uint8_t * buffer, uint32_t length);
};

#endif /* _STK_GENERICACCESS_H_ */
