typedef struct ParserStruct {
    Ptr (*read)(struct ParserStruct *parser);
    void (*pushBack)(struct ParserStruct *parser);
    int (*nextToken)(struct ParserStruct *parser, char**token);
    TokenizerPtr tokenizer;
    int last;
    char *lastTokenStr;
    int lastToken;
} Parser, *ParserPtr;

void testReader(void);
ParserPtr createParser(TokenizerPtr tokenizer);
