#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define HASH_SIZE 10000

typedef struct HashNode {
    char *key;
    int val;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    HashNode **hash_array;
    HashNode **items;
    int count;
} HashTable;

unsigned long murmurHash3(const char *str) {
    unsigned int h = 0x12345678;
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h % HASH_SIZE;
}

HashNode *create_node(const char *key) {
    HashNode *node = malloc(sizeof(HashNode));
    node->key = malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->val = 1;
    node->next = NULL;
    return node;
}

HashTable *create_hash_table(void) {
    HashTable *hashTable = malloc(sizeof(HashTable) * 1);
    hashTable->hash_array = malloc(sizeof(HashNode *) * HASH_SIZE);
    hashTable->items = malloc(sizeof(HashNode *) * HASH_SIZE); //todo:
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable->hash_array[i] = NULL;
        hashTable->items[i] = NULL;
    }
    hashTable->count = 0;
    return hashTable;
}

void insert(HashTable *hashTable, const char *key) {
    unsigned int hash = murmurHash3(key);
    HashNode *node = hashTable->hash_array[hash];
    if (node == NULL) {
        hashTable->items[hashTable->count] = create_node(key);
        hashTable->hash_array[hash] = hashTable->items[hashTable->count];
        hashTable->count++;
        return;
    }
    HashNode *prev_node;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            node->val++;
            return;
        }
        prev_node = node;
        node = node->next;
    }
    prev_node->next = create_node(key);
}

int get(HashTable *hashTable, const char *key) {
    unsigned int hash = murmurHash3(key);
    HashNode *node = hashTable->hash_array[hash];
    if (node == NULL) {
        return 0;
    }
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->val;
        }
        node = node->next;
    }
    return 0;
}

int cmp(const void *a, const void *b) {
    HashNode *e1 = *(HashNode **) a;
    HashNode *e2 = *(HashNode **) b;
    return e2->val - e1->val;
}

char **find_top_k(char *strings, const int32_t k) {
    HashTable *hashTable = create_hash_table();
    while (strings != NULL) {
        insert(hashTable, strings);
        strings = strtok(NULL, " \n");
    }
    int num_entries = hashTable->count;
    HashNode *frequency_table[hashTable->count];
    memcpy(frequency_table, hashTable->items, num_entries * sizeof(HashNode *));
    qsort(frequency_table, num_entries, sizeof(HashNode *), cmp);

    char **top_k = malloc((k + 1) * sizeof(char *));
    top_k[k] = NULL;
    for (int i = 0; i < k; ++i) {
        top_k[i] = frequency_table[i]->key;
    }
    return top_k;
}

void remove_punct_and_make_lower_case(char *p) {
    char *src = p, *dst = p;
    while (*src) {
        if (ispunct((unsigned char) *src)) {
            src++;
        } else if (isupper((unsigned char) *src)) {
            *dst++ = tolower((unsigned char) *src);
            src++;
        } else if (src == dst) {
            src++;
            dst++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = 0;
}

char *read_file(const char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = malloc((file_size + 1) * sizeof(char));
    fread(content, sizeof(char), file_size, file);
    content[file_size] = '\0';
    fclose(file);
    return content;
}

char **find_frequent_words(const char *path, int32_t n) {
    char *text = read_file(path);
    remove_punct_and_make_lower_case(text);
    char* words = strtok(text, " \n");
    return find_top_k(words, n);
}


int main() {
    int32_t n = 30;
    char **top_k = find_frequent_words("Data\\shakespeare.txt", n);
    for (int i = 0; i < n; i++) {
        printf("%s\n", top_k[i]);
    }
    return 0;
}