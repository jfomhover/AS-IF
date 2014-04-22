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

#ifndef _STK_PROGRAMMER_H_
#define _STK_PROGRAMMER_H_

class STK_Programmer {
    virtual boolean stk_getsync();
    virtual boolean stk_setdeviceparameters();
    virtual boolean stk_universal(uint8_t * sequence4bytes, uint8_t answer);
    virtual boolean stk_enterprogmode();
    virtual boolean stk_leaveprogmode();
    virtual boolean stk_loadaddress(uint16_t addr);  // addr is given in BYTES (not WORDS)
    virtual boolean stk_progpage(uint8_t * page, uint16_t len);
    virtual boolean stk_readpage(uint16_t len);
    virtual boolean stk_getsignature();
    virtual boolean verifyUniversal(uint8_t b);
    virtual boolean verifyPage(uint8_t * page, uint16_t len);
    virtual boolean verifySignature(uint8_t a, uint8_t b, uint8_t c);
};

#endif /* _STK_PROGRAMMER_H_ */
