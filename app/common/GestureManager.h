#ifndef _COMMON_GESTUREMANAGER_H_
#define _COMMON_GESTUREMANAGER_H_

#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct AConfiguration;
struct AInputEvent;

namespace common {

enum GestureState {
    GESTURE_STATE_NONE = 0,
    GESTURE_STATE_START = 1,
    GESTURE_STATE_MOVE = 2,
    GESTURE_STATE_END = 4,
    GESTURE_STATE_ACTION = (GESTURE_STATE_START | GESTURE_STATE_END)
};

// sort by priority in descending order
enum GestureType {
    GESTURE_TYPE_NONE = 0,
    GESTURE_DOUBLE_TAP,
    GESTURE_DRAG,
    GESTURE_PINCH,
    GESTURE_TAP,
    GESTURE_NUMBERS
};

class GestureBase {
public:
    GestureBase();
    virtual ~GestureBase();
    virtual void setConfiguration(struct AConfiguration *config);
    virtual GestureState detect(const struct AInputEvent *event) = 0;
    virtual bool getPointer(glm::vec2 &v) = 0;
    virtual bool getPointers(glm::vec2 &v1, glm::vec2 &v2) = 0;

protected:
    float m_dpFactor;
};

typedef std::shared_ptr<GestureBase> GestureBasePtr;

class GestureManager {
public:
    static GestureManager *Get();
    ~GestureManager();
    GestureType detect(const AInputEvent *event);
    GestureState getState() const { return m_currentState; }
    bool getPointer(glm::vec2 &v);
    bool getPointers(glm::vec2 &v1, glm::vec2 &v2);
    void setConfiguration(AConfiguration *config);

private:
    // only one instance
    GestureManager();

    // record the current gesture type
    GestureType m_currentGesture;

    // record the current gesture state
    GestureState m_currentState;

    // record all the gesture detectors
    std::map<GestureType, GestureBasePtr> m_gestureMap;
};

}

#endif  // _COMMON_GESTUREMANAGER_H_
