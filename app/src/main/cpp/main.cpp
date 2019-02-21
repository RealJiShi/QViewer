#include "Engine.h"
#include "CubeRenderer.h"

#ifdef __ANDROID__
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#endif

class NativeRenderer : public common::Renderer {
public:
    NativeRenderer() {}
    virtual ~NativeRenderer() {}
    virtual void init(AAssetManager *amgr) {}
    virtual void render() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    }
    virtual GLint getTextureType() {
        return 0;
    }
    virtual void unload() {}
};

void android_main(struct android_app* state) {
    // initialize engine
    auto renderer = std::make_shared<CubeRenderer>();
    common::Engine g_engine(renderer);
    g_engine.setState(state);
    // TODO: initialize sensor, handle input

#ifdef __ANDROID__
    state->userData = &g_engine;
    state->onAppCmd = common::Engine::handleCmd;
    state->onInputEvent = common::Engine::handleInput;

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int id;
        int events;
        android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((id = ALooper_pollAll(g_engine.isReady() ? 0 : -1, NULL, &events,
                                     (void**)&source)) >= 0) {
            // Process this event.
            if (source) {
                source->process(state, source);
            }

            // TODO: process sensors
            g_engine.processSensors(id);

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                g_engine.terminate();
                return;
            }
        }

        if (g_engine.isReady()) {
            g_engine.draw();
        }
    }
#endif
}
