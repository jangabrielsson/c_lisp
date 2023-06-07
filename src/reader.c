#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "tokenizer.h"
#include "types.h"

void testReader(void)
{
    printf("Lisp>");

    TokenizerPtr tokenizer = createStringTokenizer("(+ 1 2 3)");
    while (1) {
        char *token;
        int tokenType = nextToken(&token, tokenizer);
        if (tokenType == 0) break; // EOF
        printf("b %d %s\n",tokenType,token);
    }
}

Ptr parse(tokenFun)
{   
    char *token;
    int tokenType = tokenFun->next(&token);
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
            return NIL; // EOF
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


Ptr reads(TokenizerPtr ptr)
{
    return NIL;
}