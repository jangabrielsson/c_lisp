typedef struct TokenizerStruct {
    int (*next_token)(struct TokenizerStruct *self, char** buffer);
} *TokenizerPtr;

TokenizerPtr stdin_tokenizer(void);
TokenizerPtr file_tokenizer(const char *fname);
TokenizerPtr string_tokenizer(const char *str);
TokenizerPtr dispose_tokenizer(TokenizerPtr self);
