#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWindow>

#include <QThread>

#include <trianglerenderer.h>
#include <display.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool initialize();

    Ui::MainWindow *ui;

    TriangleRenderer* renderer;

    Display* previewWindow;
    Display* renderWindow;

    QThread* renderThread;
};

#endif // MAINWINDOW_H
