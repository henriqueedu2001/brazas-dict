#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "kdtree.h"

node *insert_node(node *root, node *new_node);
node *search_node_rec(node *root, float *embedding, int depth);
bool equal_embeddings(float *a, float *b);

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
 * @param[in] root The root for insertion
 * @param[in] new_node The new node of the tree
 * @param[in] depth The depth of the current root
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
 * @param[in] root The root for insertion
 * @param[in] new_node The new node of the tree
 * @param[in] depth The depth of the current root
 * @return Returns a pointer of the root
 */
node *insert_node(node *root, node *new_node) {
    return insert_node_rec(root, new_node, 0);
}

/**
 * Builds the k-d tree with a given array of nodes
 * @param[in] nodes The nodes for the k-d tree
 * @param[in] n_nodes The total number of nodes
 */
node *build_kdtree(node **nodes, int n_nodes) {
    int i;
    for(i = 1; i < n_nodes; i++){
        insert_node(nodes[0], nodes[i]);
    }

    return NULL;
}

/**
 * Destroys the k-d tree or some of its branches
 * @param[in, out] root The root of the tree or a subtree
 */
void destroy_kdtree(node *root) {
    if(root == NULL) return;
    destroy_kdtree(root->left);
    destroy_kdtree(root->right);
    free(root);
}

/**
 * Recursively searches for node with a given embedding in the k-d tree
 * @param[in] root The root of the k-d tree or some of its branches
 * @param[in] embedding The embedding of the search
 * @param[in] depth The depth of the actual node in the k-d tree
 * @return the queried node or a NULL pointer
 */
node *search_node_rec(node *root, float *embedding, int depth) {
    if(root == NULL) return NULL;
    
    if(equal_embeddings(root->embedding, embedding))
        return root;    

    int dim = depth % EMBEDDING_SIZE;
    if(root->embedding[dim] < embedding[dim])
        return search_node_rec(root->right, embedding, depth + 1);
    
    if(root->embedding[dim] > embedding[dim])
        return search_node_rec(root->left, embedding, depth + 1);
    
    return NULL;
}

/**
 * Searches for node with a given embedding in the k-d tree
 * @param[in] root The root of the k-d tree or some of its branches
 * @param[in] embedding The embedding of the search
 * @return the queried node or a NULL pointer
 */
node *search_node(node *root, float *embedding) {
    return search_node_rec(root, embedding, 0);
}

/**
 * Verifies if two embeddings are equal to each other
 * @param[in] a first embedding
 * @param[in] b second embedding
 * @return Returns true if they are equal and false otherwise
 */
bool equal_embeddings(float *a, float *b) {
    int i;
    for(i = 0; i < EMBEDDING_SIZE; i++) 
        if(a[i] != b[i]) return false;

    return true;
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