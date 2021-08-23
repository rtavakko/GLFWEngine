#ifndef DISPLAY_H
#define DISPLAY_H

#include <glfwopenglrenderwindow.h>

class Display : public GLFWOpenGLRenderWindow
{
public:
    Display(QObject* parent,
            QScreen* outputScreen,
            OpenGLRenderSpecs nNodeSpecs,
            const QSurfaceFormat& surfaceFormat,
            GLFWwindow* sharedContext);

public slots:

    virtual void renderFrame();
    void setFrame(GLuint texID, unsigned int width, unsigned int height);

protected:
    GLuint inputTextureID;
};

#endif // DISPLAY_H
