#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "kdtree.h"
#include <math.h>

node *insert_node_rec(node *root, node *new_node, int depth);
node *search_node_rec(node *root, float *embedding, int depth);
node *nn_search_rec(node *root, float *embedding, int depth);

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
 * Searches for node with a given embedding in the k-d tree
 * @param[in] root The root of the k-d tree or some of its branches
 * @param[in] embedding The embedding of the search
 * @return the queried node or a NULL pointer
 */
node *search_node(node *root, float *embedding) {
    return search_node_rec(root, embedding, 0);
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

node *closest(node *a, node *b, float *embedding) {
    if(a == NULL) return b;
    if(b == NULL) return a;
    // printf("%s %s\n", a->word, b->word);

    float dist_a = embeddings_sq_distance(a->embedding, embedding);
    float dist_b = embeddings_sq_distance(b->embedding, embedding);
    if(dist_a > dist_b) return b;
    
    return a;
}

/**
 * Recursively searches the node with the closest embedding to a given target embedding
 * @param[in] root The root or a branch of the k-d tree
 * @param[in] embedding The target embedding of the search
 * @param[in] depth The depth of the actual node in the k-d tree
 * @return The closest node
 */
node *nn_search_rec(node *root, float *embedding, int depth) {
    printf(">ENTER %d\n", depth);
    if(root == NULL) return NULL;

    node *next_branch, *other_branch;
    int dim = depth % EMBEDDING_SIZE;

    if(root->embedding[dim] > embedding[dim]) {
        next_branch = root->left;
        other_branch = root->right;
    } else {
        next_branch = root->right;
        other_branch = root->left;
    }

    node *temp = nn_search_rec(next_branch, embedding, depth + 1);
    node *best = closest(root, temp, embedding);

    float squared_dist = embeddings_sq_distance(embedding, best->embedding);
    float axis_dist = embedding[dim] - root->embedding[dim];

    if(squared_dist >= axis_dist*axis_dist) {
        temp = nn_search_rec(other_branch, embedding, depth + 1);
        best = closest(root, temp, embedding);
    }

    return best;
}

/**
 * Searches the node with the closest embedding to a given target embedding
 * @param[in] root The root or a branch of the k-d tree
 * @param[in] embedding The target embedding of the search
 * @return The closest node
 */
node *nn_search(node *root, float *embedding) {
    return nn_search_rec(root, embedding, 0);
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
 * Evaluates the square of the distance between two embeddings, in R^n norm
 * @param[in] a
 * @param[in] b
 * @return The distance d(a,b) = sum_{i=0}^{n} (a[i] - b[i])^2
 */
float embeddings_sq_distance(float *a, float *b) {
    float dist = 0;
    int i = 0;

    for(i = 0; i < EMBEDDING_SIZE; i++)
        dist += (a[i] - b[i])*(a[i] - b[i]);

    return dist;
}

/**
 * Evaluates the distance between two embeddings, in R^n norm
 * @param[in] a
 * @param[in] b
 * @return The distance sqrt(d(a,b)) = sqrt(sum_{i=0}^{n} (a[i] - b[i])^2)
 */
float embeddings_distance(float *a, float *b) {
    return sqrt(embeddings_sq_distance(a, b));
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