// connectionitem.cpp
#include "connectionitem.h"
#include <QPen>

ConnectionItem::ConnectionItem(const QPointF& p1, const QPointF& p2, const QPen& pen)
    : QGraphicsLineItem(p1.x(), p1.y(), p2.x(), p2.y())
{
    setPen(pen);
    setZValue(-1); // 确保连接线在模块下方
}
