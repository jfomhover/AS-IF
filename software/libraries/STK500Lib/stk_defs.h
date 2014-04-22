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

#ifndef _STK_DEFS_H_
#define _STK_DEFS_H_

// CONFIG

// for config, see STK500Lib.h file
#include "STK500Lib.h"

// DEFINITIONS

#define STK_HWVER 2
#define STK_SWMAJ 1
#define STK_SWMIN 18

// STK Definitions
#define STK_OK              0x10
#define STK_FAILED          0x11
#define STK_UNKNOWN         0x12
#define STK_INSYNC          0x14
#define STK_NOSYNC          0x15
#define CRC_EOP             0x20 //ok it is a space...

#define STK_GET_SYNC        0x30 // '0'
#define STK_GET_SIGN_ON     0x31 // '1'

#define STK_GET_PARAMETER   0x41 // 'A' (gets version)
#define STK_SET_PARAMETER   0x40
#define STK_SET_DEVICE      0x42 // 'B' set the programming parameters
#define STK_SET_DEVICE_EXT  0x45 // 'E' extended params

#define STK_ENTER_PROGMODE  0x50 // 'P'
#define STK_LEAVE_PROGMODE  0x51 // 'Q'
#define STK_CHIP_ERASE      0x52 // 'R'
#define STK_LOAD_ADDRESS    0x55 // 'U'
#define STK_UNIVERSAL       0x56 // 'V'

#define STK_PROG_FLASH      0x60
#define STK_PROG_DATA       0x61 // 'a'
#define STK_PROG_FUSE       0x62 // 'b'
#define STK_PROG_FUSE_EXT   0x65 // 'e'
#define STK_PROG_LOCK       0x63 // 'c'
#define STK_PROG_PAGE       0x64 // 'd'

#define STK_PAGE_FLASH      'F'
#define STK_PAGE_EEPROM     'E'

#define STK_READ_FLASH      0x70 // 'p'
#define STK_READ_DATA       0x71 // 'q'
#define STK_READ_FUSE       0x72 // 'r'
#define STK_READ_LOCK       0x73 // 's'
#define STK_READ_PAGE       0x74 // 't'
#define STK_READ_SIGN       0x75 // 'u'
#define STK_READ_FUSE_EXT   0x77 // 'w'

// for command STK_SET_DEVICE
typedef struct _stk_param {
  uint8_t devicecode;   // Device code as defined in â€œdevices.hâ€�
  uint8_t revision;     // Device revision. Currently not used. Should be set to 0.
  uint8_t progtype;     // Defines which Program modes is supported:
                        //  â€œ0â€� â€“ Both Parallel/High-voltage and Serial mode
                        //  â€œ1â€� â€“ Only Parallel/High-voltage mode
  uint8_t parmode;      // Defines if the device has a full parallel interface or a pseudo parallel programming interface:
                        //  â€œ0â€� â€“ Pseudo parallel interface
                        //  â€œ1â€� â€“ Full parallel interface
  uint8_t polling;      // Defines if polling may be used during SPI access:
                        //  â€œ0â€� â€“ No polling may be used
                        //  â€œ1â€� â€“ Polling may be used
  uint8_t selftimed;    // Defines if programming instructions are self timed:
                        //  â€œ0â€� â€“ Not self timed
                        //  â€œ1â€� â€“ Self timed
  uint8_t lockbytes;    // Number of Lock bytes. Currently not used. Should be set  to actual number of Lock bytes for future compability
  uint8_t fusebytes;    // Number of Fuse bytes. Currently not used. Should be set to actual number of Fuse bytes for future caompability.
  uint16_t flashpoll;   // [BIG ENDIAN!] FLASH polling value. See Data Sheet for the device
  uint16_t eeprompoll;  // [BIG ENDIAN!] EEPROM polling value. See data sheet for the device
  uint16_t pagesize;    // [BIG ENDIAN!] Page size in bytes for pagemode parts
  uint16_t eepromsize;  // [BIG ENDIAN!] EEPROM size in bytes
  uint32_t flashsize;   // [BIG ENDIAN!] FLASH size in bytes
} STK_Parameters;


typedef struct _stk_exp_param {
  uint8_t commandsize;    // defines how many bytes of additional parameters the command contains
  uint8_t eeprompagesize; // EEPROM page size in bytes
  uint8_t signalpagel;    // defines to which port pin the PAGEL signal should be mapped
  uint8_t signalbs2;      // defines to which port pin the BS2 signal should be mapped
  uint8_t reset_disable;  // defines whether a part has RSTDSBL Fuse (value=1) or not (value=0)
} STK_ExtParameters;

#endif /* _STK_DEFS_H_ */
