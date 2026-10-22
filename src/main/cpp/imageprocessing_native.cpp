#include "imageprocessing_native.h"

#include <cstring>
#include <libyuv.h>
#include <pthread.h>

#include "VideoChannel.h"

static VideoChannel *videoChannel = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void ip_rotation(uint8_t *data, int width, int height, int degress) {
    uint8_t *src = data;
    int ySize = width * height;
    int uSize = (width >> 1) * (height >> 1);
    int size = (ySize * 3) >> 1;
    uint8_t dst[size];

    uint8_t *src_y = src;
    uint8_t *src_u = src + ySize;
    uint8_t *src_v = src + ySize + uSize;

    uint8_t *dst_y = dst;
    uint8_t *dst_u = dst + ySize;
    uint8_t *dst_v = dst + ySize + uSize;

    libyuv::I420Rotate(src_y, width, src_u, width >> 1, src_v, width >> 1, dst_y, height,
                       dst_u, height >> 1, dst_v, height >> 1, width, height,
                       static_cast<libyuv::RotationMode>(degress));

    memcpy(data, dst, size);
}

void ip_scale(const uint8_t *src, uint8_t *dst, int srcWidth, int srcHeight, int dstWidth,
              int dstHeight) {
    int64_t size = (dstWidth * dstHeight * 3) >> 1;

    int src_stride_y = srcWidth;
    int src_stride_u = srcWidth >> 1;
    int src_stride_v = src_stride_u;
    int dst_stride_y = dstWidth;
    int dst_stride_u = dstWidth >> 1;
    int dst_stride_v = dst_stride_u;

    int src_y_size = srcWidth * srcHeight;
    int src_u_size = src_stride_u * (srcHeight >> 1);
    const uint8_t *src_y = src;
    const uint8_t *src_u = src + src_y_size;
    const uint8_t *src_v = src + src_y_size + src_u_size;

    int dst_y_size = dstWidth * dstHeight;
    int dst_u_size = dst_stride_u * (dstHeight >> 1);
    uint8_t *dst_y = dst;
    uint8_t *dst_u = dst + dst_y_size;
    uint8_t *dst_v = dst + dst_y_size + dst_u_size;

    libyuv::I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, srcWidth,
                      srcHeight, dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v,
                      dstWidth, dstHeight, libyuv::FilterMode::kFilterNone);
    (void) size;
}

void ip_initVideoCodec(int width, int height, int fps, int bit_rate) {
    pthread_mutex_lock(&mutex);
    if (videoChannel) {
        delete videoChannel;
        videoChannel = 0;
    }
    videoChannel = new VideoChannel;
    videoChannel->openCodec(width, height, fps, bit_rate);
    pthread_mutex_unlock(&mutex);
}

int ip_yuvEncode(const uint8_t *in_data, uint8_t *out_data, int *out_len, int *out_type) {
    if (in_data == NULL || out_data == NULL || out_len == NULL || out_type == NULL) {
        return 0;
    }
    out_len[0] = 0;
    out_type[0] = 0;
    int ret = 0;
    pthread_mutex_lock(&mutex);
    if (videoChannel) {
        ret = videoChannel->encode(const_cast<uint8_t *>(in_data), out_data, out_len, out_type);
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

void ip_releaseVideoCodec() {
    pthread_mutex_lock(&mutex);
    if (videoChannel) {
        delete videoChannel;
        videoChannel = 0;
    }
    pthread_mutex_unlock(&mutex);
}
