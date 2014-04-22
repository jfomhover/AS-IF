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
#include "STK500Lib.h"

// NOTE : made this system below in order to be able to change that from any library
// for different purposes, instead of changing some configuration file

struct STK500LibStruct STK500Lib = {
	&Serial,
	&delay,
	50,
	50
};


void STK500Lib_init() {
	STK500Lib.STKDebugStream = &Serial;
	STK500Lib._delay = &delay;
	STK500Lib.inputTimeout = 50;
	STK500Lib.outputTimeout = 50;
};

void _stk_dumparray(const char * prefix, byte * ptr, int len) {
	STK_DEBUG_PRINT(prefix);
	STK_DEBUG_PRINT(" buf[");
	STK_DEBUG_PRINT(len);
	STK_DEBUG_PRINT("] = ");

	for (int i=0; i<len; i++) {
		if (ptr[i] < 0x10)
			STK_DEBUG_PRINT('0');
		STK_DEBUG_PRINT2(ptr[i],HEX);
		STK_DEBUG_PRINT(' ');
	}
	STK_DEBUG_PRINT('\n');
}
