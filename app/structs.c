//
// Created by Yuriko on 2/19/2025.
//

#include "structs.h"

#include <stdlib.h>
#include <string.h>

constexpr int trie_child_len = 26;

struct TrieNode* initialize_trie_node() {
    struct TrieNode *root = malloc(sizeof(struct TrieNode));
    root->child = (struct TrieNode **)malloc(trie_child_len* sizeof(struct TrieNode*));
    root->is_end_of_word = false;
    return root;
}

void add_to_trie_node(struct TrieNode *root, char *word) {
    struct TrieNode *node = root;
    char *character = word;
    while (*character != '\0') {
        if (node->child[*character - 'a']==NULL) {
            node->child[*character - 'a'] = initialize_trie_node();
        }
        node = node->child[*character - 'a'];
        character+=1;
    }
    node->is_end_of_word = true;
}

struct TrieNode *get_trie_from_word(struct TrieNode *root, char *word) {
    struct TrieNode *node = root;
    char *character = word;
    while (*character != '\0') {
        if (node->child[*character - 'a']==NULL) {
            return node;
        }
        node = node->child[*character - 'a'];
        character+=1;
    }
    return node;
}

char ** get_closest_result(struct TrieNode *root, int max_result) {
    struct TrieNode *node = root;
    char *result = malloc(50 * sizeof(char));

}