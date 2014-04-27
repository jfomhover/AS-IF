#include <Arduino.h>

#include "asif_config.h"
#include "asif_debug.h"
#include "ASIF_ThreadI2CSensor.h"
#include "asif_ports.h"
#include "ASIF_Port_I2CServer.h"
//#include "ports/asif_i2C_server.h"
#include "ASIF_Control.h"
#include "nanosat_message.h"

void ASIF_ThreadI2CSensor::setup() {
//    asif_port_init_sensors();
//    asif_i2c_server_launch();
	threadId = ASIF_THREADID_SENSOR;
}
;

boolean ASIF_ThreadI2CSensor::isTriggered() {
	return (asif_i2cPort.available() > 0);
}
;

void ASIF_ThreadI2CSensor::trigger() {

	if (isTriggered()) {
		uint16_t t_len = asif_i2cPort.fill((uint8_t *) message,
				sizeof(nanosat_message_t));
		if (t_len > 0) {
			uint16_t t_pgm_id = ASIF_Control.getRunningProgram();

			if (t_pgm_id != ASIF_PGMID_NULL) {
				outfile.setSourceId(threadId);
				outfile.setFileId(t_pgm_id | ASIF_FS_DATMASK);
				nanosat_message_t * t_ptr = (nanosat_message_t *) message;

				switch (t_ptr->type) {
				case EXIT:
					ASIF_Control.endProgram(threadId, t_pgm_id);
					break;
				case APPEND:
					outfile.appendData(t_ptr->buf, t_ptr->len); // lockSD inside
					ASIF_DEBUG_DEBUGOP("data", t_ptr->len);
					if (outfile.getDataSize() > ASIF_DATA_LIMIT)
						ASIF_Control.endProgram(threadId, t_pgm_id);
					break;
				case CAM:
					ASIF_Control.unlockSnapshot();
					ASIF_Control.endProgram(threadId, t_pgm_id);
					break;
				default:
					break;
				};
			}
		}
	}
}
;

