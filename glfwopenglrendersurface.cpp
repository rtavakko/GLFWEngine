#include "glfwopenglrendersurface.h"

GLFWOpenGLRenderSurface::GLFWOpenGLRenderSurface(QObject *parent,
                                                 QScreen *outputScreen,
                                                 OpenGLRenderSpecs nNodeSpecs,
                                                 const QSurfaceFormat &surfaceFormat,
                                                 GLFWwindow *sharedContext) :
    GLFWOpenGLSurface(parent,
                      outputScreen,
                      nNodeSpecs,
                      surfaceFormat,
                      sharedContext,
                      true)
{
}
