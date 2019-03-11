#include "GestureManager.h"

#include <algorithm>
#ifdef __ANDROID__
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#endif

#include <unistd.h>

#include "util.h"
#include "LogUtil.h"

namespace common {

const int32_t DOUBLE_TAP_TIMEOUT = 300 * 1000000;
const int32_t TAP_TIMEOUT = 180 * 1000000;
const int32_t DOUBLE_TAP_SLOP = 100;
const int32_t TOUCH_SLOP = 8;

static int32_t findIndex(const struct AInputEvent *event, int32_t id) {
#ifdef __ANDROID__
    int32_t count = AMotionEvent_getPointerCount(event);
    for (auto i = 0; i < count; ++i) {
        if (id == AMotionEvent_getPointerId(event, i)) {
            return i;
        }
    }
#endif
    return -1;
}

GestureBase::GestureBase() {}

GestureBase::~GestureBase() {}

void GestureBase::setConfiguration(struct AConfiguration *config) {
#ifdef __ANDROID__
    m_dpFactor = 160.0f / AConfiguration_getDensity(config);
#endif
}

class TapGesture : public GestureBase {
public:
    TapGesture();
    ~TapGesture();
    virtual GestureState detect(const struct AInputEvent *event);
    virtual bool getPointer(glm::vec2 &v);
    virtual bool getPointers(glm::vec2 &v1, glm::vec2 &v2);

private:
    int32_t m_downPointerID;
    float m_downX, m_downY;
};

TapGesture::TapGesture() :
    m_downX(0), m_downY(0) {}

TapGesture::~TapGesture() {}

GestureState TapGesture::detect(const struct AInputEvent *event) {
#ifdef __ANDROID__
    if (AMotionEvent_getPointerCount(event) > 1) {
        return GESTURE_STATE_NONE;
    }
    int32_t action = AMotionEvent_getAction(event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
    case AMOTION_EVENT_ACTION_DOWN:
        m_downPointerID = AMotionEvent_getPointerId(event, 0);
        m_downX = AMotionEvent_getX(event, 0);
        m_downY = AMotionEvent_getY(event, 0);
        break;
    case AMOTION_EVENT_ACTION_UP:
        int64_t event_time = AMotionEvent_getEventTime(event);
        int64_t down_time = AMotionEvent_getDownTime(event);
        if (event_time - down_time <= TAP_TIMEOUT) {
            if (m_downPointerID == AMotionEvent_getPointerId(event, 0)) {
                float x = AMotionEvent_getX(event, 0) - m_downX;
                float y = AMotionEvent_getY(event, 0) - m_downY;
                if (x * x + y * y < TOUCH_SLOP * TOUCH_SLOP * m_dpFactor) {
                    ALOGV("Tap Detected");
                    return GESTURE_STATE_ACTION;
                }
            }
        }
        break;
    }
#endif
    return GESTURE_STATE_NONE;
}

bool TapGesture::getPointer(glm::vec2 &v) {
    v.x = m_downX;
    v.y = m_downY;
    return true;
}

bool TapGesture::getPointers(glm::vec2 &v1, glm::vec2 &v2) {
    UNUSED(v1);
    UNUSED(v2);
    return false;
}

class DoubleTapGesture : public GestureBase {
public:
    DoubleTapGesture();
    ~DoubleTapGesture();
    virtual void setConfiguration(struct AConfiguration *config);
    virtual GestureState detect(const struct AInputEvent *event);
    virtual bool getPointer(glm::vec2 &v);
    virtual bool getPointers(glm::vec2 &v1, glm::vec2 &v2);

private:
    TapGesture m_tapGesture;
    int64_t m_lastTapTime;
    float m_lastTapX, m_lastTapY;
};

DoubleTapGesture::DoubleTapGesture() :
    m_lastTapTime(0), m_lastTapX(0), m_lastTapY(0) {
}

DoubleTapGesture::~DoubleTapGesture() {}

void DoubleTapGesture::setConfiguration(struct AConfiguration *config) {
#ifdef __ANDROID__
    m_dpFactor = 160.0f / AConfiguration_getDensity(config);
#endif
    m_tapGesture.setConfiguration(config);
}

GestureState DoubleTapGesture::detect(const struct AInputEvent *event) {
#ifdef __ANDROID__
    if (AMotionEvent_getPointerCount(event) > 1) {
        // only support single double tap
        return GESTURE_STATE_NONE;
    }

    bool tap_detected = m_tapGesture.detect(event);

    int32_t action = AMotionEvent_getAction(event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
    case AMOTION_EVENT_ACTION_DOWN:
    {
        int64_t event_time = AMotionEvent_getEventTime(event);
        if (event_time - m_lastTapTime <= DOUBLE_TAP_TIMEOUT) {
            float x = AMotionEvent_getX(event, 0) - m_lastTapX;
            float y = AMotionEvent_getY(event, 0) - m_lastTapY;
            if (x * x + y * y < DOUBLE_TAP_SLOP * DOUBLE_TAP_SLOP * m_dpFactor) {
                ALOGV("Doubletap Detected");
                return GESTURE_STATE_ACTION;
            }
        }
        break;
    }
    case AMOTION_EVENT_ACTION_UP:
        if (tap_detected) {
            m_lastTapTime = AMotionEvent_getEventTime(event);
            m_lastTapX = AMotionEvent_getX(event, 0);
            m_lastTapY = AMotionEvent_getY(event, 0);
        }
        break;
    }
#endif
    return GESTURE_STATE_NONE;
}

bool DoubleTapGesture::getPointer(glm::vec2 &v) {
    UNUSED(v);
    return false;
}

bool DoubleTapGesture::getPointers(glm::vec2 &v1, glm::vec2 &v2) {
    UNUSED(v1);
    UNUSED(v2);
    return false;
}

class DragGesture : public GestureBase {
public:
    DragGesture();
    ~DragGesture();
    virtual GestureState detect(const struct AInputEvent *event);
    virtual bool getPointer(glm::vec2 &v);
    virtual bool getPointers(glm::vec2 &v1, glm::vec2 &v2);

private:
    const struct AInputEvent *m_event;
    std::vector<int32_t> m_points;
};

DragGesture::DragGesture() :
    m_event(nullptr) {
}

DragGesture::~DragGesture() {}

GestureState DragGesture::detect(const struct AInputEvent *event) {
    GestureState ret = GESTURE_STATE_NONE;
#ifdef __ANDROID__
    int32_t action = AMotionEvent_getAction(event);
    int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
            >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
    m_event = event;

    int32_t count = AMotionEvent_getPointerCount(event);
    switch (flags) {
    case AMOTION_EVENT_ACTION_DOWN:
        m_points.push_back(AMotionEvent_getPointerId(event, 0));
        ret = GESTURE_STATE_START;
        break;
    case AMOTION_EVENT_ACTION_POINTER_DOWN:
        m_points.push_back(AMotionEvent_getPointerId(event, index));
        break;
    case AMOTION_EVENT_ACTION_UP:
        m_points.pop_back();
        ret = GESTURE_STATE_END;
        break;
    case AMOTION_EVENT_ACTION_POINTER_UP:
    {
        int32_t released_pointer_id = AMotionEvent_getPointerId(event, index);
        int32_t i = 0;
        for (auto it = m_points.begin(); it != m_points.end(); ++it, ++i) {
            if (*it == released_pointer_id) {
                m_points.erase(it);
                break;
            }
        }
        if (i <= 1 && count == 2) {
            ret = GESTURE_STATE_START;
        }
    }
        break;
    case AMOTION_EVENT_ACTION_MOVE:
        switch (count) {
        case 1:
            // drag
            ret = GESTURE_STATE_MOVE;
            break;
        default:
            break;
        }

        break;
    case AMOTION_EVENT_ACTION_CANCEL:
        break;
    }
#endif
    return ret;
}

bool DragGesture::getPointer(glm::vec2 &v) {
    if (m_points.empty()) {
        return false;
    }
    int32_t index = findIndex(m_event, m_points[0]);
    if (index == -1) {
        return false;
    }

#ifdef __ANDROID__
    v.x = AMotionEvent_getX(m_event, index);
    v.y = AMotionEvent_getY(m_event, index);
#endif

    return true;
}

bool DragGesture::getPointers(glm::vec2 &v1, glm::vec2 &v2) {
    UNUSED(v1);
    UNUSED(v2);
    return false;
}

class PinchGesture : public GestureBase {
public:
    PinchGesture();
    ~PinchGesture();
    virtual GestureState detect(const struct AInputEvent *event);
    virtual bool getPointer(glm::vec2 &v);
    virtual bool getPointers(glm::vec2 &v1, glm::vec2 &v2);

private:
    const struct AInputEvent *m_event;
    std::vector<int32_t> m_points;
};

PinchGesture::PinchGesture() :
    m_event(nullptr) {
}

PinchGesture::~PinchGesture() {}

GestureState PinchGesture::detect(const struct AInputEvent *event) {
    GestureState ret = GESTURE_STATE_NONE;
#ifdef __ANDROID__
    int32_t action = AMotionEvent_getAction(event);
    uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
    int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                    AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    m_event = event;

    int32_t count = AMotionEvent_getPointerCount(event);
    switch (flags) {
    case AMOTION_EVENT_ACTION_DOWN:
        m_points.push_back(AMotionEvent_getPointerId(event, 0));
        break;
    case AMOTION_EVENT_ACTION_POINTER_DOWN:
        m_points.push_back(AMotionEvent_getPointerId(event, index));
        if (count == 2) {
            ret = GESTURE_STATE_START;
        }
        break;
    case AMOTION_EVENT_ACTION_UP:
        m_points.pop_back();
        break;
    case AMOTION_EVENT_ACTION_POINTER_UP:
    {
        int32_t released_pointer_id = AMotionEvent_getPointerId(event, index);
        int32_t i = 0;
        for (auto it = m_points.begin(); it != m_points.end(); ++it) {
            if (*it == released_pointer_id) {
                m_points.erase(it);
                break;
            }
        }

        if (i <= 1 && count != 2) {
            ret = static_cast<GestureState>(GESTURE_STATE_START | GESTURE_STATE_END);
        }
    }
        break;
    case AMOTION_EVENT_ACTION_MOVE:
        switch (count) {
        case 1:
            break;
        default:
            // multi touch
            ret = GESTURE_STATE_MOVE;
            break;
        }
        break;
    case AMOTION_EVENT_ACTION_CANCEL:
        break;
    }
#endif
    return ret;
}

bool PinchGesture::getPointer(glm::vec2 &v) {
    UNUSED(v);
    return false;
}

bool PinchGesture::getPointers(glm::vec2 &v1, glm::vec2 &v2) {
    if (m_points.size() < 2) {
        return false;
    }

    int32_t index = findIndex(m_event, m_points[0]);
    if (index == -1) {
        return false;
    }

#ifdef __ANDROID__
    float x1 = AMotionEvent_getX(m_event, index);
    float y1 = AMotionEvent_getY(m_event, index);

    index = findIndex(m_event, m_points[1]);
    if (index == -1) {
        return false;
    }

    float x2 = AMotionEvent_getX(m_event, index);
    float y2 = AMotionEvent_getY(m_event, index);

    v1.x = x1;
    v1.y = y1;
    v2.x = x2;
    v2.y = y2;
#endif
    return true;
}

class GestureFactory {
public:
    static GestureBasePtr Create(GestureType type) {
        switch (type) {
        case GESTURE_TAP:
        {
            auto ret = std::make_shared<TapGesture>();
            return ret;
        }
        case GESTURE_DOUBLE_TAP:
        {
            auto ret = std::make_shared<DoubleTapGesture>();
            return ret;
        }
        case GESTURE_DRAG:
        {
            auto ret = std::make_shared<DragGesture>();
            return ret;
        }
        case GESTURE_PINCH:
        {
            auto ret = std::make_shared<PinchGesture>();
            return ret;
        }
        default:
            return nullptr;
        }
    }
};

GestureManager *GestureManager::Get() {
    static GestureManager mgr;
    return &mgr;
}

GestureManager::~GestureManager() {}

GestureType GestureManager::detect(const struct AInputEvent *event) {
#ifdef __ANDROID__
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
        return GESTURE_TYPE_NONE;
    }
#endif

    // reset first
    m_currentGesture = GESTURE_TYPE_NONE;
    m_currentState = GESTURE_STATE_NONE;

    // for-loop the detectors
    for (unsigned int i_type = GESTURE_DOUBLE_TAP; i_type < GESTURE_NUMBERS - 1; ++i_type) {
        GestureType g_type = static_cast<GestureType>(i_type);
        auto iter = m_gestureMap.find(g_type);
        if (iter != m_gestureMap.end()) {
            auto gesture = iter->second;
            if (!gesture) {
                continue;
            }
            GestureState state = gesture->detect(event);
            if (state != GESTURE_STATE_NONE && m_currentGesture != GESTURE_TYPE_NONE) {
                // only record first
                // seems buggy here.., if returns directly, it will lead to crash
                m_currentGesture = g_type;
                m_currentState = state;
            }
        }
    }

    return m_currentGesture;
}

bool GestureManager::getPointer(glm::vec2 &v) {
    if (m_currentGesture == GESTURE_TYPE_NONE || m_currentState == GESTURE_STATE_NONE) {
        return false;
    }
    auto it = m_gestureMap.find(m_currentGesture);
    if (it != m_gestureMap.end()) {
        return it->second->getPointer(v);
    }
    return false;
}

bool GestureManager::getPointers(glm::vec2 &v1, glm::vec2 &v2) {
    if (m_currentGesture == GESTURE_TYPE_NONE || m_currentState == GESTURE_STATE_NONE) {
        return false;
    }
    auto it = m_gestureMap.find(m_currentGesture);
    if (it != m_gestureMap.end()) {
        return it->second->getPointers(v1, v2);
    }
    return false;
}

void GestureManager::setConfiguration(struct AConfiguration *config) {
    for (auto it = m_gestureMap.begin(); it != m_gestureMap.end(); ++it) {
        if (it->second != nullptr) {
            it->second->setConfiguration(config);
        }
    }
}

GestureManager::GestureManager() :
    m_currentGesture(GESTURE_TYPE_NONE), m_currentState(GESTURE_STATE_NONE) {
    // create map
    for (unsigned int i_type = GESTURE_DOUBLE_TAP; i_type < GESTURE_NUMBERS - 1; ++i_type) {
        auto p_gesture = std::make_pair(static_cast<GestureType>(i_type),
                                        GestureFactory::Create(static_cast<GestureType>(i_type)));
        if (p_gesture.second) {
            m_gestureMap.insert(p_gesture);
        }
    }
}

} // namespace common
