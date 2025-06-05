#include "moduleitem.h"
#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent> // 添加鼠标事件支持
#include <QMessageBox> // 添加消息框支持
#include <QVBoxLayout>

// 详细信息对话框类
class ModuleDetailsDialog : public QDialog {
public:
    ModuleDetailsDialog(const QString& title, const QString& content, QWidget* parent = nullptr)
        : QDialog(parent) {
        setWindowTitle(title);
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        setMinimumSize(300, 200);

        QVBoxLayout* layout = new QVBoxLayout(this);
        QLabel* contentLabel = new QLabel(content);
        contentLabel->setWordWrap(true);
        contentLabel->setTextFormat(Qt::RichText);
        contentLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        layout->addWidget(contentLabel);
    }
};

ModuleItem::ModuleItem(const QString& name, qreal x, qreal y, qreal w, qreal h)
    : QGraphicsRectItem(0, 0, w, h), moduleName(name)
{
    setPos(x, y);
    setBrush(QBrush(Qt::lightGray));
    setAcceptHoverEvents(true); // 接受悬停事件

    // 默认的详细信息
    infoText = QString("<b>基本参数:</b><br>")
               + QString("• 位置: (%1, %2)<br>").arg(x).arg(y)
               + QString("• 尺寸: %1 x %2<br>").arg(w).arg(h);

    // 添加标签
    label = new QGraphicsTextItem(name, this);
    label->setPos(10, 10);
    label->setDefaultTextColor(Qt::black);

    // 初始端口
    addPort(Right);
    addPort(Left);
}

void ModuleItem::setSize(qreal width, qreal height) {
    setRect(0, 0, width, height);
    updatePortPositions();
}

void ModuleItem::addPort(PortPosition pos) {
    QGraphicsEllipseItem* port = new QGraphicsEllipseItem(-4, -4, 8, 8, this);
    port->setBrush(Qt::yellow);
    ports.append({port, pos});
    updatePortPositions();
}

QPointF ModuleItem::getPortPos(int portId) const {
    if(portId >= 0 && portId < ports.size()) {
        QPointF localPos;
        const Port& port = ports[portId];
        const QRectF rect = this->rect();

        switch(port.position) {
        case Left:   localPos = QPointF(0, rect.height()/2); break;
        case Right:  localPos = QPointF(rect.width(), rect.height()/2); break;
        case Top:    localPos = QPointF(rect.width()/2, 0); break;
        case Bottom: localPos = QPointF(rect.width()/2, rect.height()); break;
        }
        return mapToScene(localPos);
    }
    return pos();
}

void ModuleItem::updatePortPositions() {
    const QRectF rect = this->rect();
    for(int i=0; i<ports.size(); ++i) {
        QGraphicsEllipseItem* port = ports[i].shape;
        switch(ports[i].position) {
        case Left:   port->setPos(0, rect.height()/2 - 4); break;
        case Right:  port->setPos(rect.width(), rect.height()/2 - 4); break;
        case Top:    port->setPos(rect.width()/2 - 4, 0); break;
        case Bottom: port->setPos(rect.width()/2 - 4, rect.height()); break;
        }
    }
}

void ModuleItem::setName(const QString& name) {
    moduleName = name;
    label->setPlainText(name);
}

void ModuleItem::setInfoText(const QString& text) {
    infoText = text;
}

void ModuleItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    // 调用基类事件处理
    QGraphicsRectItem::mousePressEvent(event);

    // 创建并显示详细信息对话框
    ModuleDetailsDialog* dialog = new ModuleDetailsDialog(moduleName, infoText);
    dialog->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    dialog->show();
}
