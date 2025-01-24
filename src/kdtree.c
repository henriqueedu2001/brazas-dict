#include <stdlib.h>
#include <stdio.h>
#include "kdtree.h"

/**
 * Creates a node
 * @param[in] embedding The embedding of the word
 * @param[in] word The string that represents thw word 
 * @param[in] root The root of the node, in the k-d tree
 * @param[in] left The left child of the node, in the k-d tree
 * @param[in] right The right child of the node, in the k-d tree
 * @return Returns a pointer to a new node
 */
node *create_node(float *embedding, char *word, node *root, node *left, node *right) {
    node *new_node = malloc(sizeof(node));

    if(new_node != NULL){
        new_node->embedding = embedding;
        new_node->word = word;
        new_node->root = root;
        new_node->left = left;
        new_node->right = right;
    }

    return new_node;
}

/**
 * Creates a node
 * @return Returns a pointer to a new node, with NULL pointers for all properties
 */
node *create_empty_node() {
    node *new_node = malloc(sizeof(node));

    if(new_node != NULL){
        new_node->embedding = NULL;
        new_node->word = NULL;
        new_node->root = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
    }

    return new_node;
}

/**
 * Recursively inserts a node in the k-d tree
 * @param root The root for insertion
 * @param new_node The new node of the tree
 * @param depth The depth of the current root
 * @return Returns a pointer of the root
 */
node *insert_node_rec(node *root, node *new_node, int depth) {
    int dim = depth % EMBEDDING_SIZE;

    if(root == NULL) {
        new_node->root = root;
        root = new_node;
        root->left = root->right = NULL;

        return root;
    }

    if(new_node->embedding[dim] > root->embedding[dim]) {
        root->right = insert_node_rec(root->right, new_node, depth + 1);
    } else {
        root->left = insert_node_rec(root->left, new_node, depth + 1);
    }

    return root;
}

/**
 * Inserts a node in the k-d tree
 * @param root The root for insertion
 * @param new_node The new node of the tree
 * @param depth The depth of the current root
 * @return Returns a pointer of the root
 */
node *insert_node(node *root, node *new_node) {
    return insert_node_rec(root, new_node, 0);
}

/**
 * Prints the information of a given node
 * @param[in] single_node The pointer to the node
 */
void print_node(node *single_node) {
    if(single_node == NULL) {
        printf("empty node\n");
        return;
    }

    if(single_node->word == NULL){
        printf("empty node\n");
        return;
    }
    
    if(single_node->word != NULL)
        printf("word: %s\n", single_node->word);
    
    if(single_node->embedding != NULL) {
        int i;

        printf("embedding: ");
        for(i = 0; i < EMBEDDING_SIZE; i++)
            printf("%e ", single_node->embedding[i]);
    }

    printf("\n");
}