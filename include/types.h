typedef unsigned long ulong;
typedef unsigned int uint;

union Expr;
typedef union Expr Expr;
typedef Expr *Ptr;

void print(Ptr p);

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
    Ptr (*fun)(Ptr *stack);
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

Ptr mkCons(Ptr car, Ptr cdr);
Ptr mkAtom(char *name, Ptr value);

Ptr NIL;
Ptr T;