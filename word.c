#include "word.h" 
#include "code.h" 
#include <stdio.h> 
#include <stdlib.h> 

Word *word_create(uint8_t *syms, uint32_t len) { 
    Word *word = (Word *) calloc(1, sizeof(Word)); 
    if (word) { 
        word->syms = (uint8_t *) calloc(len, sizeof(uint8_t)); 
        word->len = len; 
        if (word->syms) { 
            for (uint8_t i = 0; i < len; i++) { 
                word->syms[i] = syms[i]; 
            }
        } else { 
            free(word); 
            word = NULL; 
            return word; 
        }

    } else { 
        word = NULL; 
    }
    return word; 
}

Word *word_append_sym(Word *w,uint8_t sym) { 
    if (w->len) { 
        Word *new_word = (Word *) malloc(sizeof(Word)); 
        if (!new_word) { 
            return NULL; 
        }
        new_word->syms = (uint8_t *) calloc(w->len + 1, sizeof(uint8_t)); 
        if (!new_word->syms) { 
            free(new_word); 
            return NULL; 
        }
        
        for (uint32_t i = 0; i < w->len; i++) { 
            new_word->syms[i] = w->syms[i]; 
        }
        
        new_word->syms[w->len] = sym; 
        new_word->len = w->len + 1; 
        return new_word; 
    } else { 
        return word_create(&sym, EMPTY_CODE); 
    }
}

void word_delete(Word *w) { 
    if (w) { 
        if (w->syms) { 
            free(w->syms); 
            w->syms = NULL; 
        } else { 
            fprintf(stderr, "Error:Null Pointer\n"); 
            exit(EXIT_FAILURE); 
        }
        free(w); 
        w = NULL; 
    } else { 
        fprintf(stderr, "Error:Null Pointer\n"); 
        exit(EXIT_FAILURE); 
    }
}

WordTable *wt_create(void) { 
    WordTable *new_wt = (WordTable *) calloc(MAX_CODE, sizeof(WordTable)); 
    if (new_wt) { 
        for (uint16_t i = 0; i < MAX_CODE; i += 1) { 
            new_wt[i] = NULL; 
        }
        new_wt[EMPTY_CODE] = word_create(NULL, 0); 
        if (new_wt[EMPTY_CODE] == NULL) { 
            for (uint16_t i = 0; i < MAX_CODE; i += 1) { 
                word_delete(new_wt[i]); 
            }
            free(new_wt); 
            return NULL; 
        }
    } else { 
        new_wt = NULL; 
    }
    return new_wt; 
}

void wt_reset(WordTable *wt) { 
    if (wt) { 
        for (int i = START_CODE; i < MAX_CODE; i++) { 
            if (wt[i]) { 
                word_delete(wt[i]); 
                wt[i] = NULL; 
            }
        }
    }
}

void wt_delete(WordTable *wt) { 
    if (wt) { 
        for (int i = EMPTY_CODE; i < MAX_CODE; i += 1) { 
            if (wt[i]) { 
                word_delete(wt[i]); 
                wt[i] = NULL; 
            }
        }
        free(wt); 
    }
}
