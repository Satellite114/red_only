#ifndef _TINY_C_TASK_H_
#define _TINY_C_TASK_H_

#include <stdint.h>
#define TINYC_256_W 256
#define TINYC_256_H 192

#define TINYC_FRAMES_PER_CYCLE 1
#define TINYC_USE_SLEEP 1
#define TINYC_SLEEP_TIME_MS 10000
#define TINYC_USE_LCD 0
#define SLEEP_TEST 0

#define OUT_FRRAME_START_INDEX 4
#define MARK_SIZE 20
#define MARK_BOX_CAL_S(point, size) ((point) > ((size) / 2) ? ((point) - (((size) / 2))) : (0))
#define MARK_BOX_CAL_E(point, size, range) ((point) + ((size) / 2) < (range) ? ((point) + (((size) / 2))) : (range))
uint8_t tinycTaskInit(void);
void TINYC_256_Task(void);
 void tinyc_low_power_delay(uint32_t s);
 void GPIO_All_Analog(void);
#endif /*_TINY_C_TASK_H_*/