#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MOC3D"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#endif // __ANDROID__

#endif // LOG_UTIL_H
