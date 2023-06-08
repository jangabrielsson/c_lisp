#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "y.tab.h"

#include "clisp.h"
#include "tokenizer.h"
#include "reader.h"

#define tEOF 0

static void lerror(ParserPtr self, int errcode) {
  longjmp(*(jmp_buf *)(self->ex_buf), errcode);
}

enum err_codes {
  ERR_EOF = 1,
  ERR_MALFORMED_LIST,
  ERR_MISSING_RBRACE,
  ERR_UNKNOWN_TOKEN,
};

const char* err_strings[] = {
    "___",
    "end-of-file",
    "malformed list",
    "missing ')'",
    "unknown token",
};

static Ptr parse(ParserPtr self) {   
    char *token;
    int token_type = self->next_token(self, &token);
    switch(token_type) {
        case 0: 
            lerror(self, ERR_EOF);
        case tINTEGER: {
            long n;
            sscanf(token, "%ld", &n);
            return INT2PTR(n); 
        }
        case tATOM: 
            return make_atom(self->lisp, token, self->NIL);
        case tLBRACE:   
            if (self->next_token(self, &token) == tRBRACE) {
                return self->NIL; 
            } else {
                self->push_back(self);
                Ptr l = make_cons(self->lisp, parse(self), self->NIL);
                Ptr t = l;
                while (1) {
                    int tk = self->next_token(self,&token);
                    if (tk == tRBRACE) {
                        return l;
                    } else if (tk == tDOT) {
                        t->cons.cdr = parse(self);
                        if (self->next_token(self,&token) != tRBRACE) {
                            lerror(self, ERR_MALFORMED_LIST);
                        } else {
                            return l;
                        }
                    } else if (tk == tEOF) {
                        lerror(self, ERR_MALFORMED_LIST);
                    } else {
                        self->push_back(self);
                        t->cons.cdr = make_cons(self->lisp, parse(self), self->NIL);
                        t = t->cons.cdr;
                    } 
                } // while
            } // else
     
        default:    
            lerror(self, ERR_UNKNOWN_TOKEN);
    } // switch
    return self->NIL;
}

static Ptr read_s(ParserPtr self, int *read_error) {
    jmp_buf ex_buf;
    self->ex_buf = (void *)&ex_buf;
    int err = setjmp(ex_buf);
    *read_error = 0;
    if (err != 0) {
        printf("Reader error: %s\n", err_strings[err]);
        if (err == ERR_UNKNOWN_TOKEN) {
            printf("Unknown token: %s\n", self->last_token_str);    
        }
        *read_error = err;
        return self->NIL;
    }
    return parse(self);
}

static int read_token(ParserPtr self, char** token) {
    if (self->last) {
        self->last = 0;
        *token = self->last_token_str;
        return self->last_token;
    }
    TokenizerPtr tp = (TokenizerPtr)self->tokenizer;
    self->last_token = tp->next_token(tp, &(self->last_token_str));
    *token = self->last_token_str;
    return self->last_token;
}

static void push_back(ParserPtr self) {
    self->last = 1;
}

static ParserPtr create_parser(LispPtr lisp, TokenizerPtr tokenizer) {
    ParserPtr parser = malloc(sizeof(Parser));
    parser->lisp = lisp;
    parser->NIL = lisp->NIL;
    parser->T = lisp->T;
    parser->tokenizer = tokenizer;
    parser->last_token_str = NULL;
    parser->last_token = 0;
    parser->read = read_s;
    parser->next_token = read_token;
    parser->push_back = push_back;
    return parser;
}

ParserPtr string_reader(LispPtr lisp, const char *str) {
    TokenizerPtr tokenizer = string_tokenizer(str);
    return create_parser(lisp, tokenizer);
}

ParserPtr stdin_reader(LispPtr lisp) {
    TokenizerPtr tokenizer = stdin_tokenizer();
    return create_parser(lisp, tokenizer);
}

ParserPtr file_reader(LispPtr lisp, const char *filename) {
    TokenizerPtr tokenizer = file_tokenizer(filename);
    return create_parser(lisp, tokenizer);
}

void lisp_print(LispPtr lisp, Ptr e) {
  if (TYPE(e) == ATOM_TYPE) {
    printf("%s", UNTAG(e)->atom.name);
  }
  else if (TYPE(e) == CONS_TYPE) {
    printf("(");
    while (TYPE(e) == CONS_TYPE) {
      lisp_print(lisp, UNTAG(e)->cons.car);
      e = UNTAG(e)->cons.cdr;
      if (TYPE(e) == CONS_TYPE) printf(" ");
    }
    if (e != lisp->NIL) {
      printf(" . ");
      lisp_print(lisp, e);
    }
    printf(")");
  } else if (TYPE(e) == INT_TYPE) {
    printf("%ld", (long)e >> 2);
  }
}

