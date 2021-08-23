#ifndef GLFWOPENGLNODE_H
#define GLFWOPENGLNODE_H

#include <GL/glew.h>

#include <QDebug>
#include <QFile>

#include <chrono>
#include <ctime>

class GLFWOpenGLNode
{
public:
    //Defines an OpenGL texture
    typedef struct OpenGLTextureSpecs
    {
        unsigned int width;
        unsigned int height;
        unsigned int channels;

        GLenum target;

        GLint internalFormat;
        GLenum format;

        GLenum dataType;
    }
    OpenGLTextureSpecs;

    //Defines an OpenGL renderer
    typedef struct OpenGLRenderSpecs
    {
        OpenGLTextureSpecs frameType;

        double frameRate;
    }
    OpenGLRenderSpecs;

    GLFWOpenGLNode(OpenGLRenderSpecs specs);

    virtual ~GLFWOpenGLNode();

    virtual GLuint getTextureID() const;

    virtual OpenGLRenderSpecs getSpecs() const;

    //These are the main functions we will use
    virtual void initialize();
    virtual void resize(unsigned int w, unsigned int h);
    virtual void updateSpecs(OpenGLRenderSpecs specs);

    virtual void setFrameRate(float fps);

    virtual void renderFrame();

protected:
    virtual GLuint loadShaders(std::string vertShader, std::string fragShader);

    virtual bool initializeOpenGLFunctions();

    virtual void initializeFBO();
    virtual void initializeShaderProgram();
    virtual void initializeVertexBuffers();
    virtual void initializeUniforms();

    virtual void resizeFBO();

    virtual void updateUniforms();

    virtual void updateStartTime();
    virtual void updateEndTime();

    //Variables
    bool initialized;

    OpenGLRenderSpecs renderSpecs;

    const GLenum GL_outputColorAttachment = GL_COLOR_ATTACHMENT0;

    GLuint programID;

    GLint vertexAttributeLocation;
    GLint texCoordAttributeLocation;

    GLint textureUniformLocation;

    GLuint vaoID;
    GLuint vboID;
    GLuint fboID;

    GLint textureUnit;
    GLuint outputTextureID;

    //Used for timing
    std::chrono::time_point<std::chrono::high_resolution_clock> t_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_end;

    std::chrono::duration<double,std::milli> t_delta;
};

#endif // GLFWOPENGLNODE_H
