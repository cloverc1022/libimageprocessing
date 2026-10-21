//
// Created by Administrator on 2019/12/29.
//

#include <cstdint>
#include <cstring>
#include <alloca.h>
#include "VideoChannel.h"
#include <android/log.h>

VideoChannel::VideoChannel() {

}

VideoChannel::~VideoChannel() {
    if (codec) {
        x264_encoder_close(codec);
        codec = 0;
    }
}

void Log_i(const char *log, ...) {
    va_list arg;
    va_start(arg, log);
    __android_log_vprint(ANDROID_LOG_INFO, "ImageProcessing", log, arg);
    va_end(arg);
}

void x264_log_default2(void *, int i_level, const char *psz, va_list list) {
    __android_log_vprint(ANDROID_LOG_ERROR, "X264", psz, list);
}

void VideoChannel::openCodec(int width, int height, int fps, int bitrate) {
    // 编码器参数
    x264_param_t param;

    // ultrafast: 编码速度与质量的控制 ,使用最快的模式编码
    // zerolatency: 无延迟编码 ， 实时通信方面
    x264_param_default_preset(&param, "ultrafast", "zerolatency");
    // main base_line high
    //base_line 3.2 编码规格 无B帧(数据量最小，但是解码速度最慢)
    param.i_level_idc = 32;
    //输入数据格式
    param.i_csp = X264_CSP_I420;
    param.i_width = width;
    param.i_height = height;
    //无b帧
    param.i_bframe = 0;
    //参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
    param.rc.i_rc_method = X264_RC_ABR;
    //码率(比特率,单位Kbps)
    param.rc.i_bitrate = bitrate / 1000;
    //瞬时最大码率
    param.rc.i_vbv_max_bitrate = bitrate / 1000 * 1.2;

    //帧率
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    param.pf_log = x264_log_default2;
    //帧距离(关键帧)  2s一个关键帧
    param.i_keyint_max = fps * 2;
    // 是否复制sps和pps放在每个关键帧的前面 该参数设置是让每个关键帧(I帧)都附带sps/pps。
    param.b_repeat_headers = 1;
    //不使用并行编码。zerolatency场景下设置param.rc.i_lookahead=0;
    // 那么编码器来一帧编码一帧，无并行、无延时
    param.i_threads = 1;
    param.rc.i_lookahead = 0;
    x264_param_apply_profile(&param, "baseline");

    codec = x264_encoder_open(&param);
    ySize = width * height;
    uSize = (width >> 1) * (height >> 1);
    this->width = width;
    this->height = height;
}

int VideoChannel::encode(uint8_t *data, uint8_t *out_data, int *size, int *type) {
    if (codec == NULL || data == NULL || out_data == NULL || size == NULL || type == NULL) {
        return 0;
    }

    // 非所有权模式：不 x264_picture_alloc，避免每帧泄漏内部图像缓冲（STAB-05）
    x264_picture_t pic_in;
    x264_picture_init(&pic_in);
    pic_in.img.i_csp = X264_CSP_I420;
    pic_in.img.i_plane = 3;
    pic_in.img.plane[0] = data;
    pic_in.img.plane[1] = data + ySize;
    pic_in.img.plane[2] = data + ySize + uSize;
    pic_in.img.i_stride[0] = width;
    pic_in.img.i_stride[1] = width >> 1;
    pic_in.img.i_stride[2] = width >> 1;
    //todo 编码的i_pts，每次需要增长
    pic_in.i_pts = i_pts++;

    x264_picture_t pic_out;
    x264_nal_t *pp_nal;
    int pi_nal;
    int s = 0;
    //pi_nal: 输出了多少nal
    int ret = x264_encoder_encode(codec, &pp_nal, &pi_nal, &pic_in, &pic_out);
    if (ret <= 0) {
        return ret;
    }

    for (int i = 0; i < pi_nal; i++) {
        //数据
        uint8_t *p_payload = pp_nal[i].p_payload;
        //数据长度
        int i_payload = pp_nal[i].i_payload;
        memcpy(out_data, p_payload, i_payload);
        out_data += i_payload;
        s += i_payload;
    }
    size[0] = s;
    type[0] = pic_out.i_type;
    return ret;
}