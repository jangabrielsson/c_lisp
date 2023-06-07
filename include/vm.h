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

  Ptr runCode(Instr *code);