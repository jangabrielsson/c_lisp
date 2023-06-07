#include <stdio.h>
#include <setjmp.h>
#include "types.h"
#include "vm.h"


static jmp_buf ex_buf__;
static Ptr lerror(char *m) {
  printf("%s", m);
  longjmp(ex_buf__, 42);
  return NIL;
}

char *ERRSTRINGS[] = {
    "ADD expected number as first argument",
    "ADD expected number as second argument",
};

enum ERRORS {
  ERR_CAR = 0,
  ERR_CDR,
};

Ptr runCode(Instr *code) {
  int p = 0;
  int stackPtr = -1;
  Ptr stack[200];
  char *ERR = 0;

  int err = setjmp(ex_buf__);
  if (err != 0){
    printf("ERR");
    return NIL;
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
        x = CHECK_INT(x, "ADD expected number as first argument");
        long y = (long)POP();
        y = CHECK_INT(y, "ADD expected number as second argument");
        PUSH(INT2PTR(x + y));
        break;
      }
      case OP_SUB: {
        long x = (long)POP();
        x = CHECK_INT(x, "SUB expected number as first argument");
        long y = (long)POP();
        y = CHECK_INT(y, "SUB expected number as second argument");
        PUSH(INT2PTR(x - y));
        break;
      }
      case OP_CONS: {
        Ptr a = POP();
        Ptr b = POP();
        PUSH(mkCons(b, a));
        break;
      }
      case OP_CAR: {
        Ptr e = POP();
        e = CHECK_TYPE(e, CONS_TYPE, "car of non-cons");
        PUSH(e->cons.car);
        break;
      }
      case OP_CDR: {
        Ptr e = POP();
        e = CHECK_TYPE(e, CONS_TYPE, "cdr of non-cons");
        PUSH(e->cons.cdr);
        break;
      }
      case OP_CALL: {
        Ptr ptr = UNTAG(code[p].value);
        ptr = UNTAG(ptr->atom.value);
        // Check that we have args
        stackPtr -= ptr->builtin.nargs;
        PUSH(ptr->builtin.fun(stack));
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

