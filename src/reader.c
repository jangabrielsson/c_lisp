#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#include "types.h"
#include "tokenizer.h"
#include "reader.h"

Ptr parse(ParserPtr self)
{   
    char *token;
    int tokenType = self->next(self, &token);
    switch(tokenType) {
        case 0: 
            return NIL; // ERRROR
        case tINTEGER: 
        {
            long n;
            sscanf(token, "%ld", &n);
            return INT2PTR(n); // EOF
        }
        case tSTRING: 
            return NIL; // EOF
        case tATOM: 
            return mkAtom(token, NIL);
        case tQUOTE: 
            return NIL; // EOF
        case tBACK_QUOTE:
            return NIL; // EOF
        case tCOMMA:
            return NIL; // EOF
        case tCOM_DOT:  
            return NIL; // EOF
        case tCOM_AT:   
            return NIL; // EOF
        case tLBRACK:   
            return NIL; // EOF
        case tRBRACK:   
            return NIL; // EOF
        case tLBRACE:   
            if (self->next(self, &token) == tRBRACE) {
                return NIL; // EOF
            } else {
                self->pushBack(self);
                Ptr l = mkCons(self->parse(self), NIL);
                while (1) {
                    int tk = self->next(self,&token);
                    if (tk == tRBRACE) {
                        return l
                    } elseif (tk == 'TT_DOT') {
                        t.cdr = p:parse(st)
                        if st:nextToken() ~= 'TT_RPAR' then
                            Exception.Reader("Missing ')'", st:lineNo())
                        else
                            return l
                        end
                    } elseif (tk == 'TT_EOF') {
                        Exception.Reader("Malformed list!", st:lineNo())
                    } else
                        self->pushBack();
                        t.cdr = Cons(p:parse(st), Lisp.NIL);
                        t = t.cdr
                        -- break
                    }
          t.cdr = p:parse(st)
          if st:nextToken() ~= 'TT_RPAR' then
            Exception.Reader("Missing ')'", st:lineNo())
          else
            return l
          end
        elseif tk == 'TT_EOF' then
          Exception.Reader("Malformed list!", st:lineNo())
        else
          st:pushBack();
          t.cdr = Cons(p:parse(st), Lisp.NIL);
          t = t.cdr
          -- break
        end
      end
    end
  end,
        case tRBRACE:   
            return NIL; // EOF
        case tFUNCTION: 
            return NIL; // EOF
        case tKEYWORD:  
            return NIL; // EOF
        case tMESSAGE:  
            return NIL; // EOF
        case tADD:  
            return NIL; // EOF
        case tSUB:  
            return NIL; // EOF
        case tMUL:  
            return NIL; // EOF
        case tDIV:  
            return NIL; // EOF
        case tGREATER:  
            return NIL; // EOF
        case tLESS: 
            return NIL; // EOF
        case tEQUAL:    
            return NIL; // EOF
        case tDOT:  
            return NIL; // EOF
        case tHEX_INTEGER:  
            return NIL; // EOF
        case tFLOAT:    
            // float f;
            // sscanf(token, "%lf", &f);
            // return INT2PTR(f); // EOF
        case tCHRLIT:   
            return NIL; // EOF
        case tSTRLIT:   
            return NIL; // EOF
        default:    
            return NIL; // EOF
    }
}

static int readToken(ParserPtr parser, char** token)
{
    return parser->tokenizer->nextToken(parser->tokenizer, token);
}

ParserPtr createParser(TokenizerPtr tokenizer)
{
    ParserPtr parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;
    parser->lastToken = NULL;
    parser->lastTokenType = 0;
    parser->read = parse;
    parser->next = readToken;
    return parser;
}

void testReader(void)
{
    printf("Lisp>");

    char str[] = "42";
    TokenizerPtr tokenizer = tokenizer_string(str);
    ParserPtr parser = createParser(tokenizer);

    Ptr p = parser->read(parser);
    printf("read: %s -> ", str);
    lisp_print(p);
    printf("\n");
}
