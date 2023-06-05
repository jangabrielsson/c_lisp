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

Ptr readExpr(TokenizerPtr tkz) {
function Lisp.Reader(str)
  local st = Tokenizer(str) 
  local self = { st = st, line = 1 }
  local function parse(st) -- -> Expr
    local pf = parseFuns[st:nextToken()] or parseFuns['TT_DEFAULT']
    return pf(st,self)
  end
  function self:parse()
    local stat,res = pcall(function()
        if st:nextToken() == 'TT_EOF' then
          return Lisp.EOF
        else 
          st:pushBack()
          return parse(st)
        end
      end)
    if not stat then
      error(res)
    else return res end
  end
  return self
end
}

Ptr parse(tokenFun)
{   
    char *token;
    int tokenType = tokenFun->next(&token);
    switch(tokenType) {
        case 0: 
            return NIL; // ERRROR
        case tINTEGER: 
            long n;
            sscanf(token, "%ld", &n);
            return INT2PTR(n); // EOF
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
  TT_STRING = function(st) return st.val end,
  TT_ATOM   = function(st) return Atom(st.val):intern() end,
  TT_QUOTE  = function(st,p) return Cons(Lisp.QUOTE, Cons(p:parse(st), Lisp.NIL)) end,
  TT_HASH   = function(st,p) -- Hack
    local Expr e = p:parse(st).cdr.car
    if e:isAtom() then e = Cons(Lisp.QUOTE, Cons(e, Lisp.NIL)) end
    return Cons(Lisp.FUNCTION, Cons(e, Lisp.NIL))
  end,
  TT_BACKQUOTE = function(st,p) return Cons(Atom("BACKQUOTE"):intern(),Cons(p:parse(st),Lisp.NIL)) end,
  TT_COMMA     = function(st,p) 
    local n = st:nextToken();
    if n == 'TT_DOT' then
      return  Cons(Lisp['*BACK-COMMA-DOT*'],Cons(p:parse(st),Lisp.NIL))
    elseif n == 'TT_AT' then
      return Cons(Lisp['*BACK-COMMA-AT*'],Cons(p:parse(st),Lisp.NIL))
    else 
      st:pushBack();
      return Cons(Lisp['*BACK-COMMA*'],Cons(p:parse(st),Lisp.NIL))
    end
  end,
  TT_LPAR   = function(st,p) 
    if st:nextToken() == 'TT_RPAR' then
      return Lisp.NIL
    else 
      st:pushBack();
      local l = Cons(p:parse(st), Lisp.NIL);
      local t = l;
      while true do
        local tk = st:nextToken()
        if tk == 'TT_RPAR' then
          return l
        elseif tk == 'TT_DOT' then
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
  TT_UNKNOWN = function(st) 
    Lisp:log(0,"Reader TT_UNKNOWN: %s",st.val)
    return Atom(st.val):intern()
  end,
  TT_ERROR = function(st) 
    Lisp:log(0,"Reader TT_ERROR: %s",st.val);
    Exception.Reader("Missing ')'", st:lineNo())
  end,
  TT_DEFAULT = function(st) 
    return Atom(st.val):intern()
  end
}
