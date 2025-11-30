#include "tinycTask.h"
#include "usart.h"
#include "falcon_cmd.h"
#include "tinyAdjust.h"
#include "dcmi.h"
#include "stdio.h"
#include "lora.h"

static uint16_t frameRate = 0;

float_t max_temp;
float_t min_temp;
float_t ave_temp;

uint16_t TINYC_256_SRC_ADDR[TINYC_256_W * TINYC_256_H];

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern DCMI_HandleTypeDef hdcmi;

extern void (*DCMI_IQR)(void);
extern void (*DCMI_ITCB)(void);
static volatile uint8_t frameOk = 0;

volatile uint8_t frameStop;
volatile uint8_t frameCotinue;
volatile uint8_t frames;
volatile uint8_t reline_mode;
PreviewStartParam_t preview_start_param = {0, 0, TINYC_256_W, TINYC_256_H, 25, 0};

float_t max_temp;
float_t min_temp;
float_t ave_temp;

IrRect_t area = {
    .start_point = {0, 0},
    .end_point = {TINYC_256_W, TINYC_256_H},
};
TpdLineRectTempInfo_t info_temp;

void TINYC_256_DCMI_Stop(void)
{
    HAL_DCMI_Stop(&hdcmi);
}

/**
 * @brief
 * @describe
 * @author SmallWhitePassingBy
 * @param buffer_index: 0 or 1
 * @return
 */
void TINYC_256_Start(void)
{
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)TINYC_256_SRC_ADDR, TINYC_256_W * TINYC_256_H * 2 / 4);
}

void _bsp_tinyc_256_irq_all(void)
{
    TINYC_256_DCMI_Stop();
    frameOk = 1;
}

void lcd_fill_tinyc(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    static uint16_t height = 0, width = 0, i = 0;
    width = ex - sx + 1;
    height = ey - sy + 1;
    // color += width * height - width;
#if OS_USED
    if (osMutexWait(displayMuxHandle, 0) == osOK)
    {
#endif
        for (i = 0; i < height; i++)
        {
            // lcd_set_cursor(sx, sy + i);
            // lcd_write_ram_prepare();
            // HAL_DMA_Start(&hdma_memtomem_dma1_stream1, (uint32_t)color, (uint32_t)&LCD->LCD_RAM, width);
            // HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_stream1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
            color += width;
        }
#if OS_USED
        xSemaphoreGive(displayMuxHandle);
    }
#endif
}
static uint8_t redmode = 0;

/**
 * @brief
 * @describe
 * @author SmallWhitePassingBy
 * @param
 * @return 1: ok
 */
uint8_t tinycTaskInit(void)
{
    int8_t ert_cnt = 0;
    Lora_data_init();
power_reset:
    HAL_GPIO_WritePin(POWER_5V0_GPIO_Port, POWER_5V0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER_3V3_GPIO_Port, POWER_3V3_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    int8_t ert = 0;
    HAL_GPIO_WritePin(POWER_5V0_GPIO_Port, POWER_5V0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER_3V3_GPIO_Port, POWER_3V3_Pin, GPIO_PIN_SET);


    HAL_GPIO_WritePin(RST_TINY_GPIO_Port, RST_TINY_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_TINY_GPIO_Port, RST_TINY_Pin, GPIO_PIN_SET);

    OV_OR_TINYC_DCMI_Init(0);
#if OS_USED
    osDelay(600);
#else
    HAL_Delay(1000);
#endif

    ert += vdcmd_init_by_type(VDCMD_I2C_VDCMD);
    if (ert < 0)
    {
        ert_cnt++;
        if (ert_cnt < 5)
            goto power_reset;
        else
        {
            return 0;
        }
    }
    DCMI_IQR = NULL;
    DCMI_ITCB = _bsp_tinyc_256_irq_all;

    // 增加等待时间，确保设备完全准备好
#if OS_USED
    osDelay(200);
#else
    HAL_Delay(1000);
#endif

    ert += preview_start(&preview_start_param);
    if (ert < 0)
    {
        ert_cnt++;
        if (ert_cnt < 5)
            goto power_reset;
        else
        {
            return 0;
        }
    }

    ert += y16_preview_start(PREVIEW_PATH0, Y16_MODE_TEMPERATURE);
    if (ert < 0)
    {
        ert_cnt++;
        if (ert_cnt < 5)
            goto power_reset;
        else
        {
            return 0;
        }
    }
    frameOk = 0;

    ert += read_nuc_parameter();

    if (ert < 0)
    {
        ert_cnt++;
        if (ert_cnt < 5)
            goto power_reset;
        else
        {
            return 0;
        }
    }

    ert += calculate_org_env_cali_parameter();
    if (ert < 0)
    {
        ert_cnt++;
        if (ert_cnt < 5)
            goto power_reset;
        else
        {
            return 0;
        }
    }
    frameRate = 0;

    frameStop = 0;
    frameCotinue = 0;

    TINYC_256_Start();
    // 关闭快门
    set_prop_auto_shutter_params(SHUTTER_PROP_SWITCH, 0);
    // pseudo_color_set(PREVIEW_PATH0,PSEUDO_COLOR_MODE_4);
    return 1;
}

void TINYC_256_Task(void)
{

    if (frameOk)
    {
        frameOk = 0;
        frames++;
        if (frameRate >= OUT_FRRAME_START_INDEX)
        {
            tpd_get_rect_temp_info(area, &info_temp);
            float_t max_temp = (float)info_temp.temp_info_value.max_temp / 16 - 272.5f;
            float_t min_temp = (float)info_temp.temp_info_value.min_temp / 16 - 272.5f;
            float_t ave_temp = (float)info_temp.temp_info_value.ave_temp / 16 - 272.5f;

            // org_kelvin_temp = (float)center_temp / 16;
            // temp_calc_with_new_env_calibration(org_kelvin_temp, &new_kelvin_temp);

            // Lora_send_Init();
            // HAL_Delay(3000);
            // Lora_sendData(max_temp, min_temp, ave_temp);
            // HAL_Delay(3000);
            // Lora_powerDown();
            // HAL_Delay(10);
        }
    }
    else
    {
        frameRate++;
    }
    if (frameStop == 0)
    {
        TINYC_256_Start();

        if (frameCotinue)
        {
            frameCotinue = 0;
            frameRate = 0;
            frameStop = 0;
            TINYC_256_DCMI_Stop();
            TINYC_256_Start();
        }
    }
}
