#include "OpenGLShaderProgram.h"

#if defined(__WIN32) || defined(__WIN64)
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "log_util.h"

#define UNIFORM_LOCATION \
    glGetUniformLocation(m_programID, name.c_str())

namespace util {

OpenGLShader::OpenGLShader(OpenGLShader::ShaderType type) :
    m_shaderID(0), m_shaderType(type)
{
    init();
}

OpenGLShader::~OpenGLShader()
{
    glDeleteShader(m_shaderID);
}

bool OpenGLShader::compileSourceCode(const std::string &source) {
    if (source.empty()) {
#ifdef __ANDROID__
        ALOGE("Empty shader source!\n");
#endif
        return false;
    }
    const char *c_source = source.c_str();
    glShaderSource(m_shaderID, 1, &c_source, nullptr);
    glCompileShader(m_shaderID);
    return checkCompileErrors();
}

bool OpenGLShader::compileSourceFile(const std::string &filename) {
    std::ifstream _file;
    _file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        _file.open(filename);
        std::stringstream _stream;
        _stream << _file.rdbuf();
        _file.close();
        const std::string &source = _stream.str();
        return compileSourceCode(source);
    } catch (std::ifstream::failure e) {
#ifdef __ANDROID__
        ALOGE("Cannot open %s()\n", filename.c_str());
#endif
    }
    return false;
}

void OpenGLShader::init() {
    GLenum shaderType = GL_NONE;
    switch (m_shaderType) {
    case Vertex:
        shaderType = GL_VERTEX_SHADER;
        break;
    case Fragment:
        shaderType = GL_FRAGMENT_SHADER;
        break;
    case Geometry:
        shaderType = GL_GEOMETRY_SHADER;
        break;
    case TessellationControl:
        shaderType = GL_TESS_CONTROL_SHADER;
        break;
    case TessellationEvaluation:
        shaderType = GL_TESS_EVALUATION_SHADER;
        break;
    case Compute:
        shaderType = GL_COMPUTE_SHADER;
        break;
    }
    m_shaderID = glCreateShader(shaderType);
}

bool OpenGLShader::checkCompileErrors() const {
    GLint success = 0;
    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint infoLogLen = 0;
        glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar *info = new GLchar[infoLogLen];
            glGetShaderInfoLog(m_shaderID, infoLogLen, nullptr, info);
#ifdef __ANDROID__
            ALOGE("Could not compile shader:\n%s\n", info);
#endif
            delete  [] info;
        }
    }
    return success;
}

OpenGLShaderProgram::OpenGLShaderProgram() :
    m_isLinked(false) {
    m_programID = glCreateProgram();
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    m_shaders.clear();
    glDeleteProgram(m_programID);
}

bool OpenGLShaderProgram::addShaderFromSourceCode(OpenGLShader::ShaderType type, const std::string &source)
{
    OpenGLShaderPtr shader = std::make_shared<OpenGLShader>(type);
    if (shader->compileSourceCode(source)) {
        m_shaders.push_back(shader);
        return true;
    }
    return false;
}

bool OpenGLShaderProgram::addShaderFromSourceFile(OpenGLShader::ShaderType type, const std::string &filename)
{
    OpenGLShaderPtr shader = std::make_shared<OpenGLShader>(type);

    if (shader->compileSourceFile(filename)) {
        m_shaders.push_back(shader);
        return true;
    }
    return false;
}

bool OpenGLShaderProgram::link()
{
    if (m_isLinked) {
        return true;
    }
    for (auto shader : m_shaders) {
        glAttachShader(m_programID, shader->shaderID());
    }
    glLinkProgram(m_programID);
    if (!checkLinkErrors()) {
        m_isLinked = true;
        return true;
    }
    return false;
}

void OpenGLShaderProgram::bind()
{
    glUseProgram(m_programID);
}

void OpenGLShaderProgram::release()
{
    glUseProgram(0);
}

void OpenGLShaderProgram::setBoolean(const std::string &name, bool value) const
{
    glUniform1i(UNIFORM_LOCATION, value);
}

void OpenGLShaderProgram::setInt(const std::string &name, int value) const
{
    glUniform1i(UNIFORM_LOCATION, value);
}

void OpenGLShaderProgram::setFloat(const std::string &name, float value) const
{
    glUniform1f(UNIFORM_LOCATION, value);
}

void OpenGLShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

void OpenGLShaderProgram::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(UNIFORM_LOCATION, x, y);
}

void OpenGLShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

void OpenGLShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(UNIFORM_LOCATION, x, y, z);
}

void OpenGLShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

void OpenGLShaderProgram::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(UNIFORM_LOCATION, x, y, z, w);
}

void OpenGLShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(mat));
}

void OpenGLShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(mat));
}

void OpenGLShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(mat));
}

bool OpenGLShaderProgram::checkLinkErrors() const
{
    GLint success = GL_FALSE;
    glGetShaderiv(m_programID, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        GLint infoLogLen = 0;
        glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar *info = new GLchar[infoLogLen];
            glGetProgramInfoLog(m_programID, infoLogLen, nullptr, info);
#ifdef __ANDROID__
            ALOGE("Could not link shader:\n%s\n", info);
#endif
            delete [] info;
        }
    }
    return !success;
}

} // namespace util
