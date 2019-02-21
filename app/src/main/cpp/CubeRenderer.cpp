#include "CubeRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "LogUtil.h"
#include "SensorManager.h"

static glm::vec4 Vertices[] = {
    // front
    { -1.0, -1.0,  1.0, 1.0 },
    {  1.0, -1.0,  1.0, 1.0 },
    {  1.0,  1.0,  1.0, 1.0 },
    { -1.0,  1.0,  1.0, 1.0 },
    // back
    { -1.0, -1.0, -1.0, 1.0 },
    {  1.0, -1.0, -1.0, 1.0 },
    {  1.0,  1.0, -1.0, 1.0 },
    { -1.0,  1.0, -1.0, 1.0 },
};

static GLuint Indices[] = {
    // front
    0, 1, 2,
    2, 3, 0,
    // right
    1, 5, 6,
    6, 2, 1,
    // back
    7, 6, 5,
    5, 4, 7,
    // left
    4, 0, 3,
    3, 7, 4,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // top
    3, 2, 6,
    6, 7, 3,
};

static const char *VERTEX_SHADER =
        "#version 300 es\n"
        "layout(location = 0) in vec4 inPos;\n"
        "out vec4 fColor;\n"
        "uniform mat4 mvp;\n"
        "void main() {\n"
        "    gl_Position = mvp * inPos;\n"
        "}";

static const char *FRAG_SHADER =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 Color;\n"
        "void main() {\n"
        "    Color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

CubeRenderer::CubeRenderer() :
    m_camera(glm::mat4(1.0f)) {
}

CubeRenderer::~CubeRenderer() {

}

void CubeRenderer::render() {
    common::AcceleratorState state = m_sensorManager->getState();
    glClearColor(state.X / 10.0, state.Y / 10.0, state.Z / 10.0, 1.0f);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->bind();
    m_program->setMat4("mvp", m_camera);
    for (auto model : m_models) {
        glBindVertexArray(model->VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
}

GLint CubeRenderer::getTextureType() {
    return 0;
}

void CubeRenderer::unload() {
    for (auto model : m_models) {
        model.reset();
    }
    m_program.reset();
}

void CubeRenderer::setup() {
    // shader
    m_program = std::make_shared<util::OpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(util::OpenGLShader::Vertex, VERTEX_SHADER);
    m_program->addShaderFromSourceCode(util::OpenGLShader::Fragment, FRAG_SHADER);
    m_program->link();

    // model
    auto model = std::make_shared<util::ModelDrawable>();
    m_models.push_back(model);
    // VAO
    glGenVertexArrays(1, &model->VAO);
    glBindVertexArray(model->VAO);
    // VBO
    glGenBuffers(1, &model->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (Vertices), Vertices, GL_STATIC_DRAW);
    // IBO
    glGenBuffers(1, &model->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (Indices), Indices, GL_STATIC_DRAW);
    // attrib
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof (glm::vec4), nullptr);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // simple camera, will replace with tap camera
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glm::mat4 projection = glm::perspective(45.0f, float(1440)/float(2960), 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_camera = projection * view;
}

#ifdef __ANDROID__
void CubeRenderer::init(AAssetManager *amgr) {
    // initialization should be here
    setup();
}
#endif
