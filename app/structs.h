//
// Created by Yuriko on 2/19/2025.
//

#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

struct TrieNode {
    struct TrieNode **child;
    bool is_end_of_word;
};

struct TrieNode *initialize_trie_node();

void add_to_trie_node(struct TrieNode *root, char *word);

struct TrieNode *get_trie_from_word(struct TrieNode *root, char *word);

char **get_closest_result(struct TrieNode *root, int max_result);

struct LinkedListNode {
    struct LinkedListNode *next;
    struct LinkedListNode *prev;
    void *value;
};

struct SingleLinkedList {
    struct LinkedListNode *top;
    struct LinkedListNode *bottom;
    int length;
};

struct LinkedListNode *initialize_linked_list_node(void *value);

struct SingleLinkedList *initialize_single_linked_list();

void enqueue_single_linked_list(struct SingleLinkedList *list, void *value);

void *dequeue_single_linked_list(struct SingleLinkedList *list);

void free_single_linked_list(struct SingleLinkedList *list) ;
#endif //STRUCTS_H
