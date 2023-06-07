union Expr;
typedef union Expr Expr;
typedef Expr *Ptr;

typedef struct LispStruct {
    Ptr NIL;
    Ptr T;
} *LispPtr;

