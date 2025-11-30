#include "lora.h"
#include "string.h"
#include "stdio.h"
uint8_t Lora_data_at[5];
uint16_t Lora_data_msg[20];
uint8_t Lora_data_powerdown[13];

void Lora_data_init()
{

    Lora_data_at[0] = 'a';
    Lora_data_at[1] = 't';
    Lora_data_at[2] = '\r';
    Lora_data_at[3] = '\n';
    Lora_data_at[4] = '\n';

    Lora_data_msg[0] = 'a';
    Lora_data_msg[1] = 't';
    Lora_data_msg[2] = '+';
    Lora_data_msg[3] = 'm';
    Lora_data_msg[4] = 's';
    Lora_data_msg[5] = 'g';
    Lora_data_msg[6] = 'h';
    Lora_data_msg[7] = 'e';
    Lora_data_msg[8] = 'x';
    Lora_data_msg[9] = '=';

    Lora_data_msg[10] = 0x22;
    // Lora_data_msg[23] = 0x22;
    // Lora_data_msg[24] = '\r';
    // Lora_data_msg[25] = '\n';
    // Lora_data_msg[26] = '\n';

    Lora_data_powerdown[0] = 'a';
    Lora_data_powerdown[1] = 't';
    Lora_data_powerdown[2] = '+';
    Lora_data_powerdown[3] = 'l';
    Lora_data_powerdown[4] = 'o';
    Lora_data_powerdown[5] = 'w';
    Lora_data_powerdown[6] = 'p';
    Lora_data_powerdown[7] = 'o';
    Lora_data_powerdown[8] = 'w';
    Lora_data_powerdown[9] = 'e';
    Lora_data_powerdown[10] = 'r';
    Lora_data_powerdown[11] = '\r';
    Lora_data_powerdown[12] = '\n';
    Lora_data_powerdown[13] = '\n';
}
void Lora_send_Init()
{
    HAL_UART_Transmit(LORA_TX_PORT, Lora_data_at, sizeof(Lora_data_at), HAL_MAX_DELAY);
}

void Lora_powerDown()
{

    HAL_UART_Transmit(LORA_TX_PORT, Lora_data_powerdown, sizeof(Lora_data_powerdown), HAL_MAX_DELAY);
}
void Lora_sendData(float max, float min, float avg)
{
    uint8_t buffer[64];
    uint8_t hex_str[32];
    uint8_t *p = hex_str;

    /* 1. ×10 并向上取整 */
    int16_t vals[3] = {
        (int16_t)ceilf(max * 10.0f),
        (int16_t)ceilf(min * 10.0f),
        (int16_t)ceilf(avg * 10.0f)};

    /* 2. 每个 int16 按小端转 4 个 HEX 字符 */
    for (int i = 0; i < 3; ++i)
    {
        uint8_t *pb = (uint8_t *)&vals[i];
        for (int j = 0; j < 2; ++j)
        { // int16 只有 2 字节
            sprintf((char *)p, "%02X", pb[j]);
            p += 2;
        }
    }
    *p = '\0';

    /* 3. 组包发送 */
    snprintf((char *)buffer, sizeof(buffer), "at+msghex=\"%s\"\r\n\n", hex_str);
    HAL_UART_Transmit(LORA_TX_PORT, buffer, strlen((char *)buffer), HAL_MAX_DELAY);
}