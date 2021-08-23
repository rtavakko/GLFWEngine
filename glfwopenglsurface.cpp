#include "glfwopenglsurface.h"

GLFWOpenGLSurface::GLFWOpenGLSurface(QObject *parent,
                                     QScreen *outputScreen,
                                     GLFWOpenGLNode::OpenGLRenderSpecs nNodeSpecs,
                                     const QSurfaceFormat &surfaceFormat,
                                     GLFWwindow *sharedContext,
                                     bool isOffscreen) :
    QObject(parent),
    GLFWOpenGLNode(nNodeSpecs),
    openGLFormat(surfaceFormat),
    openGLContext(nullptr),
    sharedOpenGLContext(sharedContext),
    offscreen(isOffscreen)
{
    //Create a window and OpenGL context
    bool contextInitialized = initializeOpenGLContext();
    assert(contextInitialized);
}

GLFWOpenGLSurface::~GLFWOpenGLSurface()
{

}

void GLFWOpenGLSurface::initialize()
{
    if(initialized)
        return;

    bool openGLInitialized = initializeOpenGLFunctions();
    assert(openGLInitialized);

    initializeShaderProgram();
    initializeVertexBuffers();
    initializeFBO();
    initializeUniforms();

    initialized = true;
}

WId GLFWOpenGLSurface::getNativeWindowHandle() const
{
    #if defined GLFW_EXPOSE_NATIVE_WIN32
    return (WId)(glfwGetWin32Window(openGLContext));
    #elif defined GLFW_EXPOSE_NATIVE_COCOA
    return (WId)(glfwGetCocoaWindow(openGLContext));
    #elif defined GLFW_EXPOSE_NATIVE_WAYLAND
    return (WId)(glfwGetWaylandWindow(openGLContext));
    #else
    return (WId)(NULL);
    #endif
}

const QSurfaceFormat &GLFWOpenGLSurface::getOpenGLFormat()
{
    return openGLFormat;
}

GLFWwindow *GLFWOpenGLSurface::getOpenGLContext()
{
    return openGLContext;
}

bool GLFWOpenGLSurface::initializeOpenGLContext()
{
    if(!glfwInit())
        return false;

    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    glfwSwapInterval(openGLFormat.swapInterval());

    glfwWindowHint(GLFW_VISIBLE, offscreen?GLFW_FALSE:GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);                                // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGLFormat.majorVersion());                  // We want OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGLFormat.minorVersion());
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL

    glfwWindowHint(GLFW_DEPTH_BITS, openGLFormat.depthBufferSize());
    glfwWindowHint(GLFW_STENCIL_BITS, openGLFormat.stencilBufferSize());

    glfwWindowHint(GLFW_RED_BITS, openGLFormat.redBufferSize());
    glfwWindowHint(GLFW_GREEN_BITS, openGLFormat.greenBufferSize());
    glfwWindowHint(GLFW_BLUE_BITS, openGLFormat.blueBufferSize());
    glfwWindowHint(GLFW_ALPHA_BITS, openGLFormat.alphaBufferSize());

    glfwWindowHint(GLFW_DOUBLEBUFFER, (openGLFormat.swapBehavior()==QSurfaceFormat::DoubleBuffer)?GLFW_TRUE:GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    // Open a window and create its OpenGL context
    openGLContext = glfwCreateWindow( renderSpecs.frameType.width, renderSpecs.frameType.height, "Render Surface", NULL, sharedOpenGLContext);
    if( openGLContext == NULL )
    {
        glfwTerminate();
        return false;
    }

    return true;
}

void GLFWOpenGLSurface::swapSurfaceBuffers()
{
    glfwSwapBuffers(openGLContext);
}

bool GLFWOpenGLSurface::makeContextCurrent()
{
    if(openGLContext == glfwGetCurrentContext())
        return true;

    glfwMakeContextCurrent(openGLContext);return true;
    return (openGLContext == glfwGetCurrentContext());
}

void GLFWOpenGLSurface::doneContextCurrent()
{
    glfwMakeContextCurrent(NULL);
}
