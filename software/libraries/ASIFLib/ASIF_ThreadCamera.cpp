/*
 * ASIF_ThreadCamera.cpp
 *
 *  Created on: 15 avr. 2014
 *      Author: jeff
 */

#ifdef ASIF_PORTID_CAMERA

#include <Arduino.h>
#include "ASIF_ThreadCamera.h"
#include <ZMCameraLib.h>
#include "asif_ports.h"
#include "ASIF_Control.h"
#include "asif_debug.h"



ZMCamera cam(&ASIF_CAMERA_STREAM);

void ASIF_ThreadCamera::setup() {
	cam.setDebugStream(&ASIF_DEBUG_STREAM);
	cam.setDebug(true);
	cam.setID(ASIF_CAMERA_ID);

	threadId = ASIF_THREADID_CAMERA;
}

boolean ASIF_ThreadCamera::isTriggered() {
	return (true);
}

void ASIF_ThreadCamera::trigger() {
//	if (isTriggered()) {
	ASIF_Control.lockSnapshot();

	uint16_t t_pgm_id = ASIF_Control.getRunningProgram();

	if (t_pgm_id != ASIF_PGMID_NULL) {
		ASIF_Control.lockPort(ASIF_PORTID_CAMERA);

		cam.setDebug(true);

		// just checking if the cam is answering
		// she will provide a string indicating its version
		ASIF_DEBUG_CAM_LLOP(threadId, "\n*** HELLO CAM\n");
		if (cam.helloCam() <= 0) {
			ASIF_DEBUG_ERROR(threadId, "cam unreachable");
			ASIF_Control.unlockPort(ASIF_PORTID_CAMERA);
			return;  // no need to continue beyond that point, sorry !
		}

		// change the compression ratio of the camera (JPEG)
		ASIF_DEBUG_CAM_LLOP(threadId, "\n*** CHANGE COMPRESSION RATIO\n");
		if (cam.changeCompressionRatio(100)) {
			ASIF_DEBUG_CAM_LLOP(threadId, "OK");
		} else {
			ASIF_DEBUG_CAM_LLOP(threadId, "NOT OK");
		}

		// ASK FOR A CAPTURE BY THE CAMERA
		// all results will be stored on the camera, and you'll have to query for "packages"
		// (smaller bits of data that you have to aggregate afterwards to reconstitute the JPEG file
		ASIF_DEBUG_CAM_LLOP(threadId, "\n*** CAPTURE PICTURE\n");
		if (cam.captureImage(ZM_PICTURE_SIZE_1280X1024, 196)) {
			ASIF_DEBUG_CAM_LLOP(threadId, "OK");
		} else {
			ASIF_DEBUG_CAM_LLOP(threadId, "NOT OK");
		}

		ASIF_DEBUG_CAM_LLOP("snapshot size: ", cam.getSnapshotSize());

		uint16_t t_count = cam.getPackageCount();
		ASIF_DEBUG_CAM_LLOP("package count: ", cam.getPackageCount());

		// ASK FOR THE PACKAGES AND REBUILD THE JPEG FILE PACKAGE BY PACKAGE
		// you're lucky, I've done a function that does everything ^^
		ASIF_DEBUG_CAM_LLOP(threadId, "*** GET ALL PACKAGES");
		cam.setDebug(false); // cause there is a LOT of transfers during this sequence
		uint16_t t_filesize = 0; //cam.processAllPackagesToFile(&myFile); // see ZMCameraLib/ZMCameraLib.cpp

		// if the result size is null it means there has been an error in the process
		if (t_filesize == 0) {
			ASIF_DEBUG_CAM_LLOP(threadId, "error");
			ASIF_DEBUG_CAM_LLOP(threadId, cam.getErrorCode());
		} else {
			// outputs the size of the data received
			ASIF_DEBUG_CAM_LLOP("received ", t_filesize);
			// to be compared to the size of the data expected
//				Serial.print(", orig snapshot size: ");
//				Serial.println(cam.getSnapshotSize());
		}

//			myFile.close();

		ASIF_Control.unlockPort(ASIF_PORTID_CAMERA);

	}
}

#endif /* ASIF_PORTID_CAMERA */
