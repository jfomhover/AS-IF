#ifndef _ASIF_ENODEPROGRAM_H_
#define _ASIF_ENODEPROGRAM_H_

#define ASIF_PGMID_NULL       0x7FFF

enum _enode_programstate{
  PGM_FREE,
  PGM_ALLOCATED,
  PGM_RECEIVING,
  PGM_READY_FOR_UPLOAD,
  PGM_UPLOADING,
  PGM_ERROR_UPLOADING,
  PGM_RUNNING,
  PGM_TERMINATED
};

typedef struct _enode_program {
  uint16_t pgm_id;
  _enode_programstate state;
  uint32_t started;
} ASIF_ENodeProgram;

#endif /* _ASIF_ENODEPROGRAM_H_ */
