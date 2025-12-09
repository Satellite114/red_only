#include "utils.h"
uint32_t min_wh = 16000, max_wh = 20900;
static uint16_t g_min_gray = 18000;
static uint16_t g_max_gray = 22000;
static int8_t g_color_tint = 0; // <0 cooler, >0 warmer

static uint16_t crc16_table[256];
#define CONTRAST_STRENGTH 20.0f
#define MID_POINT 0.5f
// #define ALIGN_32BYTES __attribute__((aligned(32)))
// #define IN_SDRAM      __attribute__((section(".SDRAM")))

// ALIGN_32BYTES static uint16_t buf1[256 * 192] IN_SDRAM; // 中值滤波结果
// ALIGN_32BYTES static uint16_t buf2[256 * 192] IN_SDRAM; // 锐化卷积结果

float g_gamma = 1.0f; // 默认线性，可在主程序中动态调整

static const uint16_t colorLUT[256] = {
    0x000F,
    0x0010,
    0x0011,
    0x0011,
    0x0012,
    0x0012,
    0x0013,
    0x0013,
    0x0014,
    0x0015,
    0x0015,
    0x0016,
    0x0016,
    0x0017,
    0x0017,
    0x0018,
    0x0019,
    0x0019,
    0x001A,
    0x001A,
    0x001B,
    0x001B,
    0x001C,
    0x001D,
    0x001D,
    0x001E,
    0x001E,
    0x001F,
    0x001F,
    0x001F,
    0x001F,
    0x001F,
    0x001F,
    0x003F,
    0x005F,
    0x007F,
    0x009F,
    0x00BF,
    0x00DF,
    0x00FF,
    0x011F,
    0x013F,
    0x015F,
    0x017F,
    0x019F,
    0x01BF,
    0x01DF,
    0x01FF,
    0x021F,
    0x023F,
    0x025F,
    0x027F,
    0x029F,
    0x02BF,
    0x02DF,
    0x02FF,
    0x031F,
    0x033F,
    0x035F,
    0x037F,
    0x039F,
    0x03BF,
    0x03DF,
    0x03FF,
    0x041F,
    0x043F,
    0x045F,
    0x047F,
    0x049F,
    0x04BF,
    0x04DF,
    0x04FF,
    0x051F,
    0x053F,
    0x055F,
    0x057F,
    0x059F,
    0x05BF,
    0x05DF,
    0x05FF,
    0x061F,
    0x063F,
    0x065F,
    0x067F,
    0x069F,
    0x06BF,
    0x06DF,
    0x06FF,
    0x071F,
    0x073E,
    0x075E,
    0x077E,
    0x0F9D,
    0x0FBD,
    0x0FDC,
    0x17FC,
    0x17FC,
    0x1FFB,
    0x1FFB,
    0x1FFA,
    0x27FA,
    0x27FA,
    0x2FF9,
    0x2FF9,
    0x2FF8,
    0x37F8,
    0x37F8,
    0x3FF7,
    0x3FF7,
    0x3FF6,
    0x47F6,
    0x47F6,
    0x4FF5,
    0x4FF5,
    0x4FF4,
    0x57F4,
    0x57F4,
    0x5FF3,
    0x5FF3,
    0x5FF2,
    0x67F2,
    0x67F2,
    0x6FF1,
    0x6FF1,
    0x77F0,
    0x77F0,
    0x77F0,
    0x7FEF,
    0x7FEF,
    0x87EE,
    0x87EE,
    0x87EE,
    0x8FED,
    0x8FED,
    0x97EC,
    0x97EC,
    0x97EB,
    0x9FEB,
    0x9FEB,
    0xA7EA,
    0xA7EA,
    0xA7E9,
    0xAFE9,
    0xAFE9,
    0xB7E8,
    0xB7E8,
    0xB7E7,
    0xBFE7,
    0xBFE7,
    0xC7E6,
    0xC7E6,
    0xC7E5,
    0xCFE5,
    0xCFE5,
    0xD7E4,
    0xD7E4,
    0xD7E3,
    0xDFE3,
    0xDFE3,
    0xE7E2,
    0xE7E2,
    0xE7E1,
    0xEFE1,
    0xEFE1,
    0xF7E0,
    0xF7C0,
    0xF7A0,
    0xFF80,
    0xFF60,
    0xFF40,
    0xFF20,
    0xFF00,
    0xFEE0,
    0xFEC0,
    0xFEA0,
    0xFE80,
    0xFE60,
    0xFE40,
    0xFE40,
    0xFE20,
    0xFE00,
    0xFDE0,
    0xFDC0,
    0xFDA0,
    0xFD80,
    0xFD60,
    0xFD40,
    0xFD20,
    0xFD00,
    0xFCE0,
    0xFCC0,
    0xFCC0,
    0xFCA0,
    0xFC80,
    0xFC60,
    0xFC40,
    0xFC20,
    0xFC00,
    0xFBE0,
    0xFBC0,
    0xFBA0,
    0xFB80,
    0xFB60,
    0xFB40,
    0xFB20,
    0xFB20,
    0xFB00,
    0xFAE0,
    0xFAC0,
    0xFAA0,
    0xFA80,
    0xFA60,
    0xFA40,
    0xFA20,
    0xFA00,
    0xF9E0,
    0xF9C0,
    0xF9A0,
    0xF9A0,
    0xF980,
    0xF960,
    0xF940,
    0xF920,
    0xF900,
    0xF8E0,
    0xF8C0,
    0xF8A0,
    0xF880,
    0xF860,
    0xF040,
    0xF020,
    0xE800,
    0xE800,
    0xE000,
    0xD800,
    0xD800,
    0xD000,
    0xD000,
    0xC800,
    0xC800,
    0xC000,
    0xB800,
    0xB800,
    0xB000,
    0xB000,
    0xA800,
    0xA800,
    0xA000,
    0x9800,
    0x9800,
    0x9000,
    0x9000,
    0x8800,
    0x8800,
    0x8000,
    0x7800,
};

void crcInit(void)
{
    for (uint16_t i = 0; i < 256; i++)
    {
        uint16_t crc = i;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= CRC16_POLY;
            }
            else
            {
                crc >>= 1;
            }
        }
        crc16_table[i] = crc;
    }
}

uint16_t crc_caculate(uint8_t *buffer, uint16_t size)
{
    uint16_t crc = CRC16_START_VALUE;
    for (uint16_t i = 0; i < size; i++)
    {
        crc = (crc >> 8) ^ crc16_table[(crc ^ buffer[i]) & 0xFF];
    }
    return crc;
}

uint8_t reverseBits(uint8_t num, uint8_t bitCnt)
{
    uint8_t result = 0;
    for (uint8_t i = 0; i < bitCnt; ++i)
    {
        uint8_t bit = (num >> i) & 1;

        result |= (bit << (bitCnt - 1 - i));
    }
    return result;
}

// YUV422到RGB565转换函数
uint16_t yuv422_to_rgb565(uint8_t Y, uint8_t U, uint8_t V)
{
    // 将 YCbCr 转换为 RGB
    int32_t C = Y - 16;
    int32_t D = U - 128;
    int32_t E = V - 128;

    uint8_t R = (298 * C + 409 * E + 128) >> 8;
    uint8_t G = (298 * C - 100 * D - 208 * E + 128) >> 8;
    uint8_t B = (298 * C + 516 * D + 128) >> 8;

    // 裁剪 RGB 值到 8 位
    if (R > 255)
        R = 255;
    if (G > 255)
        G = 255;
    if (B > 255)
        B = 255;

    // 将 RGB 转换为 RGB565
    return ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
}

// 函数用于将YUV422数据转换为RGB565并复制到目标缓冲区
void yuv422_to_rgb565_memcpy(uint8_t *src, uint16_t *dst, int width, int height)
{
    for (int i = 0; i < width * height / 2; i++)
    {
        uint8_t Y0 = src[0];
        uint8_t U = src[1] & 0xF0; // 高4位为U
        uint8_t V = src[1] << 4;   // 低4位为V
        uint8_t Y1 = src[2];

        // 转换第一个像素
        dst[0] = yuv422_to_rgb565(Y0, U, V);

        // 转换第二个像素
        U = src[3] & 0xF0; // 高4位为U
        V = src[3] << 4;   // 低4位为V
        dst[1] = yuv422_to_rgb565(Y1, U, V);

        // 调整源指针
        src += 4;
        // 调整目标指针
        dst += 2;
    }
}

inline void U32_memcpy_asm(uint32_t *dest, const uint32_t *src, int num)
{
    asm volatile(
        "1: \n"
        "LDR r2, [%1], #4 \n" // 从 src 加载 32 位数据到寄存器 r2，并自增 src 指针
        "STR r2, [%0], #4 \n" // 将 r2 中的数据存储到 dest，并自增 dest 指针
        "SUBS %2, %2, #1 \n"  // num -= 1，递减计数器
        "BNE 1b \n"           // 如果 num > 0，跳转到标签 1，继续循环
        :
        : "r"(dest), "r"(src), "r"(num) // 输入操作数：dest, src, num
        : "r2", "memory"                // 使用的寄存器 r2 和 memory
    );
}

inline void U64_memcpy_fast(uint64_t *dest, const uint64_t *src, int num)
{
    asm volatile(
        "1: \n"
        "LDR r2, [%1], #4 \n" // 从 src 加载低 32 位到 r2
        "LDR r3, [%1], #4 \n" // 从 src 加载高 32 位到 r3
        "STR r2, [%0], #4 \n" // 将 r2 存储到 dest 的低 32 位
        "STR r3, [%0], #4 \n" // 将 r3 存储到 dest 的高 32 位
        "SUBS %2, %2, #1 \n"  // num -= 1，递减计数器
        "BNE 1b \n"           // 如果 num > 0，跳转到标签 1，继续循环
        :
        : "r"(dest), "r"(src), "r"(num)
        : "r2", "r3", "memory");
}

uint16_t gray_to_rgb565(uint16_t gray, uint16_t min_gray, uint16_t max_gray)
{
    uint8_t r, g, b;

    // 归一化灰度值到 [0, 1]
    float normalized_gray = (float)(gray - min_gray) / (max_gray - min_gray);
    if (normalized_gray < 0.0f)
        normalized_gray = 0.0f;
    else if (normalized_gray > 1.0f)
        normalized_gray = 1.0f;

    // 对比度增强：以 0.5 为中心拉伸
    const float contrast = 1.1f; // >1 提高对比度
    normalized_gray = (normalized_gray - 0.5f) * contrast + 0.5f;
    if (normalized_gray < 0.0f)
        normalized_gray = 0.0f;
    else if (normalized_gray > 1.0f)
        normalized_gray = 1.0f;

    // 基础伪彩：蓝-绿-红-紫色过渡（保持室温偏蓝）
    if (normalized_gray < 0.5f)
    {
        float t = normalized_gray / 0.5f; // 0~1
        r = 0;
        g = (uint8_t)(t * 32.0f);
        b = 31;
    }
    else if (normalized_gray < 0.75f)
    {
        float t = (normalized_gray - 0.5f) / 0.25f; // 0~1
        r = (uint8_t)(t * 31.0f);
        g = (uint8_t)((0.5f + 0.5f * t) * 63.0f);
        b = (uint8_t)((1.0f - t) * 31.0f);
    }
    else
    {
        float t = (normalized_gray - 0.75f) / 0.25f; // 0~1
        r = 31;
        g = (uint8_t)((1.0f - t) * 63.0f);
        b = (uint8_t)(t * 31.0f);
    }

    // 全局色调偏移：g_color_tint <0 偏冷，>0 偏暖，影响整幅图
    if (g_color_tint != 0)
    {
        float alpha = (float)(g_color_tint > 0 ? g_color_tint : -g_color_tint) / 10.0f; // 0~1
        if (alpha > 1.0f)
            alpha = 1.0f;

        if (g_color_tint > 0)
        {
            // 暖色：加强红色，减弱蓝色、部分绿色
            r = (uint8_t)(r + (float)(31 - r) * alpha);
            g = (uint8_t)(g * (1.0f - 0.3f * alpha));
            b = (uint8_t)(b * (1.0f - 0.7f * alpha));
        }
        else
        {
            // 冷色：加强蓝色，减弱红色、部分绿色
            b = (uint8_t)(b + (float)(31 - b) * alpha);
            g = (uint8_t)(g * (1.0f - 0.3f * alpha));
            r = (uint8_t)(r * (1.0f - 0.7f * alpha));
        }

        // 防止超出 5/6 位范围
        if (r > 31)
            r = 31;
        if (g > 63)
            g = 63;
        if (b > 31)
            b = 31;
    }

    return (r << 11) | (g << 5) | b;
}

// 3x3中值滤波（比较网络实现）
#define SWAP(a, b)            \
    {                         \
        if ((a) > (b))        \
        {                     \
            uint16_t t = (a); \
            (a) = (b);        \
            (b) = t;          \
        }                     \
    }
void medianFilter3x3(uint16_t *src, uint16_t *dst, int width, int height)
{
    uint16_t window[9];
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int k = 0;
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    window[k++] = src[(y + dy) * width + (x + dx)];
                }
            }
            // 比较网络排序，window[4]为中值
            SWAP(window[1], window[2]);
            SWAP(window[4], window[5]);
            SWAP(window[7], window[8]);
            SWAP(window[0], window[1]);
            SWAP(window[3], window[4]);
            SWAP(window[6], window[7]);
            SWAP(window[1], window[2]);
            SWAP(window[4], window[5]);
            SWAP(window[7], window[8]);
            SWAP(window[0], window[3]);
            SWAP(window[5], window[8]);
            SWAP(window[4], window[7]);
            SWAP(window[3], window[6]);
            SWAP(window[1], window[4]);
            SWAP(window[2], window[5]);
            SWAP(window[4], window[7]);
            SWAP(window[4], window[2]);
            SWAP(window[6], window[4]);
            SWAP(window[4], window[2]);
            dst[y * width + x] = window[4];
        }
    }
    // 边缘像素直接复制
    for (int x = 0; x < width; x++)
    {
        dst[x] = src[x];
        dst[(height - 1) * width + x] = src[(height - 1) * width + x];
    }
    for (int y = 0; y < height; y++)
    {
        dst[y * width] = src[y * width];
        dst[y * width + width - 1] = src[y * width + width - 1];
    }
}

// void grayImageToRGB(uint16_t *src, uint16_t *dst, int width, int height)
// {
//     static uint16_t smoothBuf[256*192]; // 假设最大分辨率为256x192
//     medianFilter3x3(src, smoothBuf, width, height);

//     uint16_t min = 65535, max = 0, tmp;
//     int size = width * height;
//     for (int i = 0; i < size; i++) {
//         tmp = smoothBuf[i];
//         if (tmp < min) min = tmp;
//         if (tmp > max) max = tmp;
//     }
//     if (max == min) max = min + 1;

//     for (int i = 0; i < size; i++) {
//         double norm = (double)(smoothBuf[i] - min) / (max - min);
//         norm = powf(norm, g_gamma);
//         if (norm < 0.0f) norm = 0.0f;
//         if (norm > 1.0f) norm = 1.0f;
//         uint16_t norm255 = (uint16_t)(norm * 255.0f);
//         dst[i] = colorLUT[norm255];
//     }
// }
// 3x3锐化卷积
void sharpen3x3(uint16_t *src, uint16_t *dst, int width, int height)
{
    // 3x3锐化核
    const static int8_t kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}};
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int32_t sum = 0;
            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    sum += src[(y + ky) * width + (x + kx)] * kernel[ky + 1][kx + 1];
                }
            }
            // 防止溢出
            if (sum < 0)
                sum = 0;
            if (sum > 65535)
                sum = 65535;
            dst[y * width + x] = (uint16_t)sum;
        }
    }
    // 边缘像素直接复制
    for (int x = 0; x < width; x++)
    {
        dst[x] = src[x];
        dst[(height - 1) * width + x] = src[(height - 1) * width + x];
    }
    for (int y = 0; y < height; y++)
    {
        dst[y * width] = src[y * width];
        dst[y * width + width - 1] = src[y * width + width - 1];
    }
}

void grayImageToRGB(uint16_t *src, uint16_t *dst, int width, int height)
{
    static uint16_t buf1[256 * 192]; // 中值滤波结果

    medianFilter3x3(src, buf1, width, height);
    sharpen3x3(buf1, src, width, height);
    uint16_t *buf2 = src;

    medianFilter3x3(src, buf1, width, height);
    // 降噪
    sharpen3x3(buf1, buf2, width, height);
    // 锐化

    uint16_t local_min = g_min_gray;
    uint16_t local_max = g_max_gray;
    if (local_max <= local_min)
    {
        local_max = local_min + 1;
    }

    int size = width * height;
    for (int i = 0; i < size; i++)
    {
        dst[i] = gray_to_rgb565(buf2[i], local_min, local_max);
    }
}

void color_shift_cooler(void)
{
    // 按键使整幅图偏冷（整体偏蓝）
    if (g_color_tint > -10)
    {
        g_color_tint--;
    }
}

void color_shift_warmer(void)
{
    // 按键使整幅图偏暖（整体偏红）
    if (g_color_tint < 10)
    {
        g_color_tint++;
    }
}

void grayImageLarger(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint16_t out_w, uint16_t out_h)
{
    // 计算缩放比例
    float scale_x = (float)(width - 1) / (out_w - 1);
    float scale_y = (float)(height - 1) / (out_h - 1);

    for (uint16_t y = 0; y < out_h; y++)
    {
        for (uint16_t x = 0; x < out_w; x++)
        {
            // 计算对应的输入图像的坐标
            float src_x = x * scale_x;
            float src_y = y * scale_y;

            // 获取整数部分和小数部分
            uint16_t x0 = (uint16_t)src_x;
            uint16_t y0 = (uint16_t)src_y;
            uint16_t x1 = (x0 + 1 < width) ? x0 + 1 : x0;
            uint16_t y1 = (y0 + 1 < height) ? y0 + 1 : y0;

            float x_diff = src_x - x0;
            float y_diff = src_y - y0;

            // 取四个相邻像素的灰度值
            uint16_t gray00 = src[y0 * width + x0];
            uint16_t gray01 = src[y0 * width + x1];
            uint16_t gray10 = src[y1 * width + x0];
            uint16_t gray11 = src[y1 * width + x1];

            // 双线性插值公式
            float gray = (1 - x_diff) * (1 - y_diff) * gray00 +
                         x_diff * (1 - y_diff) * gray01 +
                         (1 - x_diff) * y_diff * gray10 +
                         x_diff * y_diff * gray11;

            // 将插值结果放入目标图像
            dst[y * out_w + x] = (uint16_t)gray;
        }
    }
}

void grayImagEnlarge(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint8_t n)
{
    uint16_t out_w = width * n;
    uint16_t out_h = height * n;

    for (uint16_t y = 0; y < out_h; y++)
    {
        for (uint16_t x = 0; x < out_w; x++)
        {
            uint16_t src_x = x / n;
            uint16_t src_y = y / n;
            dst[y * out_w + x] = src[src_y * width + src_x];
        }
    }
}

void rgb565ImageEnlarge(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint8_t n)
{
    // 计算输出图像的宽高
    uint16_t out_w = width * n;
    uint16_t out_h = height * n;

    for (uint16_t y = 0; y < out_h; y++)
    {
        for (uint16_t x = 0; x < out_w; x++)
        {
            // 对应输入图像的坐标（最近邻插值）
            uint16_t src_x = x / n;
            uint16_t src_y = y / n;

            // 获取输入图像的 RGB565 值并赋给输出图像
            dst[y * out_w + x] = src[src_y * width + src_x];
        }
    }
}

// 提取RGB565格式的颜色通道
#define GET_R(c) (((c) >> 11) & 0x1F)
#define GET_G(c) (((c) >> 5) & 0x3F)
#define GET_B(c) ((c) & 0x1F)

// 合成RGB565格式的颜色
#define RGB565(r, g, b) (((r) << 11) | ((g) << 5) | (b))

void rgb565ImageEnlargeF(uint16_t *src, uint16_t *dst, uint16_t w_in, uint16_t h_in, uint16_t w_out, uint16_t h_out)
{
    for (uint16_t y = 0; y < h_out; y++)
    {
        for (uint16_t x = 0; x < w_out; x++)
        {
            // 计算在原图上的对应位置（浮点数）
            float src_x = ((float)x / w_out) * (w_in - 1);
            float src_y = ((float)y / h_out) * (h_in - 1);

            // 计算左上角像素的坐标
            uint16_t x0 = (uint16_t)src_x;
            uint16_t y0 = (uint16_t)src_y;
            uint16_t x1 = (x0 + 1 < w_in) ? (x0 + 1) : x0;
            uint16_t y1 = (y0 + 1 < h_in) ? (y0 + 1) : y0;

            // 获取四个相邻像素的颜色值
            uint16_t c00 = src[y0 * w_in + x0];
            uint16_t c01 = src[y0 * w_in + x1];
            uint16_t c10 = src[y1 * w_in + x0];
            uint16_t c11 = src[y1 * w_in + x1];

            // 计算权重
            float dx = src_x - x0;
            float dy = src_y - y0;

            // 分别计算R、G、B分量
            uint8_t r = (1 - dx) * (1 - dy) * GET_R(c00) + dx * (1 - dy) * GET_R(c01) + (1 - dx) * dy * GET_R(c10) + dx * dy * GET_R(c11);
            uint8_t g = (1 - dx) * (1 - dy) * GET_G(c00) + dx * (1 - dy) * GET_G(c01) + (1 - dx) * dy * GET_G(c10) + dx * dy * GET_G(c11);
            uint8_t b = (1 - dx) * (1 - dy) * GET_B(c00) + dx * (1 - dy) * GET_B(c01) + (1 - dx) * dy * GET_B(c10) + dx * dy * GET_B(c11);

            // 组合RGB565格式
            dst[y * w_out + x] = RGB565(r, g, b);
        }
    }
}
 
void rgb565Rotate90(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height)
{
    uint32_t index = 0;
    for (uint16_t y = 0; y < height; y++)
    {
        for (uint16_t x = 0; x < width; x++)
        {
            dst[x * height + (height - y - 1)] = src[y * width + x];
        }
    }
}