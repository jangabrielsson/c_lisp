typedef struct ParserStruct {
    Ptr (*read)(struct ParserStruct *parser,int *err_code);
    LispPtr lisp;
    void (*push_back)(struct ParserStruct *parser);
    int (*next_token)(struct ParserStruct *parser, char**token);
    void *tokenizer;
    int last;
    char *last_token_str;
    int last_token;
    Ptr NIL;
    Ptr T;
    void *ex_buf;
} Parser, *ParserPtr;

ParserPtr string_reader(LispPtr lisp, const char *str);
ParserPtr stdin_reader(LispPtr lisp);
ParserPtr file_reader(LispPtr lisp, const char *filename);
void lisp_print(LispPtr lisp, Ptr e);
