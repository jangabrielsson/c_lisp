typedef struct TokenizerStruct {
    int (*nextToken)(struct TokenizerStruct *self, char** buffer);
} *TokenizerPtr;

TokenizerPtr tokenizer_stdin(void);
TokenizerPtr tokenizer_file(char *fname);
TokenizerPtr tokenizer_string(char *str);
TokenizerPtr tokenizer_dispose(TokenizerPtr self);
