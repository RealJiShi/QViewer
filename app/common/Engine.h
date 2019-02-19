#ifndef _COMMON_ENGINE_H_
#define _COMMON_ENGINE_H_

#include <memory>

struct android_app;

namespace common {

class Renderer;
class GLContext;

class Engine {
public:
    explicit Engine(const std::shared_ptr<Renderer> &renderer);
    ~Engine();

    // hanlde functions
    static void handleCmd(struct android_app *app, int32_t cmd);

    // TODO: hanlde input function

    void setState(struct android_app *state);
    int onInitDisplay(struct android_app *app);

    void draw();
    void loadResources();
    void unloadResources();

    void terminate();
    void trimMemory();

    bool isReady() const;

    // TODO: some camera, sensor functions

private:
    // TODO:
    // updateFps
    // showUI
    // TransformPosition

private:
    std::shared_ptr<Renderer> m_renderer;
    GLContext *m_GLcontext;
    struct android_app *m_app;

    // flag
    bool m_initializedResources;
    bool m_hasFocus;

    // TODO: tap, pinch, drag, perf...

    // TODO: camera
};

}

#endif  // _COMMON_ENGINE_H_
