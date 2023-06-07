#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h> // g_hash_table
#include "types.h"
#include "tokenizer.h"
#include "reader.h"
#include "vm.h"

Ptr *heapStart;
Ptr *heapEnd;
unsigned int heapPtr;

void allocBlock(void) {
  Ptr *block = (Ptr *)malloc(BLOCK_SIZE);
  if (heapStart == 0L) {
    heapStart = block;
    heapEnd = block;
    block[0] = 0;
  }
  else {
    heapEnd[0] = (Ptr)block;
    heapEnd = block;
  }
  heapPtr = 2;
}

Ptr allocMem(unsigned int words) {
  Ptr m = (Ptr)&heapEnd[heapPtr];
  heapPtr += words;
  return m;
}

Ptr mkCons(Ptr car, Ptr cdr) {
  Ptr c = allocMem(2U);
  c->cons.car = car;
  c->cons.cdr = cdr;
  return (Ptr)TAG(c, CONS_TYPE);
}

GHashTable *hash;

Ptr mkAtom(char *name, Ptr value) {
  Ptr a = (Ptr)g_hash_table_lookup(hash, name);
  if (a == 0) {
    a = allocMem(2U);
    a->atom.name = strdup(name);
    a->atom.value = value;
    g_hash_table_insert(hash, name, a);
  }
  return (Ptr)TAG(a, ATOM_TYPE);
}

Ptr intern(char *name) {
  return (Ptr)g_hash_table_lookup(hash, name);
}

Ptr defineBuiltin(char *name, ulong nargs, Ptr (*fun)(Ptr *)) {
  Ptr f = allocMem(2U);
  f->builtin.nargs = nargs;
  f->builtin.fun = fun;
  return mkAtom(name, f);
}

Ptr _print(Ptr *stack) {
  lisp_print(stack[0]);
  return stack[0];
}

void init(void) {
  hash = g_hash_table_new(g_str_hash, g_str_equal);
  allocBlock();
  NIL = mkAtom("NIL", 0L);
  NIL->atom.value = NIL;
  T = mkAtom("T", 0L);
  T->atom.value = T;
  defineBuiltin("PRINT", 1, &_print);
}

void lisp_print(Ptr e) {
  if (TYPE(e) == ATOM_TYPE) {
    printf("%s", UNTAG(e)->atom.name);
  }
  else if (TYPE(e) == CONS_TYPE) {
    printf("(");
    while (TYPE(e) == CONS_TYPE) {
      lisp_print(UNTAG(e)->cons.car);
      e = UNTAG(e)->cons.cdr;
      if (TYPE(e) == CONS_TYPE) printf(" ");
    }
    if (e != NIL) {
      printf(" . ");
      lisp_print(e);
    }
    printf(")");
  } else if (TYPE(e) == INT_TYPE) {
    printf("%ld", (long)e >> 2);
  }
}

int main(void) {
  init();
  testReader();

  printf("Lisp\n");
  long a;

  printf("Ptr %lu, Long %lu\n", sizeof(Ptr), sizeof(a));
  printf("%lx\n", ((ulong)NIL));
  printf("%lx\n", ((ulong)NIL) & (ulong)0x03);
  lisp_print(NIL);
  printf("\n");

  Ptr l = mkCons(INT2PTR(3), mkCons(INT2PTR(4), mkCons(INT2PTR(5), NIL)));
  lisp_print(l);
  printf("\n");

  Instr code[] = {
      PUSHINT(3),
      PUSHINT(4),
      ADD(),
      CALL("PRINT"),
      EXIT()};

  runCode(code);

  return 0;
}
