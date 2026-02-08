#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define REPEAT(n) for (uintmax_t _dcntr = (uintmax_t)n; _dcntr--;)
typedef struct ItemBody {
    char description[24];
    uint32_t price_in_usd;
    uint32_t days_until_expiration;
} Item;

static Item const example_items[] = {
    {"Premium Milk", 12, 1},
    {"Toast Breads", 6, 2},
    {"Feta Cheese", 8, 24},
    {"Standard Milk", 3, 8},
    {"Salty Crackers", 4, 52},
    {"Orange Juice", 7, 30},
    {"Chocolate Bar", 2, 44}
};

static uint32_t const n_example_items =
    sizeof(example_items) / sizeof(example_items[0]);

typedef struct BSTNodeBody {
    Item const* item;
    struct BSTNodeBody* left;
    struct BSTNodeBody* right;
} BSTNode;

static BSTNode* insertItem(
    BSTNode* node, Item const* const item
) {
    if (node == NULL) {
        BSTNode* newNode = (BSTNode* )malloc(sizeof(BSTNode));
        newNode->item = item;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    } else if ( node->item->price_in_usd > item->price_in_usd ) {
        node->left = insertItem(node->left, item);
    } else {
        node->right = insertItem(node->right, item);
    }

    return node;
    
}

static BSTNode* Search( // en küçük sağlayanı bulma
    BSTNode const *node, uint32_t const price_threshold
) {
    if (node == NULL) {
        return NULL;
    }

    uint32_t current_price = node->item->price_in_usd;

    if (current_price >= price_threshold) {
        BSTNode* res_left = Search(node->left, price_threshold);

        if (res_left != NULL) {
            return res_left;
        }

        return (BSTNode*)node;
    } else {
        return Search(node->right, price_threshold);
    }
}

static void Traverse( // eşikten küçükleri yazdırma
    BSTNode const *node, uint32_t const price_threshold
) {
    if (node == NULL) {
        return;
    }

    Traverse(node->left, price_threshold);

    if(node->item->price_in_usd > price_threshold) {
        printf("Item %-20s | Price $%u\n", node->item->description, node->item->price_in_usd);
    }

    Traverse(node->right, price_threshold);
}

static void printItemsAbovePriceAscending(
    BSTNode const * node, uint32_t const price_threshold
) {
    BSTNode *startNode = Search(node, price_threshold);

    if (startNode == NULL) {
        printf("Eşik değerinin ($%u) üstünde veya eşit ürün yok.\n", price_threshold);
        return;
    }

    Traverse(startNode, price_threshold);


}

static void printItemsExpiringSoon(
    BSTNode const *node, uint32_t const d
) {
    if (node == NULL) {
        return;
    }

    if (node->item->days_until_expiration < d) {
        printf("Item %-20s | Days: %u\n", node->item->description, node->item->days_until_expiration);
    }

    printItemsExpiringSoon(node->left, d);

    printItemsExpiringSoon(node->right, d);

}

int main(void) {
    Item const *item = &example_items[0];

    BSTNode* root = insertItem(root, item++);

    REPEAT(n_example_items - 1)
        insertItem(root, item++);

    printItemsAbovePriceAscending(root, 5);
    printItemsExpiringSoon(root, 30);
    return 0;
}