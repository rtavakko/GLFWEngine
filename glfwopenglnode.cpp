#include "glfwopenglnode.h"

GLFWOpenGLNode::GLFWOpenGLNode(GLFWOpenGLNode::OpenGLRenderSpecs specs) :
    initialized(false),
    renderSpecs(specs),
    programID(0),
    vertexAttributeLocation(0),
    texCoordAttributeLocation(0),
    textureUniformLocation(0),
    vaoID(0),
    vboID(0),
    fboID(0),
    textureUnit(0),
    outputTextureID(0)
{
}

GLFWOpenGLNode::~GLFWOpenGLNode()
{

}

GLuint GLFWOpenGLNode::getTextureID() const
{
    return outputTextureID;
}

GLFWOpenGLNode::OpenGLRenderSpecs GLFWOpenGLNode::getSpecs() const
{
    return renderSpecs;
}

void GLFWOpenGLNode::initialize()
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

void GLFWOpenGLNode::resize(unsigned int w, unsigned int h)
{
    if(w == 0 || h == 0)
        return;

    if(renderSpecs.frameType.width != w || renderSpecs.frameType.height != h)
    {
        renderSpecs.frameType.width = w;
        renderSpecs.frameType.height = h;

        resizeFBO();
    }
}

void GLFWOpenGLNode::updateSpecs(GLFWOpenGLNode::OpenGLRenderSpecs specs)
{
    if(renderSpecs.frameType.width == 0 || renderSpecs.frameType.height == 0 || renderSpecs.frameType.channels == 0)
        return;

    if(
            renderSpecs.frameType.width != specs.frameType.width ||
            renderSpecs.frameType.height != specs.frameType.height ||
            renderSpecs.frameType.channels != specs.frameType.channels ||
            renderSpecs.frameRate != specs.frameRate ||
            renderSpecs.frameType.internalFormat != specs.frameType.internalFormat ||
            renderSpecs.frameType.format != specs.frameType.format ||
            renderSpecs.frameType.dataType != specs.frameType.dataType)
    {
        renderSpecs = specs;

        resizeFBO();
    }
}

void GLFWOpenGLNode::setFrameRate(float fps)
{
    if(fps <= 0.0f)
        return;

    renderSpecs.frameRate = fps;
}

void GLFWOpenGLNode::renderFrame()
{
     //Render to default FBO
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glBindVertexArray(vaoID);

     glClearColor(0.0f,0.0f,0.0f,1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
     //glDisable(GL_DEPTH_TEST);

     glUseProgram(programID);

     //Update shader uniform values
     updateUniforms();

     glViewport(0,0,renderSpecs.frameType.width,renderSpecs.frameType.height);
     glDrawArrays(GL_TRIANGLES, 0, 6);

     glBindTexture(GL_TEXTURE_2D, 0);
     glActiveTexture(GL_TEXTURE0);

     glUseProgram(0);

     glBindVertexArray(0);
}

GLuint GLFWOpenGLNode::loadShaders(std::string vertShader, std::string fragShader)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const * VertexSourcePointer = vertShader.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    char const * FragmentSourcePointer = fragShader.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

bool GLFWOpenGLNode::initializeOpenGLFunctions()
{
    glewExperimental = true;                                        // Needed in core profile
    if (glewInit() != GLEW_OK)                                      // Initialize GLEW
        return false;

    glEnable(GL_DEPTH_TEST);

    return true;
}

void GLFWOpenGLNode::initializeFBO()
{
    //Generate output FBO and texture
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenTextures(1, &outputTextureID);

    glBindTexture(GL_TEXTURE_2D, outputTextureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, renderSpecs.frameType.internalFormat, renderSpecs.frameType.width, renderSpecs.frameType.height, 0, renderSpecs.frameType.format, renderSpecs.frameType.dataType, (const GLvoid*)(nullptr));
    glGenerateMipmap(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_outputColorAttachment, GL_TEXTURE_2D, outputTextureID, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFWOpenGLNode::initializeShaderProgram()
{
    //Create main OpenGL program; add vertex and fragment shaders, link and bind
    QFile vertFile(QString(":/GLSL/passVertex.glsl"));
    QFile fragFile(QString(":/GLSL/passFragment.glsl"));

    if (!vertFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!fragFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    programID = loadShaders(vertFile.readAll().toStdString(),
                            fragFile.readAll().toStdString());

    glUseProgram(programID);

    //Get locations of vertex shader attributes
    vertexAttributeLocation = glGetAttribLocation(programID, "vertex");
    texCoordAttributeLocation = glGetAttribLocation(programID, "texCoord");

    textureUniformLocation = glGetUniformLocation(programID, "texture");

    glGetUniformiv(programID, textureUniformLocation, &textureUnit);   //Set the value of the texture unit (GL_TEXTUREX) so it can be used in glActiveTexture

    glUseProgram(0);
}

void GLFWOpenGLNode::initializeVertexBuffers()
{
    //Vertex and texture positions of image quad
    static const GLfloat vertexData[6][4] = {{-1.0f,-1.0f,0.0f,0.0f},{1.0f,-1.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},
                                             {-1.0f,-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{-1.0f,1.0f,0.0f,1.0f}};
    //Generate and bind VAO
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    //Generate and bind VBO
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vertexAttributeLocation);
    glEnableVertexAttribArray(texCoordAttributeLocation);

    glVertexAttribPointer(vertexAttributeLocation, 2, GL_FLOAT, GL_TRUE, 4*sizeof(GLfloat), (const void*)(0));
    glVertexAttribPointer(texCoordAttributeLocation, 2, GL_FLOAT, GL_TRUE, 4*sizeof(GLfloat), (const void*)(2*sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GLFWOpenGLNode::initializeUniforms()
{
    //Resize output texture
    glBindTexture(GL_TEXTURE_2D, outputTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, renderSpecs.frameType.internalFormat, renderSpecs.frameType.width, renderSpecs.frameType.height, 0, renderSpecs.frameType.format, renderSpecs.frameType.dataType, (const GLvoid*)(nullptr));

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLFWOpenGLNode::resizeFBO()
{
    //Resize output texture
    glBindTexture(GL_TEXTURE_2D, outputTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, renderSpecs.frameType.internalFormat, renderSpecs.frameType.width, renderSpecs.frameType.height, 0, renderSpecs.frameType.format, renderSpecs.frameType.dataType, (const GLvoid*)(nullptr));

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLFWOpenGLNode::updateUniforms()
{

}

void GLFWOpenGLNode::updateStartTime()
{
    t_start = std::chrono::high_resolution_clock::now();
}

void GLFWOpenGLNode::updateEndTime()
{
    t_end = std::chrono::high_resolution_clock::now();
    t_delta = t_end - t_start;
}
