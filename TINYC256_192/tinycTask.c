#include "tinycTask.h"
#include "usart.h"
#include "falcon_cmd.h"
#include "tinyAdjust.h"
#include "dcmi.h"
#include "stdio.h"
#include "lora.h"
#include "st7789.h"
#include "utils.h"

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

#define LCD_THERM_W ST7789_WIDTH
#define LCD_THERM_H 180

static uint16_t tinyc_gray_scaled[LCD_THERM_W * LCD_THERM_H];
static uint16_t tinyc_rgb_scaled[LCD_THERM_W * LCD_THERM_H];

static uint16_t tinyc_temp_to_color(uint16_t raw, uint16_t t_min, uint16_t t_max)
{
    if (t_max <= t_min)
    {
        return BLACK;
    }

    uint32_t v = raw;
    if (v <= t_min)
    {
        v = t_min;
    }
    else if (v >= t_max)
    {
        v = t_max;
    }

    uint32_t range = (uint32_t)t_max - (uint32_t)t_min;
    if (range == 0)
    {
        return BLACK;
    }

    uint32_t level = (v - t_min) * 255u / range;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    if (level < 64)
    {
        r = 0;
        g = 0;
        b = (uint8_t)(level * 4u);
    }
    else if (level < 128)
    {
        r = 0;
        g = (uint8_t)((level - 64u) * 4u);
        b = 255u;
    }
    else if (level < 192)
    {
        r = (uint8_t)((level - 128u) * 4u);
        g = 255u;
        b = (uint8_t)(255u - (level - 128u) * 4u);
    }
    else
    {
        r = 255u;
        g = (uint8_t)(255u - (level - 192u) * 4u);
        b = 0;
    }

    uint16_t r5 = (uint16_t)(r >> 3);
    uint16_t g6 = (uint16_t)(g >> 2);
    uint16_t b5 = (uint16_t)(b >> 3);

    return (uint16_t)((r5 << 11) | (g6 << 5) | b5);
}

static void tinyc_render_frame_to_lcd(void)
{
    grayImageLarger(TINYC_256_SRC_ADDR,
                    tinyc_gray_scaled,
                    TINYC_256_W,
                    TINYC_256_H,
                    LCD_THERM_W,
                    LCD_THERM_H);

    grayImageToRGB(tinyc_gray_scaled,
                   tinyc_rgb_scaled,
                   LCD_THERM_W,
                   LCD_THERM_H);

    uint16_t y_offset = (uint16_t)((ST7789_HEIGHT - LCD_THERM_H) / 2u);

    for (uint16_t dy = 0; dy < LCD_THERM_H; dy++)
    {
        uint16_t *line = &tinyc_rgb_scaled[(uint32_t)dy * (uint32_t)LCD_THERM_W];
        ST7789_DrawImage(0, (uint16_t)(y_offset + dy), LCD_THERM_W, 1, line);
    }
}

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
            (void)max_temp;
            (void)min_temp;
            (void)ave_temp;

            tinyc_render_frame_to_lcd();
        }
    }
    else
    {
        frameRate++;
    }
}
