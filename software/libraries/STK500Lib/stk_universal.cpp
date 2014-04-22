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

byte _universalarray[5*8] = {
 0x30, 0x00, 0x00, 0x00, 0x1E,
 0x30, 0x00, 0x01, 0x00, 0x95,
 0x30, 0x00, 0x02, 0x00, 0x0F,
 0xA0, 0x03, 0xFC, 0x00, 0xFF,
 0xA0, 0x03, 0xFD, 0x00, 0xFF,
 0xA0, 0x03, 0xFE, 0x00, 0xFF,
 0xA0, 0x03, 0xFF, 0x00, 0xFF,
 0xAC, 0x80, 0x00, 0x00, 0x00
};

boolean array_equal(byte * request, byte * array) {
  for (int i=0; i<4; i++)
    if (request[i] != array[i])
      return(false);
  return(true);
};

byte STK_request_universal(byte * request) {
  for (int i=0; i<8; i++) {
    if (array_equal(request, _universalarray+5*i))
      return(_universalarray[5*i+4]);
  }
  return(0x00);
};

