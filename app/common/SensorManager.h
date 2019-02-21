#ifndef _COMMON_SENSORMANAGER_H_
#define _COMMON_SENSORMANAGER_H_

#include <memory>

#ifdef __ANDROID__
#include <jni.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

namespace common {
enum ORIENTATION {
    ORIENTATION_UNKNOWN = -1,
    ORIENTATION_PORTRAIT,
    ORIENTATION_LANDSCAPE,
    ORIENTATION_REVERSE_PORTRIAL,
    ORIENTATION_REVERSE_LANDSCAPE
};

struct AcceleratorState {
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
};

class SensorManager {
public:
    SensorManager();
    ~SensorManager();
    void init(struct android_app *state);
    void suspend();
    void resume();
    ASensorManager* AcquireASensorManagerInstance(struct android_app *app);
    AcceleratorState getState() const { return m_acceleratorState; }
    void processSensors(int32_t id);

private:
    ASensorManager *m_sensorManger;
    ASensorEventQueue *m_sensorEventQueue;
    const ASensor *m_accelerometerSensor;
    AcceleratorState m_acceleratorState;
};

typedef std::shared_ptr<SensorManager> SensorManagerPtr;
} // namespace common
#endif

#endif // _COMMON_SENSORMANAGER_H_
