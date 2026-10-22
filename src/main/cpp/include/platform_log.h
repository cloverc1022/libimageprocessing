#ifndef IMAGEPROCESSING_PLATFORM_LOG_H
#define IMAGEPROCESSING_PLATFORM_LOG_H

#include <cstdio>
#include <cstdarg>

#if defined(BUILD_FOR_OHOS)
#include "ohos_log.h"

inline void platform_log_info(const char *fmt, va_list arg) {
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, arg);
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, LOG_TAG, "%{public}s", buf);
}

inline void platform_log_error(const char *fmt, va_list arg) {
    char buf[2048];
    vsnprintf(buf, sizeof(buf), fmt, arg);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOG_TAG, "%{public}s", buf);
}
#else
#include <android/log.h>

inline void platform_log_info(const char *fmt, va_list arg) {
    __android_log_vprint(ANDROID_LOG_INFO, "ImageProcessing", fmt, arg);
}

inline void platform_log_error(const char *fmt, va_list arg) {
    __android_log_vprint(ANDROID_LOG_ERROR, "X264", fmt, arg);
}
#endif

#endif
