#ifndef _ASIF_PORTS_H_
#define _ASIF_PORTS_H_

#include "asif_config.h"
#include "ASIF_Port_I2CServer.h"
#include "ASIF_Port_Serial.h"
//#include "ports/ASIF_PortClasses.h"


// ******************
// *** IDE STREAM ***
// ******************

#ifdef ASIF_PORTID_IDE
  #ifndef ASIF_IDE_SERIALHARDWARE
    #include <SoftwareSerial.h>
    extern SoftwareSerial asif_ideSerial;
    #define ASIF_IDE_STREAM asif_ideSerial
  #else
    #define ASIF_IDE_STREAM ASIF_IDE_SERIALHARDWARE
  #endif

  extern ASIF_Port_Serial asif_idePort;
#endif

  boolean asif_port_init_ide();
  void asif_port_halt_ide();


// ********************
// *** ENODE STREAM ***
// ********************

#ifdef ASIF_PORTID_ENODE
  #ifndef ASIF_ENODE_SERIALHARDWARE
    #include <SoftwareSerial.h>
    extern SoftwareSerial asif_enodeSerial;
    #define ASIF_ENODE_STREAM asif_enodeSerial
  #else
    #define ASIF_ENODE_STREAM ASIF_ENODE_SERIALHARDWARE
  #endif

  extern ASIF_Port_Serial asif_enodePort;
#endif

  boolean asif_port_init_enode();
  void asif_port_halt_enode();


// *********************
// *** CAMERA STREAM ***
// *********************

#ifdef ASIF_PORTID_CAMERA
  #ifndef ASIF_CAMERA_SERIALHARDWARE
    #include <SoftwareSerial.h>
    extern SoftwareSerial asif_cameraSerial;
    #define ASIF_CAMERA_STREAM asif_cameraSerial
  #else
    #define ASIF_CAMERA_STREAM ASIF_CAMERA_SERIALHARDWARE
  #endif

  extern ASIF_Port_Serial asif_cameraPort;
#endif

  boolean asif_port_init_camera();
  void asif_port_halt_camera();
  
  
// ********************
// *** DEBUG STREAM ***
// ********************

#ifdef ASIF_DEBUGMODE_ON
  #ifdef ASIF_PORTID_DEBUG
    #ifdef ASIF_DEBUG_SERIALHARDWARE
      #define ASIF_DEBUG_STREAM  ASIF_DEBUG_SERIALHARDWARE
    #else /* ASIF_DEBUG_SERIALHARDWARE */
      #include <SoftwareSerial.h>
      extern SoftwareSerial asif_debugSerial;
      #define ASIF_DEBUG_STREAM asif_debugSerial
    #endif /* non ASIF_DEBUG_SERIALHARDWARE */
    
    extern ASIF_Port_Serial asif_debugPort;
  #endif /* ASIF_DEBUGMODE_ON */
#endif /* ASIF_PORTID_DEBUG */

  boolean asif_port_init_debug();
  void asif_port_halt_debug();

// ***************
// *** SENSORS ***
// ***************
  
#ifdef ASIF_PORTID_I2C
  extern ASIF_Port_I2CServer asif_i2cPort;
#endif /* ASIF_PORTID_I2C */

  boolean asif_port_init_i2c();
  void asif_port_halt_i2c();


// ***************
// *** SD CARD ***
// ***************

#ifdef ASIF_PORTID_SPI
  #include <SdFat.h>
  extern SdFat asif_sdPort;
#endif /* ASIF_PORTID_SPI */

  boolean asif_port_init_spi();
  void asif_port_halt_spi();

#endif /* _ASIF_PORTS_H_ */
