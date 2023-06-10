typedef unsigned long ulong;
typedef unsigned int uint;

#define ERROR(lisp) (lisp->error != 0)

union Expr;
typedef union Expr Expr;
typedef Expr *Ptr;

typedef struct EnvStruct {

} Env, *EnvPtr;

typedef struct LispStruct {
    EnvPtr env;
    char *version;

    void *hash;
    Ptr *heap;
    int heap_ptr;

    Ptr (* read_string)(struct LispStruct *lisp, const char *str);
    Ptr (* read_stdin)(struct LispStruct *lisp);
    Ptr (* read_file)(struct LispStruct *lisp, const char *fname);

    void (* print)(struct LispStruct *lisp, const Ptr expr);
    void (* print_greeting)(struct LispStruct *lisp);
    void (* print_architecture)(struct LispStruct *lisp);

    int error;
    char error_string[256];

    Ptr NIL;
    Ptr T;
} Lisp, *LispPtr;

struct Atom {
    char *name;
    Ptr value;
};
typedef struct Atom Atom;

struct Cons {
    Ptr car;
    Ptr cdr;
};
typedef struct Cons Cons;

struct Builtin {
    ulong nargs;
    Ptr (*fun)(LispPtr lisp, Ptr *stack);
};
typedef struct Builtin Builtin;

union Expr {
    Atom atom;
    Cons cons;
    Builtin builtin;
};

#define BLOCK_LEN (2*1000)
#define BLOCK_SIZE (BLOCK_LEN*sizeof(Ptr))
#define TAG(P,B) (((ulong)P) | ((ulong) B))
#define TYPE(P) (((ulong)P) & (ulong)0x03)
#define UNTAG(P) ((Ptr)(((ulong)P) & ~((ulong) 0x03)))

#define CONS_TYPE 0x00
#define ATOM_TYPE 0x01
#define INT_TYPE 0x02
#define EXT_TYPE 0x03

#define PTR2INT(l) ((long)(l) >> 2)
#define INT2PTR(l) ((Ptr)((l) << 2 | INT_TYPE))

Ptr make_cons(LispPtr lisp, Ptr car, Ptr cdr);
Ptr make_atom(LispPtr lisp, char *name, Ptr value);

LispPtr create_lisp(void);
void dispose_lisp(LispPtr lisp);
void gc_info(LispPtr lisp);