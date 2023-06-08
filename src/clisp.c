#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h> // g_hash_table
#include "clisp.h"
#include "reader.h"

#define VERSION "0.1"
#define MEM_SIZE 4096

static void print_greeting(LispPtr lisp) {
    printf("Welcome to CLisp! v%s\n",lisp->version);
}

static void print_architecture(LispPtr lisp) {
    printf("%ld bit\n", sizeof(void *) * 8);
}

Ptr heap_alloc(LispPtr lisp, unsigned int words) {
  Ptr m = (Ptr)&(lisp->heap[lisp->heap_ptr]);
  lisp->heap_ptr += words;
  return m;
}

Ptr make_cons(LispPtr lisp, Ptr car, Ptr cdr) {
  Ptr c = heap_alloc(lisp,2U);
  c->cons.car = car;
  c->cons.cdr = cdr;
  return (Ptr)TAG(c, CONS_TYPE);
}

Ptr make_atom(LispPtr lisp, char *name, Ptr value) {
  Ptr a = (Ptr)g_hash_table_lookup((GHashTable *)lisp->hash, name);
  if (a == 0) {
    a = heap_alloc(lisp,2U);
    a->atom.name = strdup(name);
    a->atom.value = value;
    g_hash_table_insert((GHashTable *)lisp->hash, name, a);
  }
  return (Ptr)TAG(a, ATOM_TYPE);
}

Ptr intern(LispPtr lisp, const char *name) {
  return (Ptr)g_hash_table_lookup((GHashTable *)lisp->hash, name);
}

Ptr define_builtin(LispPtr lisp, char *name, ulong nargs, Ptr (*fun)(LispPtr lisp, Ptr *)) {
  Ptr f = heap_alloc(lisp,2U);
  f->builtin.nargs = nargs;
  f->builtin.fun = fun;
  return make_atom(lisp, name, f);
}

static Ptr _print(LispPtr lisp, Ptr *stack) {
  lisp_print(lisp, stack[0]);
  return stack[0];
}

static void init_mem(LispPtr lisp) {
    lisp->heap = (Ptr *)malloc(MEM_SIZE);
    lisp->heap_ptr = 0;
    lisp->hash = (void *)g_hash_table_new(g_str_hash, g_str_equal);
}

static ParserPtr pin = 0L;
static Ptr lisp_stdin_reader(LispPtr lisp) {
    if (pin == 0) pin = stdin_reader(lisp);
    return pin->read(pin,&lisp->read_error);
}

LispPtr create_lisp() {
    LispPtr lisp = malloc(sizeof(Lisp));
    lisp->version = VERSION;

    init_mem(lisp);

    // lisp->read_string = string_reader;
    // lisp->read_file = file_reader;
    lisp->read_stdin = lisp_stdin_reader;
    lisp->print = lisp_print;
    lisp->print_greeting = print_greeting;
    lisp->print_architecture = print_architecture;

    lisp->NIL = make_atom(lisp, "NIL", 0L);
    lisp->NIL->atom.value = lisp->NIL;
    lisp->T = make_atom(lisp, "T", 0L);
    lisp->T->atom.value = lisp->T;

    define_builtin(lisp, "PRINT", 1, &_print);
    return lisp;
}

void dispose_lisp(LispPtr lisp) {
    free(lisp->heap);
    g_hash_table_destroy((GHashTable *)lisp->hash);
    free(lisp);
}