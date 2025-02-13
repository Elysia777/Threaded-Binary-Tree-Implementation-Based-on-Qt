#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QLineEdit>
#include<QRadioButton>
#include "TreeNodeItem.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onNodeClicked(int value);

private:
    void drawTree();
    void drawNode(TreeNodeItem* node, int x, int y, int offset);
    void drawEdge(TreeNodeItem* from, TreeNodeItem* to);
    void resetNodeColors(TreeNodeItem* node);
    void drawThreadEdge(TreeNodeItem* from, TreeNodeItem* to);
    TreeNodeItem* findParent(TreeNodeItem* node, TreeNodeItem* target);
    TreeNodeItem* findNode(TreeNodeItem* node, int value);

    QGraphicsScene* scene;
    QGraphicsView* view;
    QRadioButton* option1;
    QRadioButton* option2;
    QLineEdit* nodeInput;
    QPushButton* addButton;
    QPushButton* deleteButton;

    TreeNodeItem* root;
    TreeNodeItem* selectedNode;
};

#endif // MAINWINDOW_H
