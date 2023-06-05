#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h> // g_hash_table
#include <setjmp.h>
#include "types.h"
#include "reader.h"

Ptr *heapStart;
Ptr *heapEnd;
unsigned int heapPtr;

void allocBlock(void)
{
  Ptr *block = (Ptr *)malloc(BLOCK_SIZE);
  if (heapStart == 0L)
  {
    heapStart = block;
    heapEnd = block;
    block[0] = 0;
  }
  else
  {
    heapEnd[0] = (Ptr)block;
    heapEnd = block;
  }
  heapPtr = 2;
}

Ptr allocMem(unsigned int words)
{
  Ptr m = (Ptr)&heapEnd[heapPtr];
  heapPtr += words;
  return m;
}

Ptr mkCons(Ptr car, Ptr cdr)
{
  Ptr c = allocMem(2U);
  c->cons.car = car;
  c->cons.cdr = cdr;
  return (Ptr)TAG(c, CONS_TYPE);
}

Ptr NIL;
Ptr T;
GHashTable *hash;

Ptr mkAtom(char *name, Ptr value)
{
  Ptr a = (Ptr)g_hash_table_lookup(hash, name);
  if (a == 0)
  {
    a = allocMem(2U);
    a->atom.name = name;
    a->atom.value = value;
    g_hash_table_insert(hash, name, a);
  }
  return (Ptr)TAG(a, ATOM_TYPE);
}

Ptr intern(char *name)
{
  return (Ptr)g_hash_table_lookup(hash, name);
}

Ptr defineBuiltin(char *name, ulong nargs, Ptr (*fun)(Ptr *))
{
  Ptr f = allocMem(2U);
  f->builtin.nargs = nargs;
  f->builtin.fun = fun;
  return mkAtom(name, f);
}

Ptr _print(Ptr *stack)
{
  print(stack[0]);
  return stack[0];
}

void init(void)
{
  hash = g_hash_table_new(g_str_hash, g_str_equal);
  allocBlock();

  defineBuiltin("PRINT", 1, &_print);
}

void print(Ptr e)
{
  if (TYPE(e) == ATOM_TYPE)
  {
    printf("%s", UNTAG(e)->atom.name);
  }
  else if (TYPE(e) == CONS_TYPE)
  {
    print(UNTAG(e)->cons.car);
    printf(" . ");
    print(UNTAG(e)->cons.cdr);
  }
  else if (TYPE(e) == INT_TYPE)
  {
    printf("%ld", (long)e >> 2);
  }
}

enum OP
{
  OP_POP,
  OP_PUSH,
  OP_ADD,
  OP_SUB,
  OP_CONS,
  OP_CAR,
  OP_CDR,
  OP_CALL,
  OP_EXIT
};

struct Instr
{
  uint op;
  union
  {
    Ptr value;
    Ptr ptr;
    long num;
  };
};
typedef struct Instr Instr;

#define PUSH(V) stack[++stackPtr] = V
#define POP() (stack[stackPtr--])
#define CHECK_TYPE(P, T, M) ((((ulong)P) & (ulong)0x03) == (ulong)T) ? UNTAG(P) : (Ptr)lerror(M)
#define CHECK_INT(P, M) ((((ulong)P) & (ulong)0x03) == (ulong)INT_TYPE) ? (((long)P) >> 2) : (long)lerror(M)

jmp_buf ex_buf__;
Ptr lerror(char *m)
{
  printf("%s", m);
  longjmp(ex_buf__, 42);
  return NIL;
}

char *ERRSTRINGS[] = {
    "ADD expected number as first argument",
    "ADD expected number as second argument",
};

enum
{
  ERR_CAR = 0,
  ERR_CDR,
};

runCode(Instr *code)
{
  int p = 0;
  int stackPtr = -1;
  Ptr stack[200];
  char *ERR = 0;

  int err = setjmp(ex_buf__);
  if (err != 0)
  {
    printf("ERR");
    return NIL;
  }

  while (code[p].op != OP_EXIT && ERR == 0)
  {
    switch (code[p].op)
    {
    case OP_PUSH:
      PUSH(code[p].value);
      break;
    case OP_POP:
      POP();
      break;
    case OP_ADD:
    {
      long x = (long)POP();
      x = CHECK_INT(x, "ADD expected number as first argument");
      long y = (long)POP();
      y = CHECK_INT(y, "ADD expected number as second argument");
      PUSH(INT2PTR(x + y));
      break;
    }
    case OP_SUB:
    {
      long x = (long)POP();
      x = CHECK_INT(x, "SUB expected number as first argument");
      long y = (long)POP();
      y = CHECK_INT(y, "SUB expected number as second argument");
      PUSH(INT2PTR(x - y));
      break;
    }
    case OP_CONS:
    {
      Ptr a = POP();
      Ptr b = POP();
      PUSH(mkCons(b, a));
      break;
    }
    case OP_CAR:
    {
      Ptr e = POP();
      e = CHECK_TYPE(e, CONS_TYPE, "car of non-cons");
      PUSH(e->cons.car);
      break;
    }
    case OP_CDR:
    {
      Ptr e = POP();
      e = CHECK_TYPE(e, CONS_TYPE, "cdr of non-cons");
      PUSH(e->cons.cdr);
      break;
    }
    case OP_CALL:
    {
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
    }
    p++;
  }
  return POP();
}

#define PUSHINT(I)                     \
  {                                    \
    .op = OP_PUSH, .value = INT2PTR(I) \
  }
#define ADD()    \
  {              \
    .op = OP_ADD \
  }
#define EXIT()    \
  {               \
    .op = OP_EXIT \
  }
#define CALL(F)                       \
  {                                   \
    .op = OP_CALL, .value = intern(F) \
  }

int main(void)
{
  testReader();

  printf("Lisp\n");
  init();
  long a;

  NIL = mkAtom("NIL", 0L);
  NIL->atom.value = NIL;

  T = mkAtom("T", 0L);
  T->atom.value = T;

  printf("Ptr %lu, Long %lu\n", sizeof(Ptr), sizeof(a));
  printf("%lx\n", ((ulong)NIL));
  printf("%lx\n", ((ulong)NIL) & (ulong)0x03);
  print(NIL);
  printf("\n");

  Ptr l = mkCons(INT2PTR(3), mkCons(INT2PTR(4), mkCons(INT2PTR(5), NIL)));
  print(l);
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
