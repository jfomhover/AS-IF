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
#include "stk_defs.h"
#include "stk_util.h"

boolean stk_isAlphaNum(byte b) {
  if ((b >= 33) && (b <= 127))
    return(true);
  return(false);
};

void _transform_BE2LE(STK_Parameters * params) {
  uint8_t * t_ptr = (uint8_t *)params;
  uint8_t b = 0x00;
  
  b = t_ptr[8];
  t_ptr[8] = t_ptr[9];
  t_ptr[9] = b;
  
  b = t_ptr[10];
  t_ptr[10] = t_ptr[11];
  t_ptr[11] = b;
  
  b = t_ptr[12];
  t_ptr[12] = t_ptr[13];
  t_ptr[13] = b;

  b = t_ptr[14];
  t_ptr[14] = t_ptr[15];
  t_ptr[15] = b;

  b = t_ptr[16];  // uint32_t
  t_ptr[16] = t_ptr[19];
  t_ptr[19] = b;
  b = t_ptr[17];  // uint32_t
  t_ptr[17] = t_ptr[18];
  t_ptr[18] = b;
};

