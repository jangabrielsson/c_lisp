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

typedef struct TokenizerStruct {
    int (*nextToken)(struct TokenizerStruct *tokenizer, char** buffer);
    int (*readInput)(struct TokenizerStruct *currTokenizer, char* buffer, int maxBytesToRead);
    YY_BUFFER_STATE buffer;
    int closed;
    FILE *fptr;
    char *str;
    int n;
} Tokenizer, *TokenizerPtr;

int (*readHook)(TokenizerPtr self, char* buffer, int maxBytesToRead);

TokenizerPtr currTokenizer;

int readInputForLexer(char* buffer, size_t *numBytesRead, int maxBytesToRead)
{
    *numBytesRead = readHook(currTokenizer,buffer,maxBytesToRead);
    return 0;
}

static int stringReadHook(TokenizerPtr self, char* buffer, int maxBytesToRead)  
{
    int i = 0;
    char *str = self->str;
    str = str+self->n;
    while (*str && i < maxBytesToRead) {
        buffer[i++] = *str++;
    }
    if (buffer[0] == 0L) self->closed = 1;
    self->n += i;
    return i;
}

static int stdinReadHook(TokenizerPtr self, char* buffer,int maxBytesToRead)  
{
    *buffer = fgetc(stdin);
    return 1;
}

static int fileReadHook(TokenizerPtr self, char* buffer, int maxBytesToRead)
{
    if (self->fptr == 0) return 0;
    char *res = fgets(buffer, maxBytesToRead, self->fptr);
    if (res == NULL) {
        fclose(self->fptr); 
        self->closed=1; 
        self->fptr=0; 
        return 0;
    }
    return strlen(buffer);
}

void tokenizer_dispose(TokenizerPtr self)
{
    if (self->closed) return;
    self->closed = 1;
    yy_delete_buffer(self->buffer);
    free(self);
}

static int nextToken(TokenizerPtr self, char **token) // When last token is read the tkz ptr is deallocated(!!!)
{
    if (self->closed) return 0;
    readHook = self->readInput;
    yy_switch_to_buffer(self->buffer);
    currTokenizer = self;
    int y = yylex();
    *token = lexBuffer;
    return y;
}

TokenizerPtr tokenizer_stdin(void)
{
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->nextToken = nextToken;
    tkz->readInput = stdinReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    return tkz;
}

TokenizerPtr tokenizer_file(char *fname)
{
    FILE *fptr = fopen(fname, "r");
    if (fptr == 0L) return 0L;
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->nextToken = nextToken;
    tkz->readInput = fileReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->closed = 0;
    tkz->fptr = fptr;
    return tkz;
}

TokenizerPtr tokenizer_string(char *str)
{
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->nextToken = nextToken;
    tkz->readInput = stringReadHook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->str = str;
    tkz->n = 0;
    return tkz;
}



