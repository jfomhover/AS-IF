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

#ifndef _STK500LIB_H_
#define _STK500LIB_H_

// CONFIG
#include <Arduino.h>

#define STK_PROG_PAGE_SIZE  128


//#define STK_DELAY(t)  ASIF_DELAY(t)

#define STK_DEBUG

// NOTE : made this system below in order to be able to change that from any library
// for different purposes, instead of changing some configuration file

struct STK500LibStruct {
	Stream * STKDebugStream;	// default : Serial
	void (* _delay)(uint32_t);	// default : delay
	uint32_t inputTimeout;		// default : 50
	uint32_t outputTimeout;		// default : 50
};

extern struct STK500LibStruct STK500Lib;

void STK500Lib_init();

void _stk_dumparray(const char * prefix, byte * ptr, int len);

#define STK_DELAY(t) 				STK500Lib._delay(t)
#define STK_INPUTSERIAL_TIMEOUT  	STK500Lib.inputTimeout
#define STK_OUTPUTSERIAL_TIMEOUT	STK500Lib.outputTimeout

#ifdef STK_DEBUG
#define STK_DEBUG_PRINT(arg)				STK500Lib.STKDebugStream->print(arg)
#define STK_DEBUG_PRINT2(arg1,arg2)			STK500Lib.STKDebugStream->print(arg1,arg2)
#define STK_DEBUG_PRINTLN(arg)				STK500Lib.STKDebugStream->println(arg)
#define STK_DEBUG_PRINTLN2(arg1,arg2)		STK500Lib.STKDebugStream->println(arg1,arg2)
#define STK_DEBUG_BUFFER(prefix, ptr, len)	_stk_dumparray(prefix, ptr, len)
#else
#define STK_DEBUG_PRINT(arg)
#define STK_DEBUG_PRINT2(arg1,arg2)
#define STK_DEBUG_PRINTLN(arg)
#define STK_DEBUG_PRINTLN2(arg1,arg2)
#define STK_DEBUG_BUFFER(prefix, ptr, len)
#endif

#endif /* _STK500LIB_H_ */
