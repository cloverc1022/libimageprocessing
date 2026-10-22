#include <jni.h>

#include "imageprocessing_native.h"

extern "C" JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_rotation(JNIEnv *env, jobject, jbyteArray data_,
                                                          jint width, jint height, jint degress) {
    jbyte *data = env->GetByteArrayElements(data_, 0);
    ip_rotation(reinterpret_cast<uint8_t *>(data), width, height, degress);
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_scale(JNIEnv *env, jobject, jbyteArray src_,
                                                       jbyteArray dst_, jint srcWidth,
                                                       jint srcHeight, jint dstWidth,
                                                       jint dstHeight) {
    jbyte *srcData = env->GetByteArrayElements(src_, 0);
    jbyte *dstData = env->GetByteArrayElements(dst_, 0);
    ip_scale(reinterpret_cast<const uint8_t *>(srcData), reinterpret_cast<uint8_t *>(dstData),
             srcWidth, srcHeight, dstWidth, dstHeight);
    env->ReleaseByteArrayElements(src_, srcData, JNI_ABORT);
    env->ReleaseByteArrayElements(dst_, dstData, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_initVideoCodec(JNIEnv *env, jobject thiz,
                                                                jint width, jint height, jint fps,
                                                                jint bit_rate) {
    ip_initVideoCodec(width, height, fps, bit_rate);
}

extern "C" JNIEXPORT jint JNICALL
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
        if (data) env->ReleaseByteArrayElements(in_data, data, JNI_ABORT);
        if (out) env->ReleaseByteArrayElements(out_data, out, JNI_ABORT);
        if (len) env->ReleaseIntArrayElements(out_len, len, JNI_ABORT);
        if (type) env->ReleaseIntArrayElements(out_type, type, JNI_ABORT);
        return 0;
    }
    int ret = ip_yuvEncode(reinterpret_cast<uint8_t *>(data), reinterpret_cast<uint8_t *>(out),
                           len, type);
    env->ReleaseByteArrayElements(in_data, data, JNI_ABORT);
    env->ReleaseByteArrayElements(out_data, out, 0);
    env->ReleaseIntArrayElements(out_len, len, 0);
    env->ReleaseIntArrayElements(out_type, type, 0);
    return ret;
}

extern "C" JNIEXPORT void JNICALL
Java_net_ajcloud_imageprocessing_ImageProcessing_releaseVideoCodec(JNIEnv *env, jobject thiz) {
    ip_releaseVideoCodec();
}
