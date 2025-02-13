#ifndef TREENODEITEM_H
#define TREENODEITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
enum Tag{LINK,THREAD};
class TreeNodeItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT

public:
    TreeNodeItem(int value, QGraphicsItem* parent = nullptr);
    int getValue() const;
    void setValue(int value);
    Tag ltag;
    Tag rtag;
    TreeNodeItem* leftChild() const;
    TreeNodeItem* rightChild() const;
    void setLeftChild(TreeNodeItem* node);
    void setRightChild(TreeNodeItem* node);

signals:
    void clicked(int value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int value;
    QGraphicsTextItem* textItem;
    TreeNodeItem* left;
    TreeNodeItem* right;

};

#endif // TREENODEITEM_H
