/*
********************************************************************

Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com) and Nanosatisfi, Inc. (www.nanosatisfi.com)

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

    Description : wrapper interface for the ports of AS-IF (serial / i2c)
                  this is necessary because different port can need different processing strategies
                  (timeouts, remove leading zeros...)
                  
    Last changed : April 13th, 2014
    
********************************************************************
*/

#ifndef _ASIF_PORTWRAPPER_H_
#define _ASIF_PORTWRAPPER_H_

#define ASIF_PORT_PARAM_REMOVELEADINGZEROS  0x01
#define ASIF_PORT_NOEXPECTEDLEN  0xFFFF
#define ASIF_PORT_NOTIMEOUT      0xFFFFFFFF


class ASIF_PortWrapper {
public:
  virtual boolean start();
  virtual void halt();
  virtual uint16_t available();
  virtual void flushInput();  // erase whatever content is available in the port right now
  virtual void flushOutput();  // erase whatever content is available in the port right now
  virtual int16_t read();	// read 1 byte
  virtual uint16_t fill(uint8_t * buffer, uint16_t maxlen);  // no expectedlen, no timeout, no leading zeros to remove
  virtual uint16_t fill(uint8_t * buffer, uint16_t maxlen, uint16_t expectedLen);
  virtual void print(const char * str);
  virtual void send(uint8_t * buffer, uint16_t length);
};

#endif /* _ASIF_PORTWRAPPER_H_ */
