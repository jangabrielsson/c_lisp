#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clisp.h"
#include "vm.h"

#define TRUE 1

static void read_print_loop(LispPtr lisp) {
  while (TRUE) {
    printf("\n"); gc_info(lisp);
    printf("\nLisp>");
    Ptr exit = make_atom(lisp,"EXIT",lisp->NIL);
    Ptr expr = lisp->read_stdin(lisp);
    if (ERROR(lisp)) {
      printf("Error: %s\n", lisp->error_string);
      continue;
    }
    if (expr == exit) break;
    lisp->print(lisp,expr);
  }
} 

int main(void) {
  LispPtr lisp = create_lisp();
  lisp->print_greeting(lisp);
  lisp->print_architecture(lisp);

  Instr code[] = {
    PUSHINT(3),
    PUSHINT(4),
    ADD(),
    CALL(lisp,"PRINT"),
    EXIT()
  };

  runCode(lisp, code);

  read_print_loop(lisp);
  dispose_lisp(lisp);
  return 0;
}
