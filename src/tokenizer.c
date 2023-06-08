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
    int (*next_token)(struct TokenizerStruct *tokenizer, char** buffer);
    int (*read_input)(struct TokenizerStruct *curr_tokenizer, char* buffer, int maxBytesToRead);
    YY_BUFFER_STATE buffer;
    int closed;
    FILE *fptr;
    const char *str;
    int n;
} Tokenizer, *TokenizerPtr;

int (*read_hook)(TokenizerPtr self, char* buffer, int maxBytesToRead);

TokenizerPtr curr_tokenizer;

int readInputForLexer(char* buffer, size_t *numBytesRead, int maxBytesToRead)
{
    *numBytesRead = read_hook(curr_tokenizer,buffer,maxBytesToRead);
    return 0;
}

static int string_read_hook(TokenizerPtr self, char* buffer, int maxBytesToRead)  
{
    int i = 0;
    char *str = (char *)self->str;
    str = str+self->n;
    while (*str && i < maxBytesToRead) {
        buffer[i++] = *str++;
    }
    if (buffer[0] == 0L) self->closed = 1;
    self->n += i;
    return i;
}

static int stdin_read_hook(TokenizerPtr self, char* buffer,int maxBytesToRead)  
{
    *buffer = fgetc(stdin);
    return 1;
}

static int file_read_hook(TokenizerPtr self, char* buffer, int maxBytesToRead)
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

void dispose_tokenizer(TokenizerPtr self)
{
    if (self->closed) return;
    self->closed = 1;
    yy_delete_buffer(self->buffer);
    free(self);
}

static int next_token(TokenizerPtr self, char **token) // When last token is read the tkz ptr is deallocated(!!!)
{
    if (self->closed) return 0;
    read_hook = self->read_input;   // Ouch! how to make this thread safe? semaphores?
    yy_switch_to_buffer(self->buffer);
    curr_tokenizer = self;
    int y = yylex();
    *token = lexBuffer;
    return y;
}

TokenizerPtr stdin_tokenizer(void)
{
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->next_token = next_token;
    tkz->read_input = stdin_read_hook;
    tkz->buffer = yy_create_buffer(0, 8192);
    return tkz;
}

TokenizerPtr file_tokenizer(const char *fname)
{
    FILE *fptr = fopen(fname, "r");
    if (fptr == 0L) return 0L;
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->next_token = next_token;
    tkz->read_input = file_read_hook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->closed = 0;
    tkz->fptr = fptr;
    return tkz;
}

TokenizerPtr string_tokenizer(const char *str)
{
    TokenizerPtr tkz = malloc(sizeof(Tokenizer));
    tkz->next_token = next_token;
    tkz->read_input = string_read_hook;
    tkz->buffer = yy_create_buffer(0, 8192);
    tkz->str = str;
    tkz->n = 0;
    return tkz;
}



