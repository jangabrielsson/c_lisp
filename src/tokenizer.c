#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

typedef void *YY_BUFFER_STATE;

extern YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
extern int yylex(void);

extern char lexBuffer[];
struct Tokenizer;

int (*readHook)(char* buffer,int maxBytesToRead, struct Tokenizer *currTokenizer);

typedef struct Tokenizer {
    int (*readInput)(char* buffer, int maxBytesToRead, struct Tokenizer *currTokenizer);
    YY_BUFFER_STATE buffer;
    int closed;
    FILE *fptr;
    char *str;
    int n;
} Tokenizer;

Tokenizer *currTokenizer;

int readInputForLexer(char* buffer, size_t *numBytesRead, int maxBytesToRead)
{
    *numBytesRead = readHook(buffer,maxBytesToRead,currTokenizer);
    return 0;
}

int stringReadHook(char* buffer, int maxBytesToRead, Tokenizer *currTokenizer)  
{
    int i = 0;
    char *str = currTokenizer->str;
    str = str+currTokenizer->n;
    while (*str && i < maxBytesToRead) {
        buffer[i++] = *str++;
    }
    if (buffer[0] == 0L) currTokenizer->closed = 1;
    currTokenizer->n += i;
    return i;
}

int stdinReadHook(char* buffer,int maxBytesToRead,Tokenizer *currTokenizer)  
{
    *buffer = fgetc(stdin);
    return 1;
}

int fileReadHook(char* buffer, int maxBytesToRead, Tokenizer *currTokenizer)
{
    if (currTokenizer->fptr == 0) return 0;
    char *res = fgets(buffer, maxBytesToRead, currTokenizer->fptr);
    if (res == NULL) {
        fclose(currTokenizer->fptr); 
        currTokenizer->closed=1; 
        currTokenizer->fptr=0; 
        return 0;
    }
    return strlen(buffer);
}

int nextToken(char **token, Tokenizer *tkz) 
{
    if (tkz->closed) return 0;
    readHook = tkz->readInput;
    yy_switch_to_buffer(tkz->buffer);
    currTokenizer = tkz;
    int y = yylex();
    if (y == 0L) {
        tkz->closed = 1;
        yy_delete_buffer(tkz->buffer);
        free(tkz);
    }
    *token = lexBuffer;
    return y;
}

Tokenizer *createStdinTokenizer(void)
{
    Tokenizer *tkz = malloc(sizeof(Tokenizer));
    tkz->readInput = stdinReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    return tkz;
}

Tokenizer *createFileTokenizer(char *fname)
{
    FILE *fptr = fopen(fname, "r");
    if (fptr == 0L) return 0L;
    Tokenizer *tkz = malloc(sizeof(Tokenizer));
    tkz->readInput = fileReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->closed = 0;
    tkz->fptr = fptr;
    return tkz;
}

Tokenizer *createStringTokenizer(char *str)
{
    Tokenizer *tkz = malloc(sizeof(Tokenizer));
    tkz->readInput = stringReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->str = str;
    tkz->n = 0;
    return tkz;
}



