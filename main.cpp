#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //Create the global OpenGL surface / context
    QSurfaceFormat format;
    format.setDepthBufferSize(32);
    format.setStencilBufferSize(8);

    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(8);

    format.setSwapBehavior(QSurfaceFormat::SwapBehavior(2));

    format.setSwapInterval(1);  //Turn Vsync on (1) / off (0)

    format.setVersion(4,
                      1);

    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);

    //Make sure all OpenGL contexts share resources
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
