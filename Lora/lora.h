#ifndef LORA_H
#define LORA_H
#include "main.h"
#include "usart.h"

#define LORA_TX_PORT &huart3

// #define LORA_ADDR_AT 0x24000000
// #define LORA_ADDR_MSG 0x24000400
// #define LORA_ADDR_POWERDOWN 0x24000800

void Lora_data_init(void);
void Lora_send_Init(void);
void Lora_sendData(float max, float min, float avg);
void Lora_powerDown(void);

#endif //LORA_H