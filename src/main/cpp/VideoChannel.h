
#ifndef WANSVIEWCLOUD_VIDEOCHANNEL_H
#define WANSVIEWCLOUD_VIDEOCHANNEL_H

#include <x264.h>
#include "libyuv/include/libyuv/basic_types.h"

/**
 *  封装了x264的编码等操作
 */
class VideoChannel {
public:
    VideoChannel();

    ~VideoChannel();

    int width;
    int height;

    int encode(uint8_t *data, uint8_t *out_data, int *size, int *type);

public:
    void openCodec(int width, int height, int fps, int bitrate);

private:
    x264_t *codec = 0;
    int ySize;
    int uSize;
    int64_t i_pts = 0;
};


#endif //PUSHER_VIDEOCHANNEL_H
