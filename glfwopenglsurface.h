#ifndef GLFWOPENGLSURFACE_H
#define GLFWOPENGLSURFACE_H

#include <glfwopenglnode.h>

//
//Native window access
//

//Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   #ifdef _WIN64
        #ifndef GLFW_EXPOSE_NATIVE_WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
        #endif
   #else
   #endif
//MAC OS
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
    #elif TARGET_OS_IPHONE
    #elif TARGET_OS_MAC
        #ifndef GLFW_EXPOSE_NATIVE_COCOA
        #define GLFW_EXPOSE_NATIVE_COCOA
        #endif
    #else
    #endif
//Linux
#elif __linux__
    #ifndef GLFW_EXPOSE_NATIVE_WAYLAND
    #define GLFW_EXPOSE_NATIVE_WAYLAND
    #endif
#elif __unix__ // all unices not caught above
#elif defined(_POSIX_VERSION)
#else
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include<QtGlobal>

#include <QScreen>
#include <QSurfaceFormat>

class GLFWOpenGLSurface : public QObject, public GLFWOpenGLNode
{
    Q_OBJECT
public:
    GLFWOpenGLSurface(QObject* parent,
                      QScreen* outputScreen,
                      OpenGLRenderSpecs nNodeSpecs,
                      const QSurfaceFormat& surfaceFormat,
                      GLFWwindow* sharedContext,
                      bool isOffscreen);

    virtual ~GLFWOpenGLSurface();

    virtual void initialize() override;

    //Windows native resource access
    WId getNativeWindowHandle() const;

    const QSurfaceFormat& getOpenGLFormat();
    GLFWwindow* getOpenGLContext();

protected:
    virtual bool initializeOpenGLContext();

    void swapSurfaceBuffers();

    bool makeContextCurrent();
    void doneContextCurrent();

    QSurfaceFormat openGLFormat;
    GLFWwindow* openGLContext;

    GLFWwindow* sharedOpenGLContext;

    bool offscreen;
};

#endif // GLFWOPENGLSURFACE_H
