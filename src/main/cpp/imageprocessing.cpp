#include <jni.h>
#include <string>
#include <libyuv.h>
#include <x264.h>
#include "VideoChannel.h"
#include <pthread.h>

VideoChannel *videoChannel = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_rotation(JNIEnv *env, jobject, jbyteArray data_,
                                                          jint width, jint height, jint degress) {

    jbyte *data = env->GetByteArrayElements(data_, 0);
    uint8_t *src = reinterpret_cast<uint8_t *>(data);
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

    libyuv::I420Rotate(src_y, width, src_u, width
                               >> 1, src_v, width >> 1,
                       dst_y, height, dst_u, height >> 1, dst_v, height >> 1,
                       width, height, static_cast
                               <libyuv::RotationMode>(degress)
    );

    jbyteArray result = env->NewByteArray(size);
    env->
            SetByteArrayRegion(result,
                               0, size, reinterpret_cast
                                       <const jbyte *>(dst)
    );

    env->
            ReleaseByteArrayElements(data_, data,
                                     0);

    env->
            SetByteArrayRegion(data_,
                               0, size, reinterpret_cast
                                       <const jbyte *>(dst)
    );
}

extern "C"
JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_scale(JNIEnv *env, jobject, jbyteArray src_,
                                                       jbyteArray dst_, jint srcWidth,
                                                       jint srcHeight, jint dstWidth,
                                                       jint dstHeight) {
    jbyte *data = env->GetByteArrayElements(src_, 0);
    uint8_t *src = reinterpret_cast<uint8_t *>(data);


    int64_t size = (dstWidth * dstHeight * 3) >> 1;
    uint8_t dst[size];
    uint8_t *src_y;
    uint8_t *src_u;
    uint8_t *src_v;
    int src_stride_y;
    int src_stride_u;
    int src_stride_v;
    uint8_t *dst_y;
    uint8_t *dst_u;
    uint8_t *dst_v;
    int dst_stride_y;
    int dst_stride_u;
    int dst_stride_v;


    src_stride_y = srcWidth;
    src_stride_u = srcWidth >> 1;
    src_stride_v = src_stride_u;

    dst_stride_y = dstWidth;
    dst_stride_u = dstWidth >> 1;
    dst_stride_v = dst_stride_u;

    int src_y_size = srcWidth * srcHeight;
    int src_u_size = src_stride_u * (srcHeight >> 1);
    src_y = src;
    src_u = src + src_y_size;
    src_v = src + src_y_size + src_u_size;

    int dst_y_size = dstWidth * dstHeight;
    int dst_u_size = dst_stride_u * (dstHeight >> 1);
    dst_y = dst;
    dst_u = dst + dst_y_size;
    dst_v = dst + dst_y_size + dst_u_size;

    libyuv::I420Scale(src_y, src_stride_y,
                      src_u, src_stride_u,
                      src_v, src_stride_v,
                      srcWidth, srcHeight,
                      dst_y, dst_stride_y,
                      dst_u, dst_stride_u,
                      dst_v, dst_stride_v,
                      dstWidth, dstHeight,
                      libyuv::FilterMode::kFilterNone
    );
    env->
            ReleaseByteArrayElements(src_, data,
                                     0);

    env->
            SetByteArrayRegion(dst_,
                               0, size, reinterpret_cast
                                       <const jbyte *>(dst)
    );
}
extern "C"
JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_initVideoCodec(JNIEnv *env, jobject thiz,
                                                                jint width, jint height, jint fps,
                                                                jint bit_rate) {
    pthread_mutex_lock(&mutex);
    // 旧实例先释放，避免全局指针被新会话覆盖后旧线程再 delete（STAB-04）
    if (videoChannel) {
        delete videoChannel;
        videoChannel = 0;
    }
    videoChannel = new VideoChannel;
    videoChannel->openCodec(width, height, fps, bit_rate);
    pthread_mutex_unlock(&mutex);
}
extern "C"
JNIEXPORT jint JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_yuvEncode(JNIEnv *env, jobject thiz,
                                                           jbyteArray in_data, jbyteArray out_data,
                                                           jintArray out_len,
                                                           jintArray out_type) {
    if (in_data == NULL || out_data == NULL || out_len == NULL || out_type == NULL) {
        return 0;
    }
    jbyte *data = env->GetByteArrayElements(in_data, 0);
    jbyte *out = env->GetByteArrayElements(out_data, 0);
    jint *len = env->GetIntArrayElements(out_len, 0);
    jint *type = env->GetIntArrayElements(out_type, 0);
    if (data == NULL || out == NULL || len == NULL || type == NULL) {
        // Get 失败也要成对 Release 已拿到的（STAB-06）
        if (data) env->ReleaseByteArrayElements(in_data, data, JNI_ABORT);
        if (out) env->ReleaseByteArrayElements(out_data, out, JNI_ABORT);
        if (len) env->ReleaseIntArrayElements(out_len, len, JNI_ABORT);
        if (type) env->ReleaseIntArrayElements(out_type, type, JNI_ABORT);
        return 0;
    }

    len[0] = 0;
    type[0] = 0;
    int ret = 0;
    pthread_mutex_lock(&mutex);
    if (videoChannel) {
        ret = videoChannel->encode(reinterpret_cast<uint8_t *>(data),
                                   reinterpret_cast<uint8_t *>(out), len, type);
    }
    pthread_mutex_unlock(&mutex);

    // 输入只读；输出与元数据 mode=0 写回，不再额外 Set*Region（STAB-06）
    env->ReleaseByteArrayElements(in_data, data, JNI_ABORT);
    env->ReleaseByteArrayElements(out_data, out, 0);
    env->ReleaseIntArrayElements(out_len, len, 0);
    env->ReleaseIntArrayElements(out_type, type, 0);
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_releaseVideoCodec(JNIEnv *env, jobject thiz) {
    pthread_mutex_lock(&mutex);
    if (videoChannel) {
        delete (videoChannel);
        videoChannel = 0;
    }
    pthread_mutex_unlock(&mutex);
}