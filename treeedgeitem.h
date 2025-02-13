#ifndef TREEEDGEITEM_H
#define TREEEDGEITEM_H

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPen>

class TreeEdgeItem : public QGraphicsPathItem {
public:
    TreeEdgeItem(QGraphicsItem* parent = nullptr)
        : QGraphicsPathItem(parent) {
        // 设置默认样式
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(2);
        setPen(pen);
    }
void setLine(const QLineF& line);
    void setPath(const QPainterPath& path) {
        QGraphicsPathItem::setPath(path); // 调用基类的 setPath
    }
};

#endif // TREEEDGEITEM_H
