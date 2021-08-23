#ifndef GLFWOPENGLRENDERWINDOW_H
#define GLFWOPENGLRENDERWINDOW_H

#include <glfwopenglsurface.h>

class GLFWOpenGLRenderWindow : public GLFWOpenGLSurface
{
public:
    GLFWOpenGLRenderWindow(QObject* parent,
                           QScreen* outputScreen,
                           OpenGLRenderSpecs nNodeSpecs,
                           const QSurfaceFormat& surfaceFormat,
                           GLFWwindow* sharedContext);
};

#endif // GLFWOPENGLRENDERWINDOW_H
