typedef struct TokenizerStruct {
    int (*nextToken)(char** buffer, struct TokenizerStruct *tokenizer);
} *TokenizerPtr;

int nextToken(char **token, TokenizerPtr tkz);

TokenizerPtr createStdinTokenizer(void);
TokenizerPtr createFileTokenizer(char *fname);
TokenizerPtr createStringTokenizer(char *str);