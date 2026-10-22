#include "napi/native_api.h"

#include <cstdint>

#include "imageprocessing_native.h"

static bool getInt32(napi_env env, napi_value value, int32_t *out) {
    return napi_get_value_int32(env, value, out) == napi_ok;
}

static bool getArrayBuffer(napi_env env, napi_value value, void **data, size_t *length) {
    bool isArrayBuffer = false;
    if (napi_is_arraybuffer(env, value, &isArrayBuffer) == napi_ok && isArrayBuffer) {
        return napi_get_arraybuffer_info(env, value, data, length) == napi_ok;
    }
    napi_typedarray_type type;
    napi_value arrayBuffer;
    size_t byteOffset = 0;
    if (napi_get_typedarray_info(env, value, &type, length, data, &arrayBuffer, &byteOffset) !=
        napi_ok) {
        return false;
    }
    return type == napi_uint8_array || type == napi_int8_array;
}

static bool getInt32Array(napi_env env, napi_value value, int32_t **data, size_t *length) {
    napi_typedarray_type type;
    napi_value arrayBuffer;
    size_t byteOffset = 0;
    if (napi_get_typedarray_info(env, value, &type, length, (void **) data, &arrayBuffer,
                                 &byteOffset) != napi_ok) {
        return false;
    }
    return type == napi_int32_array;
}

static napi_value rotation(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    void *data = nullptr;
    size_t len = 0;
    int32_t width = 0, height = 0, degress = 0;
    if (!getArrayBuffer(env, args[0], &data, &len) || !getInt32(env, args[1], &width) ||
        !getInt32(env, args[2], &height) || !getInt32(env, args[3], &degress)) {
        return nullptr;
    }
    ip_rotation(reinterpret_cast<uint8_t *>(data), width, height, degress);
    return nullptr;
}

static napi_value scale(napi_env env, napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    void *src = nullptr;
    void *dst = nullptr;
    size_t srcLen = 0, dstLen = 0;
    int32_t srcWidth = 0, srcHeight = 0, dstWidth = 0, dstHeight = 0;
    if (!getArrayBuffer(env, args[0], &src, &srcLen) || !getArrayBuffer(env, args[1], &dst, &dstLen) ||
        !getInt32(env, args[2], &srcWidth) || !getInt32(env, args[3], &srcHeight) ||
        !getInt32(env, args[4], &dstWidth) || !getInt32(env, args[5], &dstHeight)) {
        return nullptr;
    }
    ip_scale(reinterpret_cast<const uint8_t *>(src), reinterpret_cast<uint8_t *>(dst), srcWidth,
             srcHeight, dstWidth, dstHeight);
    return nullptr;
}

static napi_value initVideoCodec(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int32_t width = 0, height = 0, fps = 0, bit_rate = 0;
    if (!getInt32(env, args[0], &width) || !getInt32(env, args[1], &height) ||
        !getInt32(env, args[2], &fps) || !getInt32(env, args[3], &bit_rate)) {
        return nullptr;
    }
    ip_initVideoCodec(width, height, fps, bit_rate);
    return nullptr;
}

static napi_value yuvEncode(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    void *inData = nullptr;
    void *outData = nullptr;
    size_t inLen = 0, outLen = 0;
    int32_t *lenArr = nullptr;
    int32_t *typeArr = nullptr;
    size_t lenCount = 0, typeCount = 0;
    if (!getArrayBuffer(env, args[0], &inData, &inLen) ||
        !getArrayBuffer(env, args[1], &outData, &outLen) ||
        !getInt32Array(env, args[2], &lenArr, &lenCount) ||
        !getInt32Array(env, args[3], &typeArr, &typeCount) || lenCount < 1 || typeCount < 1) {
        return nullptr;
    }
    int ret = ip_yuvEncode(reinterpret_cast<const uint8_t *>(inData),
                           reinterpret_cast<uint8_t *>(outData), lenArr, typeArr);
    napi_value result;
    napi_create_int32(env, ret, &result);
    return result;
}

static napi_value releaseVideoCodec(napi_env env, napi_callback_info info) {
    ip_releaseVideoCodec();
    return nullptr;
}

static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
            {"rotation", nullptr, rotation, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"scale", nullptr, scale, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"initVideoCodec", nullptr, initVideoCodec, nullptr, nullptr, nullptr, napi_default,
             nullptr},
            {"yuvEncode", nullptr, yuvEncode, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"releaseVideoCodec", nullptr, releaseVideoCodec, nullptr, nullptr, nullptr,
             napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_module imageprocessingModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "imageprocessing",
        .nm_priv = ((void *) 0),
        .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterImageProcessingModule(void) {
    napi_module_register(&imageprocessingModule);
}
