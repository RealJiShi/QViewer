#include "OpenGLCommon.h"

#ifdef __ANDROID__
#include <GLES3/gl32.h>
#include "LogUtil.h"

namespace util {
bool OpenGLCommon::CheckGLError(const std::string &func) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        // log
        ALOGE("OpenGL error after %s(): 0x%08x\n", func.c_str(), err);
        return true;
    }
    return false;
}
} // namespace utils
#endif


