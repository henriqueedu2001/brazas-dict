#ifndef KDTREE_H
#define KDTREE_H

#include <stdlib.h>

#define EMBEDDING_SIZE 2
#define emb_cast (float[EMBEDDING_SIZE])

typedef struct node_cell {
    float *embedding;
    char *word;
    struct node_cell *root;
    struct node_cell *left;
    struct node_cell *right;
} node;

node *create_node(float *embedding, char *word, node *root, node *left, node *right);
node *create_empty_node();
node *insert_node(node *root, node *new_node);
node *build_kdtree(node **nodes, int n_nodes);
void destroy_kdtree(node *root);
node *search_node(node *root, float *embedding);
node *create_empty_node();
void print_node(node *single_node);

#endif // KDTREE_H