#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    renderer(nullptr),
    previewWindow(nullptr),
    renderWindow(nullptr),
    renderThread(nullptr)
{
    ui->setupUi(this);

    bool initialized = initialize();
    assert(initialized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initialize()
{
    renderThread = new QThread();

    GLFWOpenGLNode::OpenGLRenderSpecs renderSpecs =
    GLFWOpenGLNode::OpenGLRenderSpecs{
    GLFWOpenGLNode::OpenGLTextureSpecs{
            640,
            360,
            4,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE
    },
            60.0f};

    renderer = new TriangleRenderer(nullptr,
                                    nullptr,
                                    renderSpecs,
                                    QSurfaceFormat::defaultFormat(),
                                    NULL);
    previewWindow = new Display(nullptr,
                                nullptr,
                                renderSpecs,
                                renderer->getOpenGLFormat(),
                                renderer->getOpenGLContext());
    renderWindow = new Display(nullptr,
                               nullptr,
                               renderSpecs,
                               renderer->getOpenGLFormat(),
                               renderer->getOpenGLContext());

    renderer->moveToThread(renderThread);
    //renderWindow->moveToThread(renderThread);

    QObject::connect(renderer,&TriangleRenderer::frameReady,renderWindow,&Display::setFrame);
    QObject::connect(renderer,&TriangleRenderer::frameReady,previewWindow,&Display::setFrame);

    QObject::connect(renderThread,&QThread::started,renderer,&TriangleRenderer::start);

    QWidget* previewWidget = QWidget::createWindowContainer(QWindow::fromWinId(previewWindow->getNativeWindowHandle()),this);
    previewWidget->setMinimumSize(640,360);
    resize(previewWidget->size());

    renderThread->start();

    return true;
}
