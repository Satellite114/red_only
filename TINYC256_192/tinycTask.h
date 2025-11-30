#ifndef _TINY_C_TASK_H_
#define _TINY_C_TASK_H_

#include <stdint.h>
#define TINYC_256_W 256
#define TINYC_256_H 192


#define OUT_FRRAME_START_INDEX 4
#define MARK_SIZE 20
#define MARK_BOX_CAL_S(point, size) ((point) > ((size) / 2) ? ((point) - (((size) / 2))) : (0))
#define MARK_BOX_CAL_E(point, size, range) ((point) + ((size) / 2) < (range) ? ((point) + (((size) / 2))) : (range))
uint8_t tinycTaskInit(void);
void TINYC_256_Task(void);
#endif /*_TINY_C_TASK_H_*/