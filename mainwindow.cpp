#include "MainWindow.h"
#include "TreeEdgeItem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), root(nullptr), selectedNode(nullptr) {
    // 初始化场景和视图
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);


    view->setRenderHint(QPainter::Antialiasing);

    // 初始化输入框和按钮
    nodeInput = new QLineEdit(this);
     option1 = new QRadioButton("左", this);
     option2 = new QRadioButton("右", this);
    addButton = new QPushButton("添加节点", this);
    deleteButton = new QPushButton("删除节点", this);

    // 布局
    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(nodeInput);
    inputLayout->addWidget(option1);
    inputLayout->addWidget(option2);
    inputLayout->addWidget(addButton);
    inputLayout->addWidget(deleteButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(view);
    mainLayout->addLayout(inputLayout);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 连接信号槽
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);

    // 初始化根节点
    root = new TreeNodeItem(1);
    root->ltag=THREAD;
    root->rtag=THREAD;
    drawTree();
}

MainWindow::~MainWindow() {}

void MainWindow::drawTree() {

    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items) {
        if (TreeNodeItem* node = dynamic_cast<TreeNodeItem*>(item)) {
            disconnect(node, &TreeNodeItem::clicked, this, &MainWindow::onNodeClicked);
        }
    }
    for (QGraphicsItem* item : items) {
        if (dynamic_cast<TreeEdgeItem*>(item)) {
            scene->removeItem(item);
            delete item;
        }
    }
    selectedNode=nullptr;
    if (root) {
        drawNode(root, 400, 50, 200);
    }
}

void MainWindow::drawNode(TreeNodeItem* node, int x, int y, int offset) {
    node->setPos(x, y);
    scene->addItem(node);
    connect(node, &TreeNodeItem::clicked, this, &MainWindow::onNodeClicked);

    // 绘制左子树
    if (node->leftChild()&&node->ltag==LINK) {
        drawNode(node->leftChild(), x - offset, y + 80, offset / 2);
        drawEdge(node, node->leftChild());
    }else if(node->leftChild()&&node->ltag==THREAD){
        drawThreadEdge(node,node->leftChild());
    }

    // 绘制右子树
    if (node->rightChild()&&node->rtag==LINK) {
        drawNode(node->rightChild(), x + offset, y + 80, offset / 2);
        drawEdge(node, node->rightChild());
    }else if(node->rightChild()&&node->rtag==THREAD){
        drawThreadEdge(node,node->rightChild());
    }
}

void MainWindow::drawEdge(TreeNodeItem* from, TreeNodeItem* to) {
    // 定义直线的起点和终点
    QLineF line(from->pos().x(), from->pos().y(), to->pos().x(), to->pos().y());

    // 创建边对象
    TreeEdgeItem* edge = new TreeEdgeItem;
    edge->setLine(line);

    scene->addItem(edge);
}
void MainWindow::drawThreadEdge(TreeNodeItem* from, TreeNodeItem* to) {
    QPointF start = from->pos();
    QPointF end = to->pos();

    // 创建路径
    QPainterPath path;
    path.moveTo(start);


    QPointF ctrl((start.x() + end.x()) / 2, (start.y() + end.y()) / 2 - 50); // 控制点向上偏移
    path.quadTo(ctrl, end); // 绘制弧线


    TreeEdgeItem* edge = new TreeEdgeItem;
    edge->setPath(path);

    // 设置样式：红色虚线
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(1);
    edge->setPen(pen);

    scene->addItem(edge);
}
void MainWindow::onAddButtonClicked() {
    int value = nodeInput->text().toInt();
    if (selectedNode) {
        TreeNodeItem* newNode = new TreeNodeItem(value);
        if(option1->isChecked()){
        newNode->ltag=selectedNode->ltag;
        newNode->setLeftChild(selectedNode->leftChild());
        selectedNode->setLeftChild(newNode);
        selectedNode->ltag=LINK;
        newNode->rtag=THREAD;
        newNode->setRightChild(selectedNode);
        TreeNodeItem* prev=newNode;
        if(prev->ltag==LINK){
            prev=prev->leftChild();
            while(prev->ltag==LINK){
                prev=prev->rightChild();
            }
            prev->setRightChild(newNode);
        }}else if(option2->isChecked()){
            newNode->setRightChild(selectedNode->rightChild());
            newNode->rtag=selectedNode->rtag;
            selectedNode->setRightChild(newNode);
            selectedNode->rtag=LINK;
            newNode->ltag=THREAD;
            newNode->setLeftChild(selectedNode);
            TreeNodeItem* prev=newNode;
            if(prev->rtag==LINK){
                prev=prev->rightChild();
                while(prev->ltag==LINK){
                    prev=prev->leftChild();
                }
                prev->setLeftChild(newNode);
            }
        }else{QMessageBox::warning(this,"错误","请先选择一个插入方向");}
        drawTree();
    } else {
        QMessageBox::warning(this, "错误", "请先选择一个节点！");
    }
}

void MainWindow::onDeleteButtonClicked() {
    if (selectedNode) {
        scene->removeItem(selectedNode);
        TreeNodeItem* parent = findParent(root, selectedNode);
        if(selectedNode==nullptr) {
            return;
        }
        //没有子节点
        if(selectedNode->ltag== THREAD&&selectedNode->rtag==THREAD) {
            if (parent == nullptr) {
                root = nullptr;
            } else if (parent->leftChild() == selectedNode) {
                parent->setLeftChild( selectedNode->leftChild());
                parent->ltag = THREAD;
            } else {
                parent->setRightChild(selectedNode->rightChild());
                parent->rtag = THREAD;
            }
        }
        //一个子节点
        else if (selectedNode->ltag == THREAD || selectedNode->rtag == THREAD) {
            TreeNodeItem* child = (selectedNode->ltag == LINK) ? selectedNode->leftChild() : selectedNode->rightChild();

            if (parent == nullptr) {
                root = child;
            } else if (parent->leftChild() == selectedNode) {
                parent->setLeftChild(child);
                if(selectedNode->leftChild()==child) {
                    while (child->rtag==LINK) {
                        child = child->rightChild();
                    }
                    child->setRightChild(selectedNode->rightChild());
                }
                else {
                    while (child->ltag==LINK) {
                        child = child->leftChild();
                    }
                    child->setLeftChild(selectedNode->leftChild());
                }
            } else {
                parent->setRightChild( child);
                if(selectedNode->leftChild()==child) {
                    while (child->rtag==LINK) {
                        child=child->rightChild();
                    }
                    child->setRightChild(selectedNode->rightChild());
                }
                else {
                    while (child->ltag==LINK) {
                        child=child->leftChild();
                    }
                    child->setLeftChild(selectedNode->leftChild());
                }
            }
        }
        //两个子节点
        else {
            TreeNodeItem *child=selectedNode->leftChild();
            TreeNodeItem *pre=selectedNode;
            while (child->rtag==LINK) {
                pre = child;
                child=child->rightChild();
            }
            if(child==selectedNode->leftChild()) {
                if(child->ltag==LINK) {
                    selectedNode->setLeftChild(child->leftChild());
                }else if(child->ltag==THREAD) {
                    selectedNode->setLeftChild(child->leftChild());
                    selectedNode->ltag=THREAD;
                }
            }else if (child->ltag == LINK) {
                pre->setRightChild(child->leftChild());
            }else if(child->ltag == THREAD) {
                pre->setRightChild(child->leftChild());
                pre->rtag=THREAD;
            }
            if(parent==nullptr) {
                root = child;
                child->setLeftChild(selectedNode->leftChild());
                child->ltag=selectedNode->ltag;
                child->setRightChild(selectedNode->rightChild());
                child->rtag=selectedNode->rtag;
            }else if(parent->leftChild()==selectedNode) {
                parent->setLeftChild(child);
                child->setLeftChild(selectedNode->leftChild());
                child->ltag=selectedNode->ltag;
                child->setRightChild(selectedNode->rightChild());
                child->rtag=selectedNode->rtag;
            }else if(parent->rightChild()==selectedNode) {
                parent->setRightChild(child);
                child->setLeftChild(selectedNode->leftChild());
                child->ltag=selectedNode->ltag;
                child->setRightChild(selectedNode->rightChild());
                child->rtag=selectedNode->rtag;
            }
            TreeNodeItem* left=child;
            if(left->ltag==LINK) {
                left=left->leftChild();
                while(left->rtag==LINK) {
                    left=left->rightChild();
                }
                left->setRightChild(child);
                left->rtag=THREAD;
            }
            TreeNodeItem* right=child;
            if(right->rtag==LINK) {
                right=right->rightChild();
                while(right->ltag==LINK) {
                    right=right->leftChild();
                }
                right->setLeftChild(child);
                right->ltag=THREAD;
            }
        }
        selectedNode=nullptr;
        drawTree();
    } else {
        QMessageBox::warning(this, "错误", "请先选择一个节点！");
    }

}

void MainWindow::onNodeClicked(int value) {
    // 重置所有节点的颜色
    resetNodeColors(root);

    // 查找并高亮选中的节点
    selectedNode = findNode(root, value);
    if (selectedNode) {
        selectedNode->setBrush(Qt::yellow); // 高亮选中节点
    }
}
TreeNodeItem* MainWindow::findParent(TreeNodeItem* node,TreeNodeItem* target){
    if(root==nullptr||target==nullptr)return nullptr;
    if((node->ltag==LINK&&node->leftChild()==target)||(node->rtag==LINK&&node->rightChild()==target)){
        return node;
    }
    TreeNodeItem* parent=nullptr;
    if(node->ltag==LINK){
        parent=findParent(node->leftChild(),target);
    }
    if (parent == nullptr && node->rtag == LINK) {
        parent = findParent(node->rightChild(), target);
    }
    return parent;
}
void MainWindow::resetNodeColors(TreeNodeItem* node) {
    if (!node) return;

    // 重置当前节点的颜色
    node->setBrush(Qt::white); // 默认颜色

    // 递归重置左子树和右子树的颜色
    if(node->ltag==LINK){
        resetNodeColors(node->leftChild());}
    if(node->rtag==LINK){
        resetNodeColors(node->rightChild());}
}
TreeNodeItem* MainWindow::findNode(TreeNodeItem* node, int value) {
    if (!node) return nullptr;
    if (node->getValue() == value) return node;

    TreeNodeItem* result = nullptr;
    if (node->ltag == LINK) {
        result = findNode(node->leftChild(), value);
    }
    if (!result && node->rtag == LINK) {
        result = findNode(node->rightChild(), value);
    }
    return result;
}
