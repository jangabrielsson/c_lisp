#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "y.tab.h"
#include "types.h"
#include "tokenizer.h"
#include "reader.h"

#define tEOF 0

static jmp_buf ex_buf__;
static Ptr lerror(char *m) {
  printf("%s", m);
  longjmp(ex_buf__, 42);
  return NIL;
}

static Ptr parse(ParserPtr self) {   
    char *token;
    int tokenType = self->nextToken(self, &token);
    switch(tokenType) {
        case 0: 
            return NIL; // ERRROR
        case tINTEGER: {
            long n;
            sscanf(token, "%ld", &n);
            return INT2PTR(n); 
        }
        case tATOM: 
            return mkAtom(token, NIL);
        case tLBRACE:   
            if (self->nextToken(self, &token) == tRBRACE) {
                return NIL; 
            } else {
                self->pushBack(self);
                Ptr l = mkCons(parse(self), NIL);
                Ptr t = l;
                while (1) {
                    int tk = self->nextToken(self,&token);
                    if (tk == tRBRACE) {
                        return l;
                    } else if (tk == tDOT) {
                        t->cons.cdr = parse(self);
                        if (self->nextToken(self,&token) != tRBRACE) {
                            lerror("Missing ')'");
                        } else {
                            return l;
                        }
                    } else if (tk == tEOF) {
                        lerror("Malformed list!");
                    } else {
                        self->pushBack(self);
                        t->cons.cdr = mkCons(parse(self), NIL);
                        t = t->cons.cdr;
                    } 
                } // while
            } // else
     
        default:    
            return NIL; // EOF
    } // switch
}

static Ptr readS(ParserPtr parser) {
    int err = setjmp(ex_buf__);
    if (err != 0) {
        printf("ERR");
        return NIL;
    }
    return parse(parser);
}

static int readToken(ParserPtr parser, char** token) {
    if (parser->last) {
        parser->last = 0;
        *token = parser->lastTokenStr;
        return parser->lastToken;
    }
    parser->lastToken = parser->tokenizer->nextToken(parser->tokenizer, &(parser->lastTokenStr));
    *token = parser->lastTokenStr;
    return parser->lastToken;
}

static void pushBack(ParserPtr parser) {
    parser->last = 1;
}

ParserPtr createParser(TokenizerPtr tokenizer) {
    ParserPtr parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;
    parser->lastTokenStr = NULL;
    parser->lastToken = 0;
    parser->read = readS;
    parser->nextToken = readToken;
    parser->pushBack = pushBack;
    return parser;
}

void testReader(void) {
    printf("Lisp>");

    char str[] = "(42 (A B) 41)";
    TokenizerPtr tokenizer = tokenizer_string(str);
    ParserPtr parser = createParser(tokenizer);

    Ptr p = parser->read(parser);
    printf("read: %s -> ", str);
    lisp_print(p);
    printf("\n");
}
