//
// Created by Yuriko on 2/19/2025.
//

#include "structs.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

const int trie_child_len = 26;

struct TrieNode *initialize_trie_node() {
    struct TrieNode *root = malloc(sizeof(struct TrieNode));
    root->child = (struct TrieNode **) malloc(trie_child_len * sizeof(struct TrieNode *));
    root->is_end_of_word = false;
    return root;
}

bool is_valid_trie_input(char *word) {
    for (char *character = word; *character != '\0'; character++) {
        int curr_char = *character - 'a';
        if (curr_char < 0 || curr_char >= trie_child_len) {
            return false;
        }
    }
    return true;
}

void add_to_trie_node(struct TrieNode *root, char *word) {
    struct TrieNode *node = root;
    char *character = word;

    if (!is_valid_trie_input(word)) {
        return;
    }

    // printf("added: %s\n", word);

    while (*character != '\0') {
        int curr_char = *character - 'a';
        if (node->child[curr_char] == NULL) {
            node->child[curr_char] = initialize_trie_node();
        }

        node = node->child[curr_char];
        character += 1;
    }
    node->is_end_of_word = true;
}

struct TrieNode *get_trie_from_word(struct TrieNode *root, char *word) {
    struct TrieNode *node = root;
    char *character = word;
    while (*character != '\0') {
        int char_idx = *character - 'a';
        /* Invalid input character */
        if (char_idx < 0 || char_idx >= trie_child_len) {
            return NULL;
        }

        /* Next character is not in dictionary */
        if (node->child[char_idx] == NULL) {
            return NULL;
        }

        node = node->child[char_idx];
        character += 1;
    }

    return node;
}

char **get_closest_result(struct TrieNode *root, int max_result) {
    struct TrieNode *node = root;
    char *result = malloc(50 * sizeof(char));
    char **result_arr = malloc(sizeof(char *) * 2);
    int result_i = 0;

    while (!node->is_end_of_word) {
        int i;
        for (i = 0; i < trie_child_len; ++i) {
            if (node->child[i] != NULL) {
                result[result_i] = 'a' + i;
                result_i++;
                node = node->child[i];
                break;
            }
        }
    }

    result[result_i] = '\0';
    result_arr[0] = result;
    result_arr[1] = NULL;
    return result_arr;
}

char **_get_closest_result(struct TrieNode *root, int max_result) {
    // struct TrieNode *node = root;
    // char *result = malloc(50 * sizeof(char));
}
