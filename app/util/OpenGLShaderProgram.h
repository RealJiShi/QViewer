#ifndef _OPENGLSHADERPROGRAM_H_
#define _OPENGLSHADERPROGRAM_H_

#include <list>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#ifdef __ANDROID__
#include <GLES3/gl32.h>
#else // edit mode
#include <GL/gl.h>
#endif

namespace util {

class OpenGLShader {
public:
    enum ShaderType {
        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        Compute
    };

    explicit OpenGLShader(OpenGLShader::ShaderType type);
    ~OpenGLShader();
    bool compileSourceCode(const std::string &source);
    bool compileSourceFile(const std::string &filename);
    GLuint shaderID() const { return m_shaderID; }

private:
    void init();
    bool checkCompileErrors() const;

private:
    GLuint m_shaderID;
    ShaderType m_shaderType;
};

typedef std::shared_ptr<OpenGLShader> OpenGLShaderPtr;

class OpenGLShaderProgram {
public:
    explicit OpenGLShaderProgram();
    ~OpenGLShaderProgram();

    bool addShaderFromSourceCode(OpenGLShader::ShaderType type, const std::string &source);
    bool addShaderFromSourceFile(OpenGLShader::ShaderType type, const std::string &filename);

    bool link();
    void bind();
    void release();

    GLuint programID() const { return m_programID; }

    // uniform functions
    void setBoolean(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    bool checkLinkErrors() const;

private:
    GLuint m_programID;
    bool m_isLinked;
    std::list<OpenGLShaderPtr> m_shaders;
};

typedef std::shared_ptr<OpenGLShaderProgram> OpenGLShaderProgramPtr;

} // namespace util

#endif // _OPENGLSHADERPROGRAM_H_
