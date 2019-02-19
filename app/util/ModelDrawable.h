#ifndef MODELDRAWABLE_H
#define MODELDRAWABLE_H

#include <memory>
#include <glm/glm.hpp>

#if defined(__WIN32) || defined(__WIN64)
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

namespace util {
class ModelDrawable {
public:
    enum OccludingAttrib {
        OCCLUDER,
        OCCLUDEE
    };


    explicit ModelDrawable() {}
    ~ModelDrawable() {
        // release everything
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &bbVAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
        glDeleteBuffers(1, &bbVBO);
        glDeleteBuffers(1, &bbIBO);
        glDeleteQueries(1, &Query);
    }

    GLuint VAO = 0, VBO = 0, IBO = 0;
    GLuint bbVAO = 0, bbVBO = 0, bbIBO = 0;
    float BBGeo[24];
    int BBindices[32];
    GLuint Query = 0;
    GLuint TriCount = 0;
    glm::vec3 Center = glm::vec3(0.0f);
    bool isCulled = true;
    OccludingAttrib occludingAttrib = OCCLUDEE;
};

typedef std::shared_ptr<ModelDrawable> ModelDrawablePtr;
}

#endif // MODELDRAWABLE_H
