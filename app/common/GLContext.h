#ifndef _COMMON_GLCONTEXT_H_
#define _COMMON_GLCONTEXT_H_

#ifdef __ANDROID__
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

// For local programming, no meaning
#if defined(__WIN32) || defined(__WIN64)
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

namespace common {
class GLContext
{
public:
    static GLContext *Get();

#ifdef __ANDROID__
    bool init(ANativeWindow *window);
    EGLint swap();
    EGLint resume(ANativeWindow *window);
    ANativeWindow *getANativeWindow(void) const { return m_window; }
    EGLDisplay getDisplay() const { return m_display; }
    EGLSurface getSurface() const { return m_surface; }

    bool invalidate();
    void suspend();
    int32_t getScreenWidth() const { return m_screenWidth; }
    int32_t getScreenHeight() const { return m_screenHeight; }
    int32_t getBufferColorSize() const { return m_colorSize; }
    int32_t getBufferDepthSize() const { return m_depthSize; }
    float getGLVersion() const { return m_glVersion; }
    bool checkExtension(const char *extension);

private:
    GLContext(GLContext const&);
    void operator=(GLContext const&);
    GLContext();
    virtual ~GLContext();

    void initGLES();
    void terminate();
    bool initEGLSurface();
    bool initEGLContext();

private:
    // EGL configurations
    ANativeWindow *m_window;
    EGLDisplay m_display;
    EGLSurface m_surface;
    EGLContext m_context;
    EGLConfig m_config;

    // screen parameters
    int32_t m_screenWidth;
    int32_t m_screenHeight;
    int32_t m_colorSize;
    int32_t m_depthSize;

    // flags
    bool m_glesInitialized;
    bool m_eglContexInitialized;
    bool m_contextValid;
    float m_glVersion;
#endif
};
} // namespace common

#endif // _COMMON_GLCONTEXT_H_
