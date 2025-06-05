// scenewidget.h
#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H
#include <QGraphicsView>
class SceneWidget : public QGraphicsView {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
};
#endif // SCENEWIDGET_H
