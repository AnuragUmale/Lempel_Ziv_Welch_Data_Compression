# Lempel-Ziv Compression

Lempel-Ziv Compression is a data compression algorithm that works by finding repeated patterns in a sequence of data and replacing them with shorter codes. It is a widely used lossless compression technique in various applications, such as image, audio, and video compression, and is known for its efficiency and simplicity.

# Files in the repository

1. code.h
2. decode.c
3. encode.c
4. endian.h
5. io.c
6. io.h
7. Makefile
8. README.md
9. trie.c
10. trie.h
11. word.c
12. word.h

## Build

### Building everything

```
$ make
```

**OR**

```
$ make all
```

### Building encode

```
$ make encode
```

### Building decode

```
$ make decode
```

## Running

### encode

```
$ ./encode [-vh] [-i input] [-o output]
```

 The encode program encodes a file. You can specify the following command line arguments.

- `-h`: Shows help and usage
- `-i infile`: Input containing graph (default: stdin)
- `-o outfile`: Output of computed path (default: stdout)
- `-v`: Enable verbose printing.

### decode
```
$ ./decode [-vh] [-i input] [-o output]
```

The decode program decodes a file. You can specify the following command line arguments.

- `-h`: Shows help and usage
- `-i infile`: Input containing graph (default: stdin)
- `-o outfile`: Output of computed path (default: stdout)
- `-v`: Enable verbose printing.

## Cleaning

```
$ make clean
```

# Note

If you are a student and if are taking a look at my repository for your assignment, any piece of code that your copy from this repository is completely your responsibility. I should not be held repsonsible for any academic misconduct.