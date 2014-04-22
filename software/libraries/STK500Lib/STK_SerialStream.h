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

#ifndef STK_SERIALSTREAM_H_
#define STK_SERIALSTREAM_H_

#include "STK_StreamWrapper.h"

class STK_SerialStream : public STK_StreamWrapper {
private:
  Stream * input;
  uint32_t timeout;
  boolean alwaysflush;

public:
  STK_SerialStream(Stream * input, uint32_t timeout, boolean leading0);
  boolean start();
  void halt();
  uint16_t available();
  void flushInput();
  void flush();
  int16_t read();
  uint16_t fill(uint8_t * buffer, uint16_t maxlen);
  uint16_t fill(uint8_t * buffer, uint16_t maxlen, uint16_t expectedLen);
  void write(uint8_t b);
  void print(const char * str);
  void send(uint8_t * buffer, uint16_t length);
};


#endif /* STK_SERIALSTREAM_H_ */
