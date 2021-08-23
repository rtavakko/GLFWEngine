#include "trianglerenderer.h"

TriangleRenderer::TriangleRenderer(QObject *parent,
                                   QScreen *outputScreen,
                                   GLFWOpenGLNode::OpenGLRenderSpecs nNodeSpecs,
                                   const QSurfaceFormat &surfaceFormat,
                                   GLFWwindow *sharedContext) :
    GLFWOpenGLRenderSurface(parent,
                            outputScreen,
                            nNodeSpecs,
                            surfaceFormat,
                            sharedContext),
    syncTimer(nullptr),
    depthrenderbuffer(0),
    trianglePositionAttributeLocation(0),
    triangleColorAttributeLocation(0),
    triangleMatrixUniformLocation(0),
    triangleAngle(0.0f)
{
    //Initialize sync timer
    initializeTimer();
    qRegisterMetaType<GLuint>("GLuint");
}

void TriangleRenderer::start()
{
    float timeOut = 1000.0f/renderSpecs.frameRate;
    syncTimer->start(timeOut);
}

void TriangleRenderer::stop()
{
    syncTimer->stop();
}

void TriangleRenderer::renderFrame()
{
    updateStartTime();

    //Render to FBO
    if(!makeContextCurrent())
        return;

    initialize();

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glBindVertexArray(vaoID);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(programID);

    updateUniforms();

    glViewport(0, 0, renderSpecs.frameType.width, renderSpecs.frameType.height);

    glDrawBuffers(1,&GL_outputColorAttachment);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(0);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    emit frameReady(outputTextureID,
                    renderSpecs.frameType.width,
                    renderSpecs.frameType.height);

    swapSurfaceBuffers();
    doneContextCurrent();

    updateEndTime();
}

void TriangleRenderer::initializeFBO()
{
    //Generate output FBO and texture
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenTextures(1, &outputTextureID);

    glBindTexture(GL_TEXTURE_2D, outputTextureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, renderSpecs.frameType.internalFormat, renderSpecs.frameType.width, renderSpecs.frameType.height, 0, renderSpecs.frameType.format, renderSpecs.frameType.dataType, (const GLvoid*)(nullptr));
    glGenerateMipmap(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_outputColorAttachment, GL_TEXTURE_2D, outputTextureID, 0);

    //Depth render buffer
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderSpecs.frameType.width, renderSpecs.frameType.height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TriangleRenderer::initializeShaderProgram()
{
    //Create debug triangle OpenGL program; add vertex and fragment shaders, link and bind
    QFile vertFile(QString(":/GLSL/triangleVertex.glsl"));
    QFile fragFile(QString(":/GLSL/triangleFragment.glsl"));

    if (!vertFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!fragFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    programID = loadShaders(vertFile.readAll().toStdString(),
                            fragFile.readAll().toStdString());

    glUseProgram(programID);

    //Get locations of vertex shader attributes
    trianglePositionAttributeLocation = glGetAttribLocation(programID, "positionAttribute");
    triangleColorAttributeLocation = glGetAttribLocation(programID, "colorAttribute");

    triangleMatrixUniformLocation = glGetUniformLocation(programID, "matrix");

    glUseProgram(0);
}

void TriangleRenderer::initializeVertexBuffers()
{
    static GLfloat triangleData[3][5] = {{0.0f, 0.707f, 1.0f, 0.0f, 0.0f},
                                         {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
                                         {0.5f, -0.5f, 0.0f, 0.0f, 1.0f}
    };

    //Generate and bind triangle VAO
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    //Generate and bind VBO
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 15*sizeof(GLfloat), triangleData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(trianglePositionAttributeLocation);
    glEnableVertexAttribArray(triangleColorAttributeLocation);

    glVertexAttribPointer(trianglePositionAttributeLocation, 2, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const void*)(0));
    glVertexAttribPointer(triangleColorAttributeLocation, 3, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const void*)(2*sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void TriangleRenderer::initializeUniforms()
{
    triangleAngle += 1.0f;
    triangleAngle = (triangleAngle>360.0f)?(0.0f):(triangleAngle);

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0.0f, 0.0f, -2.0f);
    matrix.rotate(triangleAngle, 0.0f, 1.0f, 0.0f);

    glUniformMatrix4fv(triangleMatrixUniformLocation,
                       1,
                       GL_FALSE,
                       (const GLfloat*)(matrix.constData()));
}

void TriangleRenderer::updateUniforms()
{
    triangleAngle += 1.0f;
    triangleAngle = (triangleAngle>360.0f)?(0.0f):(triangleAngle);

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0.0f, 0.0f, -2.0f);
    matrix.rotate(triangleAngle, 0.0f, 1.0f, 0.0f);

    glUniformMatrix4fv(triangleMatrixUniformLocation,
                       1,
                       GL_FALSE,
                       (const GLfloat*)(matrix.constData()));
}

void TriangleRenderer::initializeTimer()
{
    syncTimer = new QTimer(this);
    syncTimer->setTimerType(Qt::PreciseTimer);

    QObject::connect(syncTimer,&QTimer::timeout,this,&TriangleRenderer::renderFrame);
}
