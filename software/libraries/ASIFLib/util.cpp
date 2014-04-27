/*
 * util.c
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#include <Arduino.h>
#include <util/crc16.h>

#define CRC16 0x8005

// note : had issue with crc16... the function below is soon to be obsolete
uint16_t asif_checksum(const uint8_t * data, uint32_t size) {
	uint16_t t_crc = 0xFFFF;

	for (uint32_t i=0; i<size; i++) {
		t_crc = _crc16_update(t_crc, data[i]);
	}

	return(t_crc);
}
