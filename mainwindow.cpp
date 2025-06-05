// mainwindow.cpp
#include "mainwindow.h"
#include "scenewidget.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    SceneWidget* widget = new SceneWidget(this);
    setCentralWidget(widget);

    // 确保窗口足够大
    resize(1200, 900);

    // 设置窗口标题
    setWindowTitle("优化后的总线拓扑可视化");
}
