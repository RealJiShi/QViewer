#ifndef _COMMON_RENDERER_H_
#define _COMMON_RENDERER_H_

#ifdef __ANDROID__
#include <jni.h>
#include <GLES3/gl32.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#endif

#include "SensorManager.h"

// For local programming, no meaning
#if defined(__WIN32) || defined(__WIN64)
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

namespace common {

class Renderer {
public:
    virtual ~Renderer() {}
#ifdef __ANDROID__
    virtual void init(AAssetManager *amgr) = 0;
#endif
    virtual GLint getTextureType() = 0;
    virtual void render() = 0;
    virtual void unload() = 0;
    void bindSensor(const SensorManagerPtr &sensorMgr) { m_sensorManager = sensorMgr; }

protected:
    SensorManagerPtr m_sensorManager;
};

} // namespace common

#endif  // _COMMON_RENDERER_H_
