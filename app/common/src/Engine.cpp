#include "Engine.h"

#include <assert.h>

#ifdef __ANDROID__
#include <jni.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#endif

#include "Renderer.h"
#include "GLContext.h"

namespace common {

Engine::Engine(const std::shared_ptr<Renderer> &renderer) :
    m_renderer(renderer), m_app(nullptr), m_initializedResources(false),
    m_hasFocus(false) {
    // init GL context
    m_GLcontext = GLContext::Get();
}

Engine::~Engine() {

}

void Engine::handleCmd(struct android_app *app, int32_t cmd) {
#ifdef __ANDROID__
    Engine *engine = (Engine *)app->userData;
    switch (cmd) {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        // The window is being shown, get it ready.
        if (app->window != nullptr) {
            engine->onInitDisplay(app);
            engine->m_hasFocus = true;
            engine->draw();
        }
        break;
    case APP_CMD_TERM_WINDOW:
        // The window is being hidden or closed, clean it up.
        engine->terminate();
        engine->m_hasFocus = false;
        break;
    case APP_CMD_STOP:
        break;
    case APP_CMD_GAINED_FOCUS:
        // TODO: sensor
        engine->m_hasFocus = true;
        break;
    case APP_CMD_LOST_FOCUS:
        // TODO: sensor and focus
        engine->m_hasFocus = false;
        engine->draw();
        break;
    case APP_CMD_LOW_MEMORY:
        engine->trimMemory();
        break;
    }
#endif
}

int Engine::onInitDisplay(struct android_app *app) {
#ifdef __ANDROID__
    if (!m_initializedResources) {
        m_GLcontext->init(m_app->window);
        loadResources();
        m_initializedResources = true;
    } else if (app->window != m_GLcontext->getANativeWindow()) {
        assert(m_GLcontext->getANativeWindow());
        unloadResources();
        m_GLcontext->invalidate();
        m_app = app;
        m_GLcontext->init(app->window);
        loadResources();
        m_initializedResources = true;
    } else {
        if (EGL_SUCCESS == m_GLcontext->resume(m_app->window)) {
            unloadResources();
            loadResources();
        } else {
            assert(0);
        }
    }

    // TODO: showUI()

    // initialize GL state
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // set screen
    glViewport(0, 0, m_GLcontext->getScreenWidth(), m_GLcontext->getScreenHeight());

    // TODO: camera
#endif
    return 0;
}

void Engine::setState(struct android_app *state) {
    m_app = state;
    // TODO: set detector..
}

void Engine::loadResources() {
#ifdef __ANDROID__
    m_renderer->init(m_app->activity->assetManager);
    // TODO: bind camera
#endif
}

void Engine::unloadResources() {
    m_renderer->unload();
}

void Engine::draw() {
    // TODO: fps...
    m_renderer->render();

#ifdef __ANDROID__
    // swap
    if (EGL_SUCCESS != m_GLcontext->swap()) {
        unloadResources();
        loadResources();
    }
#endif
}

void Engine::terminate() {
#ifdef __ANDROID__
    m_GLcontext->suspend();
#endif
}

void Engine::trimMemory() {
#ifdef __ANDROID__
    // TODO: log
    m_GLcontext->invalidate();
#endif
}

bool Engine::isReady() const {
    if (m_hasFocus) {
        return true;
    }
    return false;
}

} // namespace common
