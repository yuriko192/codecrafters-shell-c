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

    printf("added: %s\n", word);

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

struct AutocompleteNode {
    struct TrieNode *trie_node;
    int node_depth;
    char *word;
};

void free_autocomplete_node(struct AutocompleteNode *node) {
    free(node->word);
    free(node);
}

void append_autocomplete_trie_child_to_list(struct SingleLinkedList *list, const char *word,
                                            const struct TrieNode *trie_node, int node_depth) {
    for (int i = 0; i < trie_child_len; i++) {
        if (trie_node->child[i] == NULL) {
            continue;
        }
        char *new_word = malloc((node_depth + 1) * sizeof(char));
        strcpy(new_word, word);
        new_word[node_depth] = 'a' + i;
        new_word[node_depth + 1] = '\0';

        struct AutocompleteNode *autocomplete_node = malloc(sizeof(struct AutocompleteNode));
        autocomplete_node->trie_node = trie_node->child[i];
        autocomplete_node->node_depth = node_depth;
        autocomplete_node->word = new_word;
        enqueue_single_linked_list(list, autocomplete_node);
    }
}

char **get_closest_result(struct TrieNode *root, int max_result) {
    struct TrieNode *node = root;
    char **result_arr = malloc(sizeof(char *) * (max_result + 1));
    int result_i = 0;

    struct SingleLinkedList *list = initialize_single_linked_list();
    append_autocomplete_trie_child_to_list(list, "", root, 0);

    while (list->length != 0) {
        struct AutocompleteNode *node = (struct AutocompleteNode *) dequeue_single_linked_list(list);

        char *current_word = strdup(node->word);
        int i = node->node_depth;
        struct TrieNode *trie_node = node->trie_node;
        free_autocomplete_node(node);

        if (trie_node->is_end_of_word) {
            result_arr[result_i] = strdup(current_word);
            result_i++;

            if (result_i == max_result) {
                free(current_word);
                break;
            }
        }

        append_autocomplete_trie_child_to_list(list, current_word,trie_node, i + 1);
        free(current_word);
    }

    result_arr[result_i] = NULL;
    free_single_linked_list(list);
    return result_arr;
}

char **_get_closest_result(struct TrieNode *root, int max_result) {
    // struct TrieNode *node = root;
    // char *result = malloc(50 * sizeof(char));
}

struct LinkedListNode *initialize_linked_list_node(void *value) {
    struct LinkedListNode *node = malloc(sizeof(struct LinkedListNode));
    node->value = value;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

struct SingleLinkedList *initialize_single_linked_list() {
    struct SingleLinkedList *list = malloc(sizeof(struct SingleLinkedList));
    list->top = NULL;
    list->bottom = NULL;
    list->length = 0;
    return list;
}

void enqueue_single_linked_list(struct SingleLinkedList *list, void *value) {
    struct LinkedListNode *node = initialize_linked_list_node(value);

    if (list->top == NULL) {
        list->top = node;
    } else {
        list->bottom->next = node;
        node->prev = list->bottom;
    }

    list->bottom = node;
    list->length++;
}

void *dequeue_single_linked_list(struct SingleLinkedList *list) {
    struct LinkedListNode *node = list->top;

    if (node == NULL) {
        return NULL;
    }

    list->length--;
    void *value = node->value;

    list->top = node->next;
    if (list->top == NULL) {
        list->bottom = NULL;
    } else {
        list->top->prev = NULL;
    }

    free(node);
    return value;
}

void free_single_linked_list(struct SingleLinkedList *list) {
    while (list->length > 0) {
        dequeue_single_linked_list(list);
    }
    free(list);
}
