#include "TreeNodeItem.h"
#include <QBrush>
#include <QGraphicsSceneMouseEvent>

TreeNodeItem::TreeNodeItem(int value, QGraphicsItem* parent)
    : QObject(nullptr),
    QGraphicsEllipseItem(-20, -20, 40, 40, parent),
    value(value),
    ltag(LINK),
    rtag(LINK),
    left(nullptr),
    right(nullptr) {
    setBrush(Qt::lightGray);
    setPen(QPen(Qt::black, 2));

    textItem = new QGraphicsTextItem(QString::number(value), this);
    textItem->setPos(-10, -15);
    textItem->setDefaultTextColor(Qt::black);
}

int TreeNodeItem::getValue() const {
    return value;
}

void TreeNodeItem::setValue(int value) {
    this->value = value;
    textItem->setPlainText(QString::number(value));
}

TreeNodeItem* TreeNodeItem::leftChild() const {
    return left;
}

TreeNodeItem* TreeNodeItem::rightChild() const {
    return right;
}

void TreeNodeItem::setLeftChild(TreeNodeItem* node) {
    left = node;
}

void TreeNodeItem::setRightChild(TreeNodeItem* node) {
    right = node;
}

void TreeNodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    emit clicked(value);  // 触发信号
    QGraphicsEllipseItem::mousePressEvent(event);
}
