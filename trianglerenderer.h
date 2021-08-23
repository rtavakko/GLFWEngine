#ifndef TRIANGLERENDERER_H
#define TRIANGLERENDERER_H

#include <glfwopenglrendersurface.h>

#include <QTimer>
#include <QMatrix4x4>

class TriangleRenderer : public GLFWOpenGLRenderSurface
{
    Q_OBJECT
public:
    TriangleRenderer(QObject* parent,
                     QScreen* outputScreen,
                     OpenGLRenderSpecs nNodeSpecs,
                     const QSurfaceFormat& surfaceFormat,
                     GLFWwindow* sharedContext);
public slots:

    virtual void start();
    virtual void stop();

    virtual void renderFrame() override;

signals:
    void frameReady(GLuint texID, unsigned int width, unsigned int height);
    void renderedFrame(double actualFPS);

protected:

    virtual void initializeFBO() override;
    virtual void initializeShaderProgram() override;
    virtual void initializeVertexBuffers() override;
    virtual void initializeUniforms() override;

    virtual void updateUniforms() override;

    virtual void initializeTimer();

    //Sync timer
    QTimer* syncTimer;

    //For rendering a debug triangle
    GLuint depthrenderbuffer;

    GLint trianglePositionAttributeLocation;
    GLint triangleColorAttributeLocation;
    GLint triangleMatrixUniformLocation;

    float triangleAngle;
};

#endif // TRIANGLERENDERER_H
