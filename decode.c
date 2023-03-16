#include "code.h" 
#include "io.h" 
#include "word.h"

#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/stat.h> 
#include <getopt.h>
#include <unistd.h> 

#define OPTIONS "hvi:o:" 

void usage(void) { 
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Decompresses files with the LZ78 decompression algorithm.\n");
    fprintf(stderr, "   Used with files compressed with the corresponding encoder.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./decode [-vh] [-i input] [-o output]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -v          Display decompression statistics\n");
    fprintf(stderr, "   -i input    Specify input to decompress (stdin by default)\n");
    fprintf(stderr, "   -o output   Specify output of decompressed input (stdout by default)\n");
    fprintf(stderr, "   -h          Display program usage\n");
}

uint64_t bits_to_byte(uint64_t bits) { 
    return !(bits % 8) ? (bits / 8) : (bits / 8) + 1;
}

int bit_length(uint16_t code) { 
    int bitlen = 0;

    while (code > 0) { 
        bitlen += 1;
        code >>= 1;
    }
    return bitlen;
}

void verb(void) {
    double saving = 1.0 - ((double) bits_to_byte(total_bits) / (double) total_syms); 
    fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", bits_to_byte(total_bits));
    fprintf(stderr, "Uncompressed file size: %" PRIu64 " bytes\n",total_syms); 
    fprintf(stderr, "Space saving: %.2f%%\n", 100 * saving);
}

int main(int argc, char **argv) {
    int options = 0;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    bool verbose = false;
    while ((options = getopt(argc, argv, OPTIONS)) != -1) {
        switch (options) {
        case 'i':
            if ((infile = open(optarg, O_RDONLY)) == -1) { 
                fprintf(stderr, "%s : failed to open the input file.\n", optarg);
                return EXIT_FAILURE; 
            }
            break;
        case 'o': 
            if ((outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
                fprintf(stderr, "%s : failed to open output file or file dne.\n", optarg); 
                close(infile); 
                return EXIT_FAILURE;
            }
            break; 
        case 'v': 
            verbose = true; 
            break; 
        case 'h': 
            usage(); 
            return EXIT_SUCCESS;
        default:
            usage(); 
            return EXIT_FAILURE;
        }
    }
    FileHeader header = { 0, 0 };
    read_header(infile, &header);
    if (header.magic != MAGIC) { 
        fprintf(stderr, "Bad magic number!\n"); 
        close(infile);
        close(outfile);
        return EXIT_FAILURE;
    }
    fchmod(outfile, header.protection); 
    WordTable *table = wt_create(); 
    uint8_t curr_sym = 0; 
    uint16_t curr_code = 0; 
    uint16_t next_code = START_CODE; 
    while ((read_pair(infile, &curr_code, &curr_sym, bit_length(next_code)))) { 
        table[next_code] = word_append_sym(table[curr_code], curr_sym); 
        write_word(outfile, table[next_code]); 
        next_code += 1; 
        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    flush_words(outfile); 
    wt_delete(table); 
    close(infile);
    close(outfile);
    if (verbose) { 
        verb(); 
    }
    return EXIT_SUCCESS;
}
