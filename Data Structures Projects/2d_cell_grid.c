#include <stdio.h>
#include <stdlib.h>     // malloc
#include <math.h>       // abs
#include <stdbool.h>    // bool

typedef struct Node {
    int value;
    struct Node *up, *down, *left, *right;
} Node;

typedef struct {
    Node *head;
    size_t rows, cols;      // size_t: yazdığın değerin (örn. int) max yer 
} CellGrid;                 // kaplayanını alır. "unsigned", yani sadece
                            // 0 ve + değer alır.

CellGrid *gridcreate(int rows, int cols, int initvalue);
void gridupdate(CellGrid *grid, int col, int row, int newvalue);
void deleterow(CellGrid *grid, int rowindex);
int sumgrid(CellGrid *grid);

int main(){
    CellGrid* grid = gridcreate(5,5,3);
    gridupdate(grid,1,2,6);
    printf("%d\n",sumgrid(grid));
    deleterow(grid,1);
    printf("%d",sumgrid(grid));
    return 0;
}

CellGrid *gridcreate(int rows, int cols, int initvalue) {
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    CellGrid *grid = (CellGrid *)malloc(sizeof(CellGrid));
    grid->rows = rows;
    grid->cols = cols;

    Node ***node_map = (Node ***)malloc(rows * sizeof(Node**));

    for (int i = 0; i < rows; i++) {
        node_map[i] = (Node *)malloc(cols * sizeof(Node*));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Node *newnode = (Node *)malloc(sizeof(Node));

            newnode->value = initvalue;
            newnode->up = NULL;
            newnode->down = NULL;
            newnode->left = NULL;
            newnode->right = NULL;

            node_map[i][j] = newnode;
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            if (!(j-1 < 0)) {
                node_map[i][j]->left = node_map[i][j-1];
            }
            if (!(j+1 >= cols)) {
                node_map[i][j]->right = node_map[i][j+1];
            }
            if (!(i-1 < 0)){
                node_map[i][j]->up = node_map[i-1][j];
            }
            if(!(i+1 >= cols)) {
                node_map[i][j]->down = node_map[i+1][j];
            }
        }
    }

    grid->head = node_map[rows/2][cols/2];

    for (int i = 0; i < rows; i++) {
        free(node_map[i]);
    }
    free(node_map);

    return grid;
}

void gridupdate(CellGrid *grid, int col, int row, int newvalue) {
    Node* target = (Node *)malloc(sizeof(Node));

    target = grid->head;

    for (int i = 0; i<abs(col); i++) {
        if (col < 0) {
            target = target->left;
        } else {
            target = target->right;
        }

        if(target == NULL) {
            return;
        }
    }

    for (int j = 0; j<abs(row); j++) {
        if (row < 0) {
            target = target->down;
        } else {
            target = target->up;
        }

        if (target == NULL) {
            return;
        }
    }

    target->value = newvalue;
}

void deleterow(CellGrid *grid, int rowindex) {
    if (rowindex < 0 || rowindex >= grid->rows) {
        return;
    }

    bool isheadchanged = false;
    if(rowindex == grid->rows / 2){     // head'in kendisi ise
        if (grid->rows % 2 == 0) {      // row sayısı çift ise
            grid->head = grid->head->up;
        } else {
            grid->head = grid->head->down;
        }
        isheadchanged = true;
    }

    Node *target = grid->head;
    while (target->left != NULL) {
        target = target->left;
    }
    while (target->up != NULL) {
        target = target->up;
    }
    for (int i = 0; i < rowindex; i++) {
        target = target->down;
    }

    Node* temp;
    while (target != NULL) {
        if(target->up != NULL) {
            target->up->down = target->down;
        }
        if(target->down != NULL) {
            target->down->up = target->up;
        }

        temp = target;
        target = target->right;
        free(temp);
    }

    if(!isheadchanged) {
        if(grid->rows % 2 == 0 && rowindex > grid->rows/2){
            grid->head = grid->head->up;
        }
        if(grid->rows %2 == 1 && rowindex < grid->rows/2){
            grid->head = grid->head->down;
        }
    }

    grid->rows = grid->rows - 1;

}



int sumgrid(CellGrid *grid) {

    int total = 0;

    Node* target = grid->head;
    while(target->left != NULL) {
        target = target->left;
    }
    while (target->up != NULL) {
        target = target->up;
    }

    Node* current;

    for (int i = 0; i < grid->rows; i++) {
        current = target;
        for (int j = 0; j < grid->cols; j++) {
            total += current->value;
            current = current->right;
        }
        target = target->down;
    }

    return total;

    return 0;
}

