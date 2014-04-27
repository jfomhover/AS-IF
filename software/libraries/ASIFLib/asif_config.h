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

 Description : configuration file for the AS-IF project
 	 	 	   the ATmega2560 section is compliant with the design of the AS-IF pro version
 	 	 	   (see repository)
 Last changed : April 17th, 2014

 ********************************************************************
 */

#ifndef _ASIF_CONFIG_H_
#define _ASIF_CONFIG_H_

// **************
// *** SYSTEM ***
// **************

#ifdef __AVR_ATmega2560__    // Arduino Mega 2560

// *** DEBUG ON/OFF ***
// activate debug by uncommenting the ASIF_DEBUGMODE_ON line
// choose the level of verbosity (see asif_debug.h for effects)

#define ASIF_DEBUGMODE_ON
#define ASIF_DEBUGLEVEL  3

// *** THREADS ID ***
// just FIY, don't modify these

#define ASIF_THREADID_CONTROL  0
#define ASIF_THREADID_IDE      1
#define ASIF_THREADID_ENODE    2
#define ASIF_THREADID_SENSOR   3
#define ASIF_THREADID_CAMERA   4

// *** PORTS AVAILABLE ***
// comment/uncomment one of the following lines to disable/enable the corresponding ports
// note : this also disables the corresponding component / processing thread

#define ASIF_PORTID_IDE    16
#define ASIF_PORTID_ENODE  17
#define ASIF_PORTID_DEBUG  18
#define ASIF_PORTID_I2C    19
#define ASIF_PORTID_SPI    20
//#define ASIF_PORTID_CAMERA 21

// *** SYSTEM SETTINGS ***
// modify at your own risk !!! (plase, just don't)

#define ASIF_RTOS
#define ASIF_CONTROL_BUFFERLEN  256
#define ASIF_DATA_LIMIT    10240
#define ASIF_TIME_LIMIT		21600000 // 6 hours in millis
#define I2C_ADD_ARD1            0x2B    // experiment arduino 1
#define I2C_ADD_ASSV_1          0x04     // master arduino

#define ASIF_PROGRAM_AVAILABLESLOTS  8
//#define ASIF_PROGRAM_USEBRIDGE  // instead of storing HEX code on SD, just forwards it to the node
#define ASIF_CAMERA_ID 0x01

#ifndef ASIF_RTOS
#define ASIF_DELAY(t)  delay(t)
#else
#include <ChibiOS_AVR.h>
#define ASIF_DELAY(t)  chThdSleep(t)
#endif

#endif


#ifdef __AVR_ATmega328P__    // Arduino Uno

// *** DEBUG ON/OFF ***
// activate debug by uncommenting the ASIF_DEBUGMODE_ON line
// choose the level of verbosity (see asif_debug.h for effects)

#define ASIF_DEBUGMODE_ON
#define ASIF_DEBUGLEVEL  3

// *** THREADS ID ***
// just FIY, don't modify these

#define ASIF_THREADID_CONTROL  0
#define ASIF_THREADID_IDE      1
#define ASIF_THREADID_ENODE    2
#define ASIF_THREADID_SENSOR   3
//#define ASIF_THREADID_CAMERA   4

// *** PORTS AVAILABLE ***
// comment/uncomment one of the following lines to disable/enable the corresponding ports
// note : this also disables the corresponding component / processing thread

//#define ASIF_PORTID_IDE    16	// serial
//#define ASIF_PORTID_ENODE  17	// serial
#define ASIF_PORTID_DEBUG  18	// serial
#define ASIF_PORTID_I2C    19	// I2C
#define ASIF_PORTID_SPI    20	// SPI
//#define ASIF_PORTID_CAMERA 21	// serial

#define ASIF_ENODE_RST     6
#define ASIF_IDE_RST       5

// *** SYSTEM SETTINGS ***
// modify at your own risk !!! (plase, just don't)

//#define ASIF_RTOS
#define ASIF_CONTROL_BUFFERLEN  32
#define ASIF_DATA_LIMIT    10240
#define ASIF_TIME_LIMIT		21600000 // 6 hours in millis
#define I2C_ADD_ARD1            0x2B    // experiment arduino 1
#define I2C_ADD_ASSV_1          0x04     // master arduino

#define ASIF_PROGRAM_AVAILABLESLOTS  1
#define ASIF_PROGRAM_USEBRIDGE  // instead of storing HEX code on SD, just forwards it to the node
//#define ASIF_CAMERA_ID 0x01

#ifndef ASIF_RTOS
#define ASIF_DELAY(t)  delay(t)
#else
#include <ChibiOS_AVR.h>
#define ASIF_DELAY(t)  chThdSleep(t)
#endif

#endif

// ***********************************
// *** PLATFORM DEPENDANT SETTINGS ***
// ***********************************

// -------------------
// --- ARDUINO UNO ---
// -------------------

#ifdef __AVR_ATmega328P__    // Arduino Uno

// *** IDE STREAM ***

#ifdef ASIF_PORTID_IDE  // if not defined, do not init/instance

//#define ASIF_IDE_SERIALHARDWARE  Serial2
#define ASIF_IDE_RX        6
#define ASIF_IDE_TX        7
#define ASIF_IDE_RST       5
#define ASIF_IDE_BAUDRATE  115200
#define ASIF_IDE_BUFFERLEN 140
#define ASIF_IDE_SERIAL_TIMEOUT  100

#endif /* ASIF_PORTID_IDE */

// *** ENODE STREAM ***

#ifdef ASIF_PORTID_ENODE  // if not defined, do not init/instance

//#define ASIF_ENODE_SERIALHARDWARE  Serial1
#define ASIF_ENODE_RX        2
#define ASIF_ENODE_TX        3
#define ASIF_ENODE_RST       5

#define ASIF_ENODE_PAGE_SIZE       128
#define ASIF_ENODE_SERIAL_TIMEOUT  100
#define ASIF_ENODE_BAUDRATE        9600
#define ASIF_ENODE_BUFFERLEN       140

#endif /* ASIF_PORTID_ENODE */

// *** CAMERA STREAM ***

#ifdef ASIF_PORTID_CAMERA  // if not defined, do not init/instance

#define ASIF_CAMERA_SERIALHARDWARE  Serial3
//    #define ASIF_CAMERA_RX        6
//    #define ASIF_CAMERA_TX        7
#define ASIF_CAMERA_BAUDRATE  9600
#define ASIF_CAMERA_SERIAL_TIMEOUT  20

#endif /* ASIF_PORTID_CAMERA */

// *** DEBUG STREAM ***

#ifdef ASIF_PORTID_DEBUG

#define ASIF_DEBUG_SERIALHARDWARE  Serial
//#define ASIF_DEBUG_RX        6
//#define ASIF_DEBUG_TX        7
#define ASIF_DEBUG_BAUDRATE  115200
#define ASIF_DEBUG_SERIAL_TIMEOUT  20

#endif /* ASIF_PORTID_DEBUG */


// *** SENSORS / I2C ***

#ifdef ASIF_PORTID_I2C

#define I2C_ADD_ARD1            0x2B    // experiment arduino 1
#define I2C_ADD_ASSV_1          0x04     // master arduino

// note : put here only for being helpful ^^, everything handled by Wire library
#define ASIF_SENSOR_SDA  A4
#define ASIF_SENSOR_SCL  A5

#endif /* ASIF_PORTID_I2C */

// *** SD CARD ***

#ifdef ASIF_PORTID_SPI

#define ASIF_SD_CSPIN    4  // -> CS of the adafruit SD breakbout board

// note : the three below are put here only for being helpful ^^, everything handled by SPI library
#define ASIF_SD_MISO     12  // -> DO of the adafruit SD breakbout board
#define ASIF_SD_MOSI     11  // -> DI of the adafruit SD breakbout board
#define ASIF_SD_SCK      13  // -> CLK of the adafruit SD breakbout board

#endif /* ASIF_PORTID_SPI */

#endif /* ARDUINO UNO END OF CONFIGURATION */

// -------------------------
// --- ARDUINO MEGA 2560 ---
// -------------------------

#ifdef __AVR_ATmega2560__    // Arduino Mega 2560

// *** IDE STREAM ***

#ifdef ASIF_PORTID_IDE  // if not defined, do not init/instance

#define ASIF_IDE_SERIALHARDWARE  Serial2
#define ASIF_IDE_RST       3
#define ASIF_IDE_BAUDRATE  115200
#define ASIF_IDE_BUFFERLEN 140
#define ASIF_IDE_SERIAL_TIMEOUT  100

#endif /* ASIF_PORTID_IDE */

// *** ENODE STREAM ***

#ifdef ASIF_PORTID_ENODE  // if not defined, do not init/instance

#define ASIF_ENODE_SERIALHARDWARE  Serial1
#define ASIF_ENODE_RST             2

#define ASIF_ENODE_PAGE_SIZE       128
#define ASIF_ENODE_SERIAL_TIMEOUT  100
#define ASIF_ENODE_BAUDRATE        9600
#define ASIF_ENODE_BUFFERLEN       140

#endif /* ASIF_PORTID_ENODE */

// *** CAMERA STREAM ***

#ifdef ASIF_PORTID_CAMERA  // if not defined, do not init/instance

#define ASIF_CAMERA_SERIALHARDWARE  Serial3
//    #define ASIF_CAMERA_RX        6
//    #define ASIF_CAMERA_TX        7
#define ASIF_CAMERA_BAUDRATE  9600
#define ASIF_CAMERA_SERIAL_TIMEOUT  20

#endif /* ASIF_PORTID_CAMERA */

// *** DEBUG STREAM ***

#ifdef ASIF_PORTID_DEBUG

#define ASIF_DEBUG_SERIALHARDWARE  Serial
//#define ASIF_DEBUG_RX        6
//#define ASIF_DEBUG_TX        7
#define ASIF_DEBUG_BAUDRATE  115200
#define ASIF_DEBUG_SERIAL_TIMEOUT  20

#endif /* ASIF_PORTID_DEBUG */

// *** SENSORS / I2C ***

#ifdef ASIF_PORTID_I2C

#define I2C_ADD_ARD1            0x2B    // experiment arduino 1
#define I2C_ADD_ASSV_1          0x04     // master arduino

// note : put here only for being helpful ^^, everything handled by Wire library
#define ASIF_SENSOR_SDA  20
#define ASIF_SENSOR_SCL  21

#endif /* ASIF_PORTID_I2C */

// *** SD CARD ***

#ifdef ASIF_PORTID_SPI

#define ASIF_SD_CSPIN    53  // -> CS of the adafruit SD breakbout board

// note : the three below are put here only for being helpful ^^, everything handled by SPI library
#define ASIF_SD_MISO     50  // -> DO of the adafruit SD breakbout board
#define ASIF_SD_MOSI     51  // -> DI of the adafruit SD breakbout board
#define ASIF_SD_SCK      52  // -> CLK of the adafruit SD breakbout board

#endif /* ASIF_PORTID_SPI */

#endif /* ARDUINO MEGA 2560 END OF CONFIGURATION */

#endif /* _ASIF_CONFIG_H_ */
