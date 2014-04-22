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

#ifndef STK_STREAMWRAPPER_H_
#define STK_STREAMWRAPPER_H_

class STK_StreamWrapper {
public:
	virtual uint16_t available();
	virtual int16_t read();	// read 1 byte
	virtual uint16_t fill(uint8_t * buffer, uint16_t maxlen); // no expectedlen, no timeout, no leading zeros to remove
	virtual uint16_t fill(uint8_t * buffer, uint16_t maxlen,
			uint16_t expectedLen);
	virtual void write(uint8_t b);
	virtual void print(const char * str);
	virtual void send(uint8_t * buffer, uint16_t length);
};

#endif /* STK_STREAMWRAPPER_H_ */
