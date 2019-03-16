#ifndef CUBERENDERER_H
#define CUBERENDERER_H

#include "Renderer.h"

#include <vector>
#include <glm/glm.hpp>

#include "ModelDrawable.h"
#include "OpenGLShaderProgram.h"

// A test class to render cube
class CubeRenderer : public common::Renderer
{
public:
    CubeRenderer();
    virtual ~CubeRenderer();
#ifdef __ANDROID__
    virtual void init();
#endif
    virtual void render();
    virtual GLint getTextureType();
    virtual void unload();

private:
    void setup();

private:
    // simple camera (glm)
    glm::mat4 m_camera;

    // models
    std::vector<util::ModelDrawablePtr> m_models;

    // program
    util::OpenGLShaderProgramPtr m_program;
};

#endif // CUBERENDERER_H
