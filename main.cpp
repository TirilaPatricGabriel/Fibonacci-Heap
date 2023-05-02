#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

class Node{
public:
    int value;
    Node* parent;
    Node* left;
    Node* right;
    Node* child;
    bool marked;
    int degree;
    bool seen;
    Node(int val);
    Node();
    friend ostream& operator <<(ostream& out, const Node&);
    Node& operator =(const Node&);
};
ostream& operator <<(ostream& out, const Node& n){
    cout<<"============NODE============";
    cout<<"VALUE: "<<n.value<<" ";
    if(n.parent != nullptr)
        cout<<"PARENT: "<<n.parent->value<<" ";
    if(n.right != nullptr)
        cout<<"RIGHT: "<<n.right->value<<" ";
    if(n.child != nullptr)
        cout<<"CHILD: "<<n.child->value<<" ";
    if(n.left != nullptr)
        cout<<"LEFT: "<<n.left->value<<endl;

    return out;
}
Node::Node(int val){
    this->value = val;
    this->parent = nullptr;
    this->child = nullptr;
    this->right = this;
    this->left = this;
    this->marked = false;
    this->seen = false;
    this->degree = 0;
}
Node::Node(){
    this->value = 0;
    this->parent = nullptr;
    this->child = nullptr;
    this->right = this;
    this->left = this;
    this->marked = false;
    this->seen = false;
    this->degree = 0;
}
Node& Node::operator=(const Node& node){
    if(this != &node){
        this->value = node.value;
        this->parent = node.parent;
        this->left = node.left;
        this->right = node.right;
        this->child = node.child;
        this->marked = node.marked;
        this->degree = node.degree;
        this->seen = node.seen;
    }
    return *this;
}

class FibonacciHeap{
    Node* maxNode;
    Node* head;
    int size;
public:
    FibonacciHeap(){ this->maxNode = nullptr; this->size = 0; }
    FibonacciHeap(int size, Node* max){
        this->size = size;
        this->maxNode = max;
    }
    void insert(int val){
        Node* n = new Node(val);
        if(this->maxNode == nullptr){
            this->maxNode = n;
        } else {
            Node* last = this->maxNode->left;

            n->right = maxNode;
            n->left = last;
            last->right = n;
            maxNode->left = n;

            if(this->maxNode->value < n->value) this->maxNode = n;
        }
        this->size+=1;
    }

    FibonacciHeap& heapUnion(FibonacciHeap& heap2) {
        this->size += heap2.size;
        bool ok = true;
        if (this->maxNode == nullptr){
            this->maxNode=heap2.maxNode;
        }

        if(this->maxNode == nullptr || heap2.maxNode == nullptr) ok = false;

        if(ok)
        {
            Node* m1 = this->maxNode;
            Node* m2 = heap2.maxNode;
            Node* l1 = m1->left;
            Node* l2 = m2->left;

            m1->left = l2;
            l2->right = m1;
            m2->left = l1;
            l1->right = m2;

            if(heap2.maxNode->value > this->maxNode->value) this->maxNode=heap2.maxNode;
        }
        return *this;
    }

    void extract(Node* node){
        if(this->head == nullptr){
            this->head = node;
        }

        Node* last = this->head->left;

        node->right = head;
        node->left = last;
        head->left = node;
        last->right = node;

        node->parent = nullptr;
    }
    void extractChildren(Node* child){
        Node* lastChild = child->left;
        Node* left = this->maxNode->left;

        lastChild->right = this->maxNode;
        child->left = left;
        maxNode->left = lastChild;
        left->right = child;

        child->parent->child = nullptr;
    }

    void link(Node* node, vector<Node*>& linkV){
        int d;
        Node* next = node->right;
        d = node->degree;
        while(linkV[d] != nullptr){
            Node* n2 = linkV[d];
            if(node->value < n2->value) {
                Node *aux = node;
                node = n2;
                n2 = aux;
            }
            this->comb(n2, node);
            linkV[d] = nullptr;
            d = d + 1;
        }
        linkV[d] = node;
        if(!next->seen){
            next->seen = true;
            this->link(next, linkV);
            next->seen = false;
        }
    }

    void consolidate(){
        int maxL = ceil(log2(this->size))+1;
        vector<Node*> linkV(maxL);
        for(int i=0; i<maxL; i++){
            linkV.push_back(nullptr);
        }

        this->maxNode->seen = true;
        this->link(this->maxNode, linkV);
        this->maxNode->seen = false;

        this->maxNode = nullptr;
        this->head = nullptr;
        for(int i=0; i<linkV.size(); i++){
            if(linkV[i] != nullptr){
                this->extract(linkV[i]);
                if(this->maxNode == nullptr || linkV[i]->value > this->maxNode->value){
                    this->maxNode = linkV[i];
                }
            }
        }
    }
    void comb(Node* n2, Node* n1){
        Node* left = n2->left;
        Node* right = n2->right;
        left->right = right;
        right->left = left;

        if(n1->child != nullptr){
            Node* child = n1->child;
            Node* last = child->left;
            n2->right = child;
            n2->left = last;
            last->right = n2;
            child->left = n2;
            n2->parent = n1;
        } else {
            n1->child = n2;
            n2->parent = n1;
            n2->left = n2;
            n2->right = n2;
        }
        n1->degree++;
        n2->marked = false;
    }
    int extractMax(){
        Node*  n = this->maxNode;
        if( n != nullptr){
            if( n->child != nullptr) {
                this->extractChildren( n->child);
            }
            this->removeMax();
            if( n ==  n->right){
                this->maxNode = nullptr;
            }
            else {
                this->maxNode = n->right;
                this->consolidate();
            }
            this->size--;
        }
        return  n->value;
    }

    void removeMax(){
        Node* left = this->maxNode->left;
        Node* right = this->maxNode->right;
        left->right = right;
        right->left = left;
    }

    FibonacciHeap& operator=(const FibonacciHeap& fh){
        if(this!=&fh){
            if(fh.maxNode != nullptr){
                this->maxNode = fh.maxNode;
            }
            this->size = fh.size;
        }
        return *this;
    };
};

int main()
{

    ifstream f("mergeheap.in");
    ofstream g("mergeheap.out");
    int N, Q, a, b, c;
    vector<FibonacciHeap*> heaps;
    f>>N; f>>Q;
    for(int i=0; i<=N; i++){
        heaps.push_back(new FibonacciHeap);
    }

    while(Q>0){
        f>>a;
        if(a == 2) f>>b;
        else f>>b>>c;
        switch(a){
            case 1: {
                heaps[b]->insert(c);
                break;
            }
            case 2: {
                g<<heaps[b]->extractMax()<<endl;
                break;
            }
            case 3: {
                *heaps[b] = heaps[b]->heapUnion(*heaps[c]);
                heaps[c] = new FibonacciHeap();
                break;
            }
            default: {
                break;
            }
        }
        Q--;
    }
    f.close();
    g.close();


    return 0;
}