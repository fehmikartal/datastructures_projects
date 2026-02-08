#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[50];
    int start_year;
    int end_year;
    int income;
} VizierRecord;

typedef struct Node {
    VizierRecord data;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    int height;
} Node;

int height (Node *N) {
    if (N == NULL) {
        return 0;
    }

    return N->height;
}

int Max(int a, int b) {
    if (a>b) {
        return a;
    }
    else {
        return b;
    }
}

Node* newNode (VizierRecord data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->height = 1;

    return new_node;
}

int getBalance(Node *N) {
    if (N == NULL) {
        return 0;
    }

    return (height(N->left) - height(N->right)); 
}

Node* rightRotate (Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    x->parent = y->parent;
    y->parent = x;
    if (T2 != NULL) {
        T2->parent = y;
    }

    y->height = Max(height(y->left), height(y->right)) + 1;
    x->height = Max(height(x->left), height(x->right)) + 1;

    return x;
}

Node* leftRotate(Node* y) {
    Node *x = y->right;
    Node *T2 = x->left;

    x->left = y;
    y->right = T2;

    x->parent = y->parent;
    y->parent = x;
    if (T2 != NULL) {
        T2->parent = y;
    }

    y->height = Max(height(y->left), height(y->right)) + 1;
    x->height = Max(height(x->left), height(x->right)) + 1;

    return x;
}

Node* insertVezier(Node* node, VizierRecord data) { // input olarak gelen node=Root zaten.
    if (node == NULL) {
        return newNode(data);
    }

    if (data.start_year < node->data.start_year) {      // data pointer değil, node pointer. ->/. farkına önem ver.
        node->left = insertVezier(node->left, data);
        node->left->parent = node;
    } else {
        node->right = insertVezier(node->right, data);
        node->right->parent = node;
    }

    node->height = Max(height(node->left), height(node->right)) + 1;

    int balance = getBalance(node);

    if (balance > 1 && data.start_year < node->left->data.start_year) {
        return rightRotate(node);
    } else if (balance < -1 && data.start_year > node->right->data.start_year) {
        return leftRotate(node);
    } else if (balance > 1 && data.start_year > node->left->data.start_year) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    } else if(balance < -1 && data.start_year < node->right->data.start_year) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;    // değişikliğin olmadığı durum.


}

Node* findNode(Node* root, int year) {
    if (root == NULL) {
        return NULL;
    }

    if (year == root->data.start_year) {
        return root;
    }
    if (year < root->data.start_year) {
        return findNode(root->left, year);  // solda ara ve sonucu döndür (returnle)
    } else {
        return findNode(root->right, year);
    }
}

Node* successor(Node* node) {
    if (node == NULL) {
        return NULL;
    } 

    // DURUM A:
    if (node->right != NULL) {
        Node* current = node->right;
        while (current->left != NULL) {
            current = current->left;
        }
        return current;
    }

    // DURUM B:
    Node* p = node->parent;

    while (p != NULL && p->right == node) {
        node = p;
        p = p->parent;
    } 

    return p;
}

Node* predecessor(Node* node) {
    if (node == NULL) {
        return NULL;
    }

    // DURUM A:
    if (node->left != NULL) {
        Node* current = node->left;
        while (current->right != NULL) {
            current = current->right;
        }
        return current;
    }

    // DURUM B:
    Node* p = node->parent;

    while (p != NULL && p->left == node) {
        node = p;
        p = p->parent;
    }

    return p;
}

void compareVizier(Node* root, int target_year, int window) {
    Node* target= findNode(root, target_year);

    if (target == NULL) {
        printf("Yok böyle biri.\n");
        return;
    }

    printf("--- QUERY: %d (%s) ---\n", target->data.start_year, target->data.name);
    printf("[ TARGET ]\n");
    printf("    > %s (%d Akce)\n", target->data.name, target->data.income);
    printf("[ NEIGHBOR COMPARISON (Window: %d) ]\n", window);

    Node* current = target;

    for (int i = 0; i < window; i++) {
        current = predecessor(current);

        if(current == NULL) {
            break;
        }

        printf("    (<<) PREDECESSOR: %s (%d Akce)<\n", current->data.name, current->data.income);

    }

    current = target;

    for (int i = 0; i < window; i++) {
        current = successor(current);

        if (current == NULL) {
            break;
        }

        printf("    (>>) SUCCESSOR: %s (%d Akce)\n", current->data.name, current->data.income);
    }

}

int main() {

    Node *root = NULL;

    static VizierRecord const history[] = {
        {"Lutfi Pasa", 1539, 1541, 155000},
        {"Mahmud Pasa", 1456, 1468, 4000000}, // RAKİP
        {"Piri Mehmed", 1518, 1523, 145000},
        {"Candarli Ali", 1387, 1406, 82000},
        {"Sokollu Mehmed", 1565, 1579, 2000000},
        {"Pargali Ibrahim", 1523, 1536, 3000000}, // BİZİM ADAM (1523)
        {"Koca Sinan", 1580, 1582, 210000},
        {"Alaeddin Pasa", 1320, 1331, 50000},
        {"Kemankes Mustafa", 1638, 1644, 230000},
        {"Gedik Ahmed", 1474, 1477, 115000},
        {"Koprulu Mehmed", 1656, 1661, 260000},
        {"Ayas Mehmed", 1536, 1539, 160000},
        {"Kuyucu Murad", 1606, 1611, 220000},
        {"Candarli Halil", 1364, 1387, 75000},
        {"Rustem Pasa", 1544, 1553, 2800000},
        {"Merzifonlu Kara", 1676, 1683, 290000},
        {"Semiz Ali", 1561, 1565, 170000},
        {"Ferhad Pasa", 1591, 1592, 190000},
        {"Nevsehirli Damat", 1718, 1730, 240000},
        {"Koprulu Fazil", 1661, 1676, 275000}
    };

    int n = sizeof(history) / sizeof(history[0]);

    for (int i = 0; i < n; i++) {
        root = insertVezier(root, history[i]);
    }

    compareVizier(root, 1523, 1);

    printf("TEST 2 ///\n");

    compareVizier(root, 1523, 2);


    return 0;
}