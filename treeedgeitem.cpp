#include "TreeEdgeItem.h"

void TreeEdgeItem::setLine(const QLineF& line) {
    // 创建路径：从起点到终点的直线
    QPainterPath path;
    path.moveTo(line.p1()); // 起点
    path.lineTo(line.p2()); // 终点
    setPath(path);          // 调用基类的 setPath
}

