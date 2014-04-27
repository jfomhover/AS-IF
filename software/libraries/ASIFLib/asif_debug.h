#ifndef _ASIF_DEBUG_H_
#define _ASIF_DEBUG_H_
#include "asif_config.h"
#include "asif_ports.h"

#ifdef ASIF_DEBUGMODE_ON

#define ASIF_DEBUGSRC_CTL		10
#define ASIF_DEBUGSRC_IDE		11
#define ASIF_DEBUGSRC_ENODE		12
#define ASIF_DEBUGSRC_SENSOR	13
#define ASIF_DEBUGSRC_CAMERA	14
#define ASIF_DEBUGSRC_GENERAL	15
#define ASIF_DEBUGSRC_ERROR		16
#define ASIF_DEBUGSRC_WARNING	17
#define ASIF_DEBUGSRC_DEBUG		18
#define ASIF_DEBUGSRC_OUT		19

void asif_debug_header(uint8_t srcid = ASIF_DEBUGSRC_OUT);
void asif_debug_pgm(const prog_uchar *str);
void asif_debug_src(uint8_t srcid);

#define ASIF_DEBUGPGM_GENERALOP(str)	    { asif_debug_header(ASIF_DEBUGSRC_GENERAL); asif_debug_pgm(str); }
#define ASIF_DEBUGPGM_ERROR(str)            { asif_debug_header(ASIF_DEBUGSRC_ERROR); asif_debug_pgm(str); }
#define ASIF_DEBUGPGM_ERRORSRC(src,str)     { asif_debug_header(ASIF_DEBUGSRC_ERROR); asif_debug_src(src); asif_debug_pgm(str); }
#define ASIF_DEBUGPGM_CONTROL_LLOP(src,str) { asif_debug_header(ASIF_DEBUGSRC_CTL); asif_debug_src(src); asif_debug_pgm(str); }
#define ASIF_OUTPUT(str)			 		{ asif_debug_header(ASIF_DEBUGSRC_OUT); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUGPGM_WARNINGSRC(src,str)   { asif_debug_header(ASIF_DEBUGSRC_WARNING); asif_debug_src(src); asif_debug_pgm(str); }
#define ASIF_DEBUGPGM_IDE_LLOP(str)			{ asif_debug_header(ASIF_DEBUGSRC_IDE); asif_debug_pgm(str); }

/*
#define ASIF_DEBUG_CONTROL_LLOP(src,str)  { asif_debug_header(ASIF_DEBUGSRC_CTL); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_IDE_LLOP(src,str)      { asif_debug_header(ASIF_DEBUGSRC_IDE); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_CAM_LLOP(src,str)      { asif_debug_header(ASIF_DEBUGSRC_CAMERA); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_GENERALOP(src,str)     { asif_debug_header(ASIF_DEBUGSRC_GENERAL); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_DEBUGOP(src,str)       { asif_debug_header(ASIF_DEBUGSRC_DEBUG); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_ERROR(src,str)         { asif_debug_header(ASIF_DEBUGSRC_ERROR); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_WARNING(src,str)       { asif_debug_header(ASIF_DEBUGSRC_WARNING); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_OUTPUT(str)			  { asif_debug_header(ASIF_DEBUGSRC_OUT); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_STREAM_FLUSH()		  ASIF_DEBUG_STREAM.flush();

#define ASIF_DEBUG_DUMP(prefix, buf, len)	{ ASIF_DEBUG_STREAM.print("DUMP buf "); \
											ASIF_DEBUG_STREAM.print(len); \
											ASIF_DEBUG_STREAM.print(" = "); \
											  for (uint32_t i=0; i<len; i++) { \
												  if (buf[i] < 0x10) { \
													  ASIF_DEBUG_STREAM.write('0'); \
												  } \
												  ASIF_DEBUG_STREAM.print(buf[i],HEX); \
												  ASIF_DEBUG_STREAM.write(' '); \
											  } 	\
											  ASIF_DEBUG_STREAM.write('\n'); }
*/

#define ASIF_DEBUG_CONTROL_LLOP(src,str)  //{ asif_debug_header(ASIF_DEBUGSRC_CTL); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_IDE_LLOP(src,str)      //{ asif_debug_header(ASIF_DEBUGSRC_IDE); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_CAM_LLOP(src,str)      //{ asif_debug_header(ASIF_DEBUGSRC_CAMERA); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_GENERALOP(src,str)     { asif_debug_header(ASIF_DEBUGSRC_GENERAL); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_DEBUGOP(src,str)       //{ asif_debug_header(ASIF_DEBUGSRC_DEBUG); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_ERROR(src,str)         { asif_debug_header(ASIF_DEBUGSRC_ERROR); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_WARNING(src,str)       { asif_debug_header(ASIF_DEBUGSRC_WARNING); ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_STREAM_FLUSH()		  ASIF_DEBUG_STREAM.flush();


#define ASIF_DEBUG_DUMP(prefix, buf, len)



#else /* ASIF_DEBUGMODE_OFF */

#define ASIF_DEBUG_CONTROL_LLOP(src,str)  //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_IDE_LLOP(src,str)      //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_GENERALOP(src,str)     //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_DEBUGOP(src,str)       //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_ERROR(src,str)         //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }
#define ASIF_DEBUG_WARNING(src,str)       //{ ASIF_DEBUG_STREAM.print(src); ASIF_DEBUG_STREAM.print('#'); ASIF_DEBUG_STREAM.println(str); }

#endif /* ASIF_DEBUGMODE_OFF */


#endif /* _ASIF_DEBUG_H_ */
