#include <stdio.h>
#include <setjmp.h>
#include "clisp.h"
#include "vm.h"


static jmp_buf ex_buf__;
static long lerror(int errcode) {
  longjmp(ex_buf__, errcode);
  return 0L;
}

enum ERRORS {
  ERR_ADD_1 = 1,
  ERR_ADD_2,
  ERR_SUB_1 = 1,
  ERR_SUB_2,
  ERR_CAR,
  ERR_CDR,
  ERR_NUM,

};

char *ERRSTRINGS[] = {
    "___",
    "ADD expected number as first argument",
    "ADD expected number as second argument",
    "SUB expected number as first argument",
    "SUB expected number as second argument",
    "car of non-cons",    
    "cdr of non-cons",
    "not a number"
};


Ptr runCode(LispPtr lisp, Instr *code) {
  int p = 0;
  int stackPtr = -1;
  Ptr stack[200];
  char *ERR = 0;

  int err = setjmp(ex_buf__);
  if (err != 0) {
    printf("VM error: %s", ERRSTRINGS[err]);
    return lisp->NIL;
  }

  while (code[p].op != OP_EXIT && ERR == 0) {
    switch (code[p].op) {
      case OP_PUSH:
        PUSH(code[p].value);
        break;
      case OP_POP:
        POP();
        break;
      case OP_ADD: {
        long x = (long)POP();
        x = CHECK_INT(x, ERR_ADD_1);
        long y = (long)POP();
        y = CHECK_INT(y, ERR_ADD_2);
        PUSH(INT2PTR(x + y));
        break;
      }
      case OP_SUB: {
        long x = (long)POP();
        x = CHECK_INT(x, ERR_SUB_1);
        long y = (long)POP();
        y = CHECK_INT(y, ERR_SUB_2);
        PUSH(INT2PTR(x - y));
        break;
      }
      case OP_CONS: {
        Ptr a = POP();
        Ptr b = POP();
        PUSH(make_cons(lisp, b, a));
        break;
      }
      case OP_CAR: {
        Ptr e = POP();
        e = CHECK_TYPE(e, CONS_TYPE, ERR_CAR);
        PUSH(e->cons.car);
        break;
      }
      case OP_CDR: {
        Ptr e = POP();
        e = CHECK_TYPE(e, CONS_TYPE, ERR_CDR);
        PUSH(e->cons.cdr);
        break;
      }
      case OP_CALL: {
        Ptr ptr = UNTAG(code[p].value);
        ptr = UNTAG(ptr->atom.value);
        // Check that we have args
        stackPtr -= ptr->builtin.nargs;
        PUSH(ptr->builtin.fun(lisp,stack));
      }
      break;
      case OP_EXIT:
        p--;
        break;
      } // switch
    p++;
  } // while
  return POP();
}

