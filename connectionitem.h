// connectionitem.h
#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H
#include <QGraphicsLineItem>
#include <QPen>
class ConnectionItem : public QGraphicsLineItem {
public:
    ConnectionItem(const QPointF& p1, const QPointF& p2,
                   const QPen& pen = QPen(Qt::darkBlue, 2));
    void updateEndpoints();
};
#endif // CONNECTIONITEM_H
