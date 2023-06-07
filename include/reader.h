typedef struct ParserStruct {
    Ptr (*read)(struct ParserStruct *parser);
    int (*next)(struct ParserStruct *parser, char**token);
    TokenizerPtr tokenizer;
    char *lastToken;
    int lastTokenType;
} Parser, *ParserPtr;

void testReader(void);
ParserPtr createParser(TokenizerPtr tokenizer);
