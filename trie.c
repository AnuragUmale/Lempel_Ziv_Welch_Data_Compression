#include "trie.h" 
#include "code.h" 
#include <stdio.h> 
#include <stdlib.h> 

TrieNode *trie_node_create(uint16_t index) { 
    TrieNode *new_node = (TrieNode *) calloc(1, sizeof(TrieNode)); 
    if (new_node) { 
        new_node->code = index; 
        for (int i = 0; i < ALPHABET; i++) { 
            new_node->children[i] = NULL; 
        }
    } else { 
        new_node = NULL; 
    }
    return new_node; 
}

void trie_node_delete(TrieNode *n) { 
    if (n) { 
        free(n); 
        n = NULL; 
        return; 
    }
}

TrieNode *trie_create(void) { 
    return trie_node_create(EMPTY_CODE); 
}

void trie_delete(TrieNode *n) { 
    if (n) { 
        for (int i = 0; i < ALPHABET; i++) { 
            if (n->children[i] != NULL) { 
                trie_delete(n->children[i]); 
            }
        }
        free(n); 
        n = NULL; 
    }
    return; 
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) { 
    if (n->children[sym] != NULL) { 
        return n->children[sym]; 
    }
    return NULL; 
}

void trie_reset(TrieNode *root) { 
    for (int i = 0; i < ALPHABET; i++) { 
        if (root->children[i] != NULL) { 
            trie_delete(root->children[i]); 
            root->children[i] = NULL; 
        }
    }
    return; 
}
