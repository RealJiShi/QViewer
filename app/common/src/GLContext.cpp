#include "GLContext.h"
#include <string>
#include "LogUtil.h"

namespace common {

GLContext *GLContext::Get() {
    static GLContext instance;
    return &instance;
}

#ifdef __ANDROID__
bool GLContext::invalidate() {
    terminate();
    m_eglContexInitialized = false;
    return true;
}

void GLContext::suspend() {
    if (m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

bool GLContext::checkExtension(const char *extension) {
    if (extension == nullptr) {
        return false;
    }

    std::string extensions = std::string((char*)glGetString(GL_EXTENSIONS));
    std::string str = std::string(extension);
    str.append(" ");

    size_t pos = 0;
    if (extensions.find(extension, pos) != std::string::npos) {
      return true;
    }

    return false;
}

GLContext::GLContext(const GLContext &) {

}

void GLContext::operator=(const GLContext &) {

}

GLContext::GLContext() :
    m_window(nullptr), m_display(EGL_NO_DISPLAY), m_surface(EGL_NO_SURFACE),
    m_context(EGL_NO_CONTEXT), m_screenWidth(0), m_screenHeight(0), m_glesInitialized(false),
    m_eglContexInitialized(false) {
}

GLContext::~GLContext() {
    terminate();
}

void GLContext::initGLES() {
    if (m_glesInitialized) {
        return;
    }
    const char *versionStr = (const char *)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        m_glVersion = 3.0f;
    }
    m_glesInitialized = true;
}

void GLContext::terminate() {
    if (m_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_context != EGL_NO_CONTEXT) {
            eglDestroyContext(m_display, m_context);
        }

        if (m_surface != EGL_NO_SURFACE) {
            eglDestroySurface(m_display, m_surface);
        }
        eglTerminate(m_display);
    }

    m_display = EGL_NO_DISPLAY;
    m_context = EGL_NO_CONTEXT;
    m_surface = EGL_NO_SURFACE;
    m_window = nullptr;
    m_contextValid = false;
}

bool GLContext::initEGLSurface() {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(m_display, 0, 0);

    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    m_colorSize = 8;
    m_depthSize = 24;

    EGLint num_configs;
    eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);

    if (!num_configs) {
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };
        eglChooseConfig(m_display, attribs, &m_config, 1, &num_configs);
        m_depthSize = 16;
    }

    if (!num_configs) {

        return false;
    }

    m_surface = eglCreateWindowSurface(m_display, m_config, m_window, nullptr);
    eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_screenWidth);
    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_screenHeight);

    return true;
}

bool GLContext::initEGLContext() {
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE
    };
    m_context = eglCreateContext(m_display, m_config, nullptr, context_attribs);

    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
        ALOGE("Unable to retrieve EGL config");
        return false;
    }

    m_contextValid = true;
    return true;
}

bool GLContext::init(ANativeWindow *window) {
    if (m_eglContexInitialized) {
        return true;
    }

    // initialize egl
    m_window = window;
    initEGLSurface();
    initEGLContext();
    initGLES();

    m_eglContexInitialized = true;
    return true;
}

EGLint GLContext::swap() {
    bool success = eglSwapBuffers(m_display, m_surface);
    if (!success) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            initEGLSurface();
            return EGL_SUCCESS;
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost
            m_contextValid = false;
            terminate();
            initEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

EGLint GLContext::resume(ANativeWindow *window) {
    if (!m_eglContexInitialized)  {
        init(window);
        return EGL_SUCCESS;
    }

    int32_t original_width = m_screenWidth;
    int32_t original_height = m_screenHeight;

    // create surface
    m_window = window;
    m_surface = eglCreateWindowSurface(m_display, m_config, m_window, nullptr);
    eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_screenWidth);
    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_screenHeight);

    if (m_screenWidth != original_width || m_screenHeight != original_height) {
        ALOGV("Screen resized");
    }

    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_TRUE)  {
        return EGL_SUCCESS;
    }

    // print err log
    EGLint err = eglGetError();
    ALOGE("Unable to eglMakeCurrent %d", err);

    if (err == EGL_CONTEXT_LOST) {
        ALOGV("Re-creating egl context");
        initEGLContext();
    } else {
        terminate();
        initEGLContext();
        initEGLContext();
    }
    return err;
}
#endif

} // namespace common


