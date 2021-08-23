#include "display.h"

Display::Display(QObject *parent,
                 QScreen *outputScreen,
                 OpenGLRenderSpecs nNodeSpecs,
                 const QSurfaceFormat &surfaceFormat,
                 GLFWwindow *sharedContext) :
    GLFWOpenGLRenderWindow(parent,outputScreen,
                           nNodeSpecs,
                           surfaceFormat,
                           sharedContext),
    inputTextureID(0)
{
}

void Display::renderFrame()
{
    updateStartTime();
    if(!makeContextCurrent())
        return;

    initialize();

    //Render to default FBO
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glBindVertexArray(vaoID);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(programID);

    //Update shader uniform values
    updateUniforms();

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, inputTextureID);

    glViewport(0,0,renderSpecs.frameType.width,renderSpecs.frameType.height);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(0);

    glBindVertexArray(0);

    swapSurfaceBuffers();
    doneContextCurrent();

    updateEndTime();
}

void Display::setFrame(GLuint texID, unsigned int width, unsigned int height)
{
    inputTextureID = texID;
    renderFrame();
}
