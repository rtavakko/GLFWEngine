#include "glfwopenglrenderwindow.h"

GLFWOpenGLRenderWindow::GLFWOpenGLRenderWindow(QObject *parent,
                                               QScreen *outputScreen,
                                               GLFWOpenGLNode::OpenGLRenderSpecs nNodeSpecs,
                                               const QSurfaceFormat &surfaceFormat,
                                               GLFWwindow *sharedContext) :
    GLFWOpenGLSurface(parent,
                      outputScreen,
                      nNodeSpecs,
                      surfaceFormat,
                      sharedContext,
                      false)
{
}
