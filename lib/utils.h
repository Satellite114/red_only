#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#ifndef UNUSED_ARGS
#define UNUSED_ARGS(args) (void)(args)
#endif
/***********************************{CRC16 START}***************************/
#define CRC16_POLY 0xA001
#define CRC16_START_VALUE 0xffff
void crcInit(void);
uint16_t crc_caculate(uint8_t *buffer, uint16_t size);
/***********************************{CRC16 END}***************************/

uint8_t reverseBits(uint8_t num, uint8_t bitCnt);
void grayImageToRGB(uint16_t *src, uint16_t *dst, int width, int height);
void yuv422_to_rgb565_memcpy(uint8_t *src, uint16_t *dst, int width, int height);
void U32_memcpy_asm(uint32_t *dest, const uint32_t *src, int num);
void U64_memcpy_fast(uint64_t *dest, const uint64_t *src, int num);
void grayImageToRGBl(uint16_t *src, uint16_t *dst, int width, int height);
void grayImageToRGBh(uint16_t *src, uint16_t *dst, int width, int height);
void grayImageLarger(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint16_t out_w, uint16_t out_h);
void grayImagEnlarge(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint8_t n);
void rgb565ImageEnlarge(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height, uint8_t n);
void rgb565ImageEnlargeF(uint16_t *src, uint16_t *dst, uint16_t w_in, uint16_t h_in, uint16_t w_out, uint16_t h_out);
void rgb565Rotate90(uint16_t *src, uint16_t *dst, uint16_t width, uint16_t height);

void color_shift_cooler(void);
void color_shift_warmer(void);
#endif /*_UTILS_H_*/