#include "io.h" 

#include "code.h" 
#include "endian.h" 

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 

#define BITS 8 

uint64_t total_syms = 0; 
uint64_t total_bits = 0; 

static uint8_t sym_buf[BLOCK] = { 0 }; 
static int sym_index = 0; 

static uint8_t bit_buf[BLOCK] = { 0 }; 
static int bitindex = 0; 

int read_bytes(int infile, uint8_t *buf,
    int to_read) { 
    int reads = -1, total = 0; 
    if (infile) { 
        while ( (total != to_read) && (reads!= 0)) { 
            if ((reads = read(infile, buf, to_read)) == -1) { 
                fprintf(stderr, "failed to read bytes in file.\n"); 
                exit(EXIT_FAILURE); 
            }
            total += reads; 
            buf += reads; 
        }
        return total; 
    } else {
        fprintf(stderr, "Error: Missing the input file\n"); 
        exit(EXIT_FAILURE); 
    }
}

int write_bytes(int outfile, uint8_t *buf, int to_write) { 
    int writes = -1, total = 0; 
    if (outfile) { 
        while ((total != to_write) && (writes!= 0)) { 
            if ((writes = write(outfile, buf, to_write)) == -1) { 
                fprintf(stderr, "failed to write bytes to file.\n"); 
                exit(EXIT_FAILURE); 
            }
            total += writes; 
            buf += writes; 
        }
        return total; 
    } else { 
        fprintf(stderr, "Error: Missing the output file\n"); 
        exit(EXIT_FAILURE); 
    }
}

void read_header(int infile, FileHeader *header) { 
    if (infile) { 
        if (big_endian()) { 
            if (header) { 
                header->magic = swap32(header->magic); 
                header->protection = swap16(header->protection); 
            } else { 
                fprintf(stderr, "Error: NULL pointer\n"); 
                exit(EXIT_FAILURE); 
            }
        }
        read_bytes(infile, (uint8_t *) header, sizeof(FileHeader)); 
        total_bits += (sizeof(FileHeader) * BITS); 
        return; 
    } else { 
        fprintf(stderr, "Error: Missing the inputfile\n"); 
        exit(EXIT_FAILURE); 
    }
}

void write_header(int outfile, FileHeader *header) { 
    if (outfile) { 
        if (big_endian()) { 
            if (header) { 
                header->magic = swap32(header->magic); 
                header->protection = swap16(header->protection); 
            } else { 
                fprintf(stderr, "Error: NULL pointer\n"); 
                exit(EXIT_FAILURE); 
            }
        }
        write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader)); 
        total_bits += (sizeof(FileHeader) * BITS); 
        return; 
    } else { 
        fprintf(stderr, "Error: Missing the output file\n"); 
        exit(EXIT_FAILURE); 
    }
}

bool read_sym( int infile, uint8_t *sym) { 
    static int end = -1; 
    if (infile) { 
        if (!sym_index) { 
            int reads = read_bytes(infile, sym_buf, BLOCK); 
            if (reads < BLOCK) { 
                end = reads + 1; 
            }
        }
        *sym = sym_buf[sym_index]; 
        sym_index = (sym_index + 1) % BLOCK; 
        if (sym_index != end) { 
            total_syms += 1; 
        }
        return sym_index == end ? false : true; 
    } else { 
        fprintf(stderr, "Error: Missing input file\n"); 
        exit(EXIT_FAILURE); 
    }
}

void write_pair(int outfile, uint16_t code, uint8_t sym,
    int bitlen) { 
    if (outfile) { 
        if (big_endian()) { 
            swap16(code); 
        }
        for (int bit = 0; bit < bitlen; bit++) { 
            if (bitindex == BITS * BLOCK) { 
                write_bytes(outfile, bit_buf, BLOCK); 
                memset(bit_buf, 0, BLOCK); 
                bitindex = 0; 
            }
            uint16_t get_code_bit = (code >> (bit % 16)) & 1; 
            if (get_code_bit) { 
                bit_buf[bitindex / 8] |= (1 << (bitindex % 8)); 
            }
            bitindex += 1; 
        }
        for (int bit = 0; bit < BITS; bit++) { 
            if (bitindex == BITS * BLOCK) { 
                write_bytes(outfile, bit_buf, BLOCK); 
                memset(bit_buf, 0, BLOCK); 
                bitindex = 0; 
            }
            uint8_t get_sym_bit = (sym >> (bit % 8)) & 1; 
            if (get_sym_bit) { 
                bit_buf[bitindex / 8] |= (1 << (bitindex % 8)); 
            }
            bitindex += 1; 
        }
        total_bits += (bitlen + BITS); 
    } else { 
        fprintf(stderr, "Error: Missing the outfile\n"); 
        exit(EXIT_FAILURE); 
    }
}

void flush_pairs(int outfile) { 
    if (outfile) { 
        if (write_bytes(outfile, bit_buf, (bitindex % 8) ? (bitindex / 8) : (bitindex / 8) + 1)) { 
            return; 
        } else { 
            fprintf(stderr, "Error: Unable to write\n"); 
            exit(EXIT_FAILURE); 
        }
    } else { 
        fprintf(stderr, "Error: Missing output file\n"); 
        exit(EXIT_FAILURE); 
    }
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    *code = 0, *sym = 0; 
    if (infile) { 
        for (int bit = 0; bit < bitlen; bit++) { 
            if (!bitindex) { 
                int val = read_bytes(infile, bit_buf, BLOCK); 
                if (val == 0) { 
                    fprintf(stderr, "Error: Unalbe to read bytes"); 
                    exit(EXIT_FAILURE); 
                }
            }
            *code |= ((bit_buf[bitindex / 8] >> (bitindex % 8)) & 1) << bit; 
            bitindex = (bitindex + 1) % (BITS * BLOCK); 
        }

        for (int bit = 0; bit < BITS; bit++) { 
            if (!bitindex) { 
                int val = read_bytes(infile, bit_buf, BLOCK); 
                if (val == 0) { 
                    fprintf(stderr, "Error: Unalbe to read bytes"); 
                    exit(EXIT_FAILURE); 
                }
            }
            *sym |= ((bit_buf[bitindex / 8] >> (bitindex % 8)) & 1) << bit; 
            bitindex = (bitindex + 1) % (BITS * BLOCK);
        }
        if (big_endian()) { 
            *code = swap16(*code); 
        }
        total_bits += (bitlen + BITS); 
        return *code != STOP_CODE ? true : false; 
    } else { 
        fprintf(stderr, "Error: Missing the input file\n"); 
        exit(EXIT_FAILURE); 
    }
}

void write_word(int outfile, Word *w) { 
    if (outfile) { 
        if (w) { 
            for (uint32_t i = 0; i < w->len; i++) { 
                if (sym_index == BLOCK) { 
                    int val = write_bytes(outfile, sym_buf, BLOCK); 
                    if (val == 0) { 
                        fprintf(stderr, "Error: Unable to write bytes"); 
                        exit(EXIT_FAILURE); 
                    }
                    sym_index = 0; 
                }
                sym_buf[sym_index] = w->syms[i]; 
                sym_index += 1; 
            }
            total_syms += w->len; 
            return; 
        } else { 
            fprintf(stderr, "Error: Null Pointer\n"); 
            exit(EXIT_FAILURE); 
        }
    } else { 
        fprintf(stderr, "Error: Missing the output file\n"); 
        exit(EXIT_FAILURE); 
    }
}

void flush_words(int outfile) { 
    if (outfile) {
        int val = write_bytes(outfile, sym_buf, sym_index); 
        if (val == 0) { 
            fprintf(stderr, "Error: Unabel to write in the file"); 
            exit(EXIT_FAILURE); 
        }
        return; 
    } else { 
        fprintf(stderr, "Error: Missing the output file\n"); 
        exit(EXIT_FAILURE); 
    }
}
