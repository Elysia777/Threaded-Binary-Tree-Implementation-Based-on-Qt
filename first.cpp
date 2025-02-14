#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
using namespace std;

enum Tag { LINK, THREAD };

struct ThreadedTreeNode {
    int data;
    ThreadedTreeNode* left;
    ThreadedTreeNode* right;
    Tag ltag;
    Tag rtag;

    ThreadedTreeNode(int val) : data(val), left(nullptr), right(nullptr), ltag(LINK), rtag(LINK) {}
};
class ThreadedBinaryTree {
private:
    ThreadedTreeNode* root;
    ThreadedTreeNode* prev; // 用于线索化时记录前一个节点

    void inOrderThreading(ThreadedTreeNode* node) {
        if (node == nullptr) return;

        // 线索化左子树
        inOrderThreading(node->left);

        // 处理当前节点
        if (node->left == nullptr) {
            node->left = prev;
            node->ltag = THREAD;
        }
        if (prev != nullptr && prev->right == nullptr) {
            prev->right = node;
            prev->rtag = THREAD;
        }
        prev = node;

        // 线索化右子树
        inOrderThreading(node->right);
    }

public:
    void generateDotFile(const std::string& filename) {
        std::ofstream dotFile(filename);
        dotFile << "digraph ThreadedBinaryTree {\n";
        dotFile << "    node [shape=circle, style=filled, fillcolor=lightblue];\n";

        std::queue<ThreadedTreeNode*> q;
        if (root) q.push(root);

        while (!q.empty()) {
            ThreadedTreeNode* current = q.front();
            q.pop();

            // 输出节点
            dotFile << "    " << current->data << ";\n";

            // 处理左子树
            if (current->ltag == LINK && current->left) {
                dotFile << "    " << current->data << " -> " << current->left->data
                        << " [color=blue];\n";
                q.push(current->left);
            } else if (current->left) {
                dotFile << "    " << current->data << " -> " << current->left->data
                        << " [style=dotted, color=red];\n";
            }

            // 处理右子树
            if (current->rtag == LINK && current->right) {
                dotFile << "    " << current->data << " -> " << current->right->data
                        << " [color=blue];\n";
                q.push(current->right);
            } else if (current->right) {
                dotFile << "    " << current->data << " -> " << current->right->data
                        << " [style=dotted, color=green];\n";
            }
        }
        dotFile << "}\n";
        dotFile.close();
    }
    ThreadedBinaryTree() : root(nullptr), prev(nullptr) {}

    void createThreadedTree(vector<int> vector) {
        if(vector.empty()) {
            root = nullptr;
            return;
        }
        root = new ThreadedTreeNode(vector[0]);
        queue<ThreadedTreeNode*> q;
        q.push(root);
        int i=1;
        while (!q.empty()&&i<vector.size()) {
            ThreadedTreeNode* node = q.front();
            q.pop();
            if(i<vector.size()) {
                node->left = new ThreadedTreeNode(vector[i]);
                q.push(node->left);
            }
            i++;
            if(i<vector.size()) {
                node->right = new ThreadedTreeNode(vector[i]);
                q.push(node->right);
            }
            i++;
        }
        // 线索化二叉树
        inOrderThreading(root);
        ThreadedTreeNode* node = root;
        while(node->right != nullptr) {
            node=node->right;
        }
        node->rtag = THREAD;
    }

    void inOrderTraversal() {
        ThreadedTreeNode* current = root;
        while (current != nullptr) {
            // 找到最左边的节点
            while (current->ltag == LINK) {
                current = current->left;
            }

            // 访问当前节点
            std::cout << current->data << " ";

            // 如果右指针是线索，直接访问后继节点
            while (current->rtag == THREAD && current->right != nullptr) {
                current = current->right;
                std::cout << current->data << " ";
            }

            // 否则，移动到右子树
            current = current->right;
        }
        std::cout << std::endl;
    }
    ThreadedTreeNode* find(int value) {
        queue<ThreadedTreeNode*> q;
        q.push(root);
        while (!q.empty()) {
            ThreadedTreeNode* node = q.front();
            q.pop();
            if(node->data == value) {
                return node;
            }
            if(node->left != nullptr&&node->ltag == LINK) {
                q.push(node->left);
            }
            if(node->right != nullptr&&node->rtag == LINK) {
                q.push(node->right);
            }
        }
        return nullptr;
    }
    void insertNode(int index, int val, bool isLeft) {
        ThreadedTreeNode* newNode = new ThreadedTreeNode(val);
        ThreadedTreeNode* parent=find(index);
        if(parent==nullptr) {
            cout<< "未找到节点" << endl;
            return;
        }
        if (isLeft) {
            newNode->left = parent->left;
            newNode->ltag = parent->ltag;
            parent->left = newNode;
            parent->ltag = LINK;
            newNode->rtag = THREAD;
            newNode->right = parent;
            ThreadedTreeNode* prev = newNode;
            if(prev->ltag == LINK) {
                prev=prev->left;
                while(prev->rtag == LINK) {
                    prev = prev->right;
                }
                prev->right = newNode;
            }
        } else {
            newNode->right = parent->right;
            newNode->rtag = parent->rtag;
            parent->right = newNode;
            parent->rtag = LINK;
            newNode->ltag=THREAD;
            newNode->left = parent;
            ThreadedTreeNode* prev = newNode;
            if(prev->rtag == LINK) {
                prev=prev->right;
                while(prev->ltag == LINK) {
                    prev = prev->left;
                }
                prev->left = newNode;
            }
        }
    }
    ThreadedTreeNode* findParent(ThreadedTreeNode* node, ThreadedTreeNode* target) {
        if (node == nullptr || target == nullptr) return nullptr;

        if ((node->ltag == LINK && node->left == target) || (node->rtag == LINK && node->right == target)) {
            return node;
        }

        ThreadedTreeNode* parent = nullptr;
        if (node->ltag == LINK) {
            parent = findParent(node->left, target);
        }
        if (parent == nullptr && node->rtag == LINK) {
            parent = findParent(node->right, target);
        }
        return parent;
    }
    void deleteNode(int index) {
        ThreadedTreeNode* current = find(index);
        ThreadedTreeNode* parent = findParent(root, current);
        if(current==nullptr) {
            return;
        }
        //没有子节点
        if(current->ltag== THREAD&&current->rtag==THREAD) {
            if (parent == nullptr) {
                root = nullptr;
            } else if (parent->left == current) {
                parent->left = current->left;
                parent->ltag = THREAD;
            } else {
                parent->right = current->right;
                parent->rtag = THREAD;
            }
            delete current;
        }
        //一个子节点
        else if (current->ltag == THREAD || current->rtag == THREAD) {
            ThreadedTreeNode* child = (current->ltag == LINK) ? current->left : current->right;

            if (parent == nullptr) {
                root = child;
            } else if (parent->left == current) {
                parent->left = child;
                if(current->left==child) {
                    while (child->rtag==LINK) {
                        child = child->right;
                    }
                    child->right=current->right;
                }
                else {
                    while (child->ltag==LINK) {
                        child = child->left;
                    }
                    child->left=current->left;
                }
            } else {
                parent->right = child;
                if(current->left==child) {
                    while (child->rtag==LINK) {
                        child=child->right;
                    }
                    child->right=current->right;
                }
                else {
                    while (child->ltag==LINK) {
                        child=child->left;
                    }
                    child->left=current->left;
                }
            }
        }
        //两个子节点
        else {
            ThreadedTreeNode *child=current->left;
            ThreadedTreeNode *pre=current;
            while (child->rtag==LINK) {
                pre = child;
                child=child->right;
            }
            if(child==current->left) {
                if(child->ltag==LINK) {
                    current->left=child->left;
                }else if(child->ltag==THREAD) {
                    current->left=child->left;
                    current->ltag=THREAD;
                }
            }else if (child->ltag == LINK) {
               pre->right=child->left;
           }else if(child->ltag == THREAD) {
               pre->right=child->left;
               pre->rtag=THREAD;
           }
            if(parent==nullptr) {
                root = child;
                child->left=current->left;
                child->ltag=current->ltag;
                child->right=current->right;
                child->rtag=current->rtag;
            }else if(parent->left==current) {
                parent->left=child;
                child->left=current->left;
                child->ltag=current->ltag;
                child->right=current->right;
                child->rtag=current->rtag;
            }else if(parent->right==current) {
                parent->right=child;
                child->left=current->left;
                child->ltag=current->ltag;
                child->right=current->right;
                child->rtag=current->rtag;
            }
           ThreadedTreeNode* left=child;
            if(left->ltag==LINK) {
                left=left->left;
                while(left->rtag==LINK) {
                    left=left->right;
                }
                left->right=child;
                left->rtag=THREAD;
            }
            ThreadedTreeNode* right=child;
            if(right->rtag==LINK) {
                right=right->right;
                while(right->ltag==LINK) {
                    right=right->left;
                }
                right->left=child;
                right->ltag=THREAD;
            }
        }
    }
    void levelOrderTraversalLinkNodes() {
        if (root == nullptr) {
            std::cout << "树为空！" << std::endl;
            return;
        }

        std::queue<ThreadedTreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            ThreadedTreeNode* current = q.front();
            q.pop();

            // 访问当前节点
            std::cout << current->data << " ";

            // 如果左孩子是 LINK 节点，加入队列
            if (current->ltag == LINK && current->left != nullptr) {
                q.push(current->left);
            }

            // 如果右孩子是 LINK 节点，加入队列
            if (current->rtag == LINK && current->right != nullptr) {
                q.push(current->right);
            }
        }
        std::cout << std::endl;
    }
    void generateTreeImage(const std::string& dotFilename, const std::string& outputImage) {
        generateDotFile(dotFilename);
        std::string command = "dot -Tpng " + dotFilename + " -o " + outputImage;
        system(command.c_str());
    }
};


    int main() {
        system("chcp 65001");
        vector<int> vector;//={1,2,3,4,5,6,7,8,9,10};
        int n,n1;
        char c;
        string s;
        ThreadedBinaryTree tree;
        int count=0;
        //tree.inOrderTraversal();
        //tree.insertNode(8,11,false);
        //tree.inOrderTraversal();
        cout<<"输入树的节点数量"<<endl;
        cin>>n;
        cout<<"输入层次遍历的节点"<<endl;
        for (int i = 0; i < n; i++) {
            int val;
            cin>>val;
            vector.push_back(val);
        }
        tree.createThreadedTree(vector);
        tree.generateTreeImage(to_string(count)+".dot",to_string(count)+".png");
        count++;
        while(true) {
            cout << "insert or delete "<< endl;
            cin>>s;
            int i=-1;
            if(s=="insert") { i =0;};
            if(s=="delete") { i =1;};
            switch (i) {
                case 0:
                    cout <<"输入一个节点，想要插入在哪个节点后，想要插入的位置(l/r)"<<endl;
                cin>>n>>n1>>c;
                bool b;
                if(c=='l') {
                    b=true;
                }
                else if(c=='r') {
                    b=false;
                }
                tree.insertNode(n1,n,b);
                tree.generateTreeImage(to_string(count)+".dot",to_string(count)+".png");
                count++;
                break;
                case 1:
                    cout<<"删除一个节点"<<endl;
                cin>>n;
                tree.deleteNode(n);
                tree.generateTreeImage(to_string(count)+".dot",to_string(count)+".png");
                count++;
                break;
                default:
                    cout<<"请输入正确内容"<<endl;
            }
        }
        return 0;
    }