#ifndef GLFWOPENGLRENDERSURFACE_H
#define GLFWOPENGLRENDERSURFACE_H

#include <glfwopenglsurface.h>

class GLFWOpenGLRenderSurface : public GLFWOpenGLSurface
{
public:
    GLFWOpenGLRenderSurface(QObject* parent,
                            QScreen* outputScreen,
                            OpenGLRenderSpecs nNodeSpecs,
                            const QSurfaceFormat& surfaceFormat,
                            GLFWwindow* sharedContext);
};

#endif // GLFWOPENGLRENDERSURFACE_H
