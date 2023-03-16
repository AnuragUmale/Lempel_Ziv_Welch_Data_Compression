CC      = clang
CFLAGS 	= -Wall -Wpedantic -Wextra -Werror -Ofast


all: encode decode

encode: encode.o word.o trie.o io.o
	$(CC) -o $@ $^

decode: decode.o word.o trie.o io.o
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f encode decode *.o
	
format:
	clang-format -i -style=file *.[ch]
