#ifndef MODULEITEM_H
#define MODULEITEM_H

#include <QGraphicsRectItem>
#include <QVector>
#include <QDialog> // 添加对话框支持
#include <QLabel> // 添加标签支持

// 前置声明
class ModuleDetailsDialog;

class ModuleItem : public QGraphicsRectItem {
public:
    enum PortPosition { Left, Right, Top, Bottom };
    explicit ModuleItem(const QString& name, qreal x, qreal y,
                        qreal w = 100, qreal h = 60);
    void setSize(qreal width, qreal height);
    void addPort(PortPosition pos);
    QPointF getPortPos(int portId) const;
    void setName(const QString& name);
    void setInfoText(const QString& text); // 新增：设置详细信息文本

    // 新添加的获取端口数量方法
    int getPortsCount() const { return ports.size(); }

    // 新添加的设置画刷方法
    void setBrush(const QBrush& brush) {
        QGraphicsRectItem::setBrush(brush);
    }

protected:
    // 重写鼠标点击事件
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    struct Port {
        QGraphicsEllipseItem* shape;
        PortPosition position;
    };
    QVector<Port> ports;
    QString moduleName;
    QString infoText; // 新增：存储详细信息文本
    QGraphicsTextItem* label;
    void updatePortPositions();
};

#endif // MODULEITEM_H
