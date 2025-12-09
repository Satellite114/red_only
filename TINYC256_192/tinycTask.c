#include "tinycTask.h"
#include "usart.h"
#include "falcon_cmd.h"
#include "tinyAdjust.h"
#include "dcmi.h"
#include "stdio.h"
#include "lora.h"
#include "st7789.h"
#include "utils.h"
#include "rtc.h"
#include "tim.h"

extern void SystemClock_Config(void);

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

#define LCD_THERM_W 256
#define LCD_THERM_H 192

static uint16_t tinyc_gray_scaled[LCD_THERM_W * LCD_THERM_H];
static uint16_t tinyc_rgb_scaled[LCD_THERM_W * LCD_THERM_H];
static uint16_t tinyc_rgb_rotated[LCD_THERM_W * LCD_THERM_H];

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

    // 顺时针旋转90度后，尺寸从 LCD_THERM_W x LCD_THERM_H 变为 LCD_THERM_H x LCD_THERM_W
    rgb565Rotate90(tinyc_rgb_scaled,
                   tinyc_rgb_rotated,
                   LCD_THERM_W,
                   LCD_THERM_H);

    uint16_t rot_w = LCD_THERM_H;      // 180
    uint16_t rot_h = LCD_THERM_W;      // 240
    uint16_t x_offset = (uint16_t)((ST7789_WIDTH - rot_w) / 2u);   // 水平居中
    uint16_t y_offset = (uint16_t)((ST7789_HEIGHT - rot_h) / 2u);  // 垂直居中

    for (uint16_t dy = 0; dy < rot_h; dy++)
    {
        uint16_t *line = &tinyc_rgb_rotated[(uint32_t)dy * (uint32_t)rot_w];
        ST7789_DrawImage((uint16_t)(x_offset),
                         (uint16_t)(y_offset + dy),
                         rot_w,
                         1,
                         line);
    }
}

static void tinyc_draw_temp_overlay(float_t max_temp, float_t min_temp, float_t ave_temp)
{
    char buf[32];

    /* 清除顶部空白区域，避免文字残影：高度取 32 像素 */
    ST7789_DrawFilledRectangle(0, 0, ST7789_WIDTH, 32, BLACK);

    /* 显示最大温度 */
    (void)snprintf(buf, sizeof(buf), "MAX:%4.1fC", (double)max_temp);
    ST7789_WriteString(0, 0, buf, Font_11x18, YELLOW, BLACK);

    /* 显示最小温度 */
    (void)snprintf(buf, sizeof(buf), "MIN:%4.1fC", (double)min_temp);
    ST7789_WriteString(0, 16, buf, Font_11x18, CYAN, BLACK);

    /* 显示平均温度（第二行偏右一些） */
    (void)snprintf(buf, sizeof(buf), "AVE:%4.1fC", (double)ave_temp);
    ST7789_WriteString(120, 0, buf, Font_11x18, GREEN, BLACK);
}

void TINYC_256_DCMI_Stop(void)
{
    HAL_DCMI_Stop(&hdcmi);
}

static uint32_t tinyc_frames_in_cycle = 0;

static volatile uint8_t tinyc_rtc_wakeup = 0;

static void tinyc_send_cycle_info(float_t max_temp, float_t min_temp, float_t ave_temp)
{
    Lora_send_Init();
    HAL_Delay(3000);
    Lora_sendData(max_temp, min_temp, ave_temp);
    HAL_Delay(3000);
    Lora_powerDown();
    HAL_Delay(10);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    if (hrtc->Instance == RTC)
    {
        tinyc_rtc_wakeup = 1;
    }
}

 void tinyc_low_power_delay(uint32_t ms)
{
#if TINYC_USE_SLEEP
    uint32_t seconds = ms / 1000U;
    if (seconds == 0U)
    {
        seconds = 1U;
    }

    tinyc_rtc_wakeup = 0;
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, seconds, RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0) != HAL_OK)
    {
        return;
    }

    /* Power down camera and LCD before entering low power */
    HAL_GPIO_WritePin(POWER_5V0_GPIO_Port, POWER_5V0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER_3V3_GPIO_Port, POWER_3V3_Pin, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

    SystemClock_Config();

    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    /* Restore camera and LCD power after wake-up */
    HAL_GPIO_WritePin(POWER_5V0_GPIO_Port, POWER_5V0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER_3V3_GPIO_Port, POWER_3V3_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(RST_TINY_GPIO_Port, RST_TINY_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_TINY_GPIO_Port, RST_TINY_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    OV_OR_TINYC_DCMI_Init(0);
    HAL_Delay(1000);
    ST7789_Init();
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 90);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
#else
    HAL_Delay(ms);
#endif
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
#if TINYC_USE_LCD
            tinyc_draw_temp_overlay(max_temp, min_temp, ave_temp);

            tinyc_render_frame_to_lcd();
#endif

            tinyc_frames_in_cycle++;
            if (tinyc_frames_in_cycle >= TINYC_FRAMES_PER_CYCLE)
            {
                tinyc_send_cycle_info(max_temp, min_temp, ave_temp);

#if TINYC_USE_SLEEP
                TINYC_256_DCMI_Stop();
                tinyc_low_power_delay(TINYC_SLEEP_TIME_MS);
                TINYC_256_Start();
#else
                HAL_Delay(TINYC_SLEEP_TIME_MS);
#endif

                tinyc_frames_in_cycle = 0;
            }
        }
    }
    else
    {
        frameRate++;
    }
}
