#ifndef IMAGEPROCESSING_NATIVE_H
#define IMAGEPROCESSING_NATIVE_H

#include <cstdint>

void ip_rotation(uint8_t *data, int width, int height, int degress);

void ip_scale(const uint8_t *src, uint8_t *dst, int srcWidth, int srcHeight, int dstWidth,
              int dstHeight);

void ip_initVideoCodec(int width, int height, int fps, int bit_rate);

int ip_yuvEncode(const uint8_t *in_data, uint8_t *out_data, int *out_len, int *out_type);

void ip_releaseVideoCodec();

#endif
