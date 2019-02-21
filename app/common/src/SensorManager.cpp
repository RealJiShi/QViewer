#include "SensorManager.h"

#ifdef __ANDROID__
#include <dlfcn.h>
#include <assert.h>
#include "LogUtil.h"

namespace common {

SensorManager::SensorManager() :
    m_sensorManger(nullptr), m_sensorEventQueue(nullptr),
    m_accelerometerSensor(nullptr) {

}

SensorManager::~SensorManager() {

}

void SensorManager::init(struct android_app *state) {
    m_sensorManger = AcquireASensorManagerInstance(state);
    m_accelerometerSensor = ASensorManager_getDefaultSensor(
                m_sensorManger, ASENSOR_TYPE_ACCELEROMETER);
    m_sensorEventQueue = ASensorManager_createEventQueue(
                m_sensorManger, state->looper, LOOPER_ID_USER, nullptr, nullptr);
}

void SensorManager::suspend() {
    if (m_accelerometerSensor) {
        ASensorEventQueue_disableSensor(m_sensorEventQueue, m_accelerometerSensor);
    }
}

void SensorManager::resume() {
    if (m_accelerometerSensor) {
        ASensorEventQueue_enableSensor(m_sensorEventQueue, m_accelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(m_sensorEventQueue, m_accelerometerSensor,
                                       (1000L / 60) * 1000);
    }
}

ASensorManager *SensorManager::AcquireASensorManagerInstance(struct android_app *app) {
    if (!app) {
        return nullptr;
    }

    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)
            dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        JNIEnv* env = nullptr;
        app->activity->vm->AttachCurrentThread(&env, nullptr);

        jclass android_content_Context = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(android_content_Context,
                                                       "getPackageName",
                                                       "()Ljava/lang/String;");
        jstring packageName= (jstring)env->CallObjectMethod(app->activity->clazz,
                                                            midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, 0);
        ASensorManager* mgr = getInstanceForPackageFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();
        if (mgr) {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE)
            dlsym(androidHandle, "ASensorManager_getInstance");
    // by all means at this point, ASensorManager_getInstance should be available
    assert(getInstanceFunc);
    dlclose(androidHandle);

    return getInstanceFunc();
}

void SensorManager::processSensors(int32_t id) {
    if (id == LOOPER_ID_USER) {
        if (m_accelerometerSensor) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(m_sensorEventQueue, &event, 1) > 0) {
                m_acceleratorState.X = event.acceleration.x;
                m_acceleratorState.Y = event.acceleration.y;
                m_acceleratorState.Z = event.acceleration.z;
            }
        }
    }
}

#endif

} // namespace common
