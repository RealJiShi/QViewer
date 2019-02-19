#ifndef _OPENGLCOMMON_H_
#define _OPENGLCOMMON_H_

#include <string>

namespace util {
class OpenGLCommon {
    static bool CheckGLError(const std::string &func);
};
} // namespace utils

#endif // _OPENGLCOMMON_H_
