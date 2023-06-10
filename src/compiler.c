#include "clisp.h"


// Ptr compile_aux(LispPtr lisp, Ptr expr) {
//     switch (TYPE(expr)) {
//         case ATOM_TYPE:
//             INSTR(PUSH_ATOM_VALUE,1,expr);
//             break
//         case INT_TYPE:
//             INSTR(PUSH_INT_VALUE,1,expr);
//             break;
//         case CONS_TYPE:
//             Ptr car = expr->cons.car;
//             Ptr cdr = expr->cons.cdr;
//             int n = 0;
//             while (cdr != lisp->NIL && TYPE(cdr) == CONS_TYPE) {
//                 compile_aux(lisp, cdr->cons.car);
//                 cdr = cdr->cons.cdr;
//                 n += 1
//             }
//             switch (TYPE(car)) {
//                 case ATOM_TYPE:
//                     INSTR(PUSH_CALL_ATOM,n,car);
//                     break;
//                 case CONS_TYPE:
//                     compile_aux(lisp, car);
//                     break;
//                 default:
//                     return lisp->NIL;
//             }
//             break;
//         default:
//             break;
//     }
// }

// Ptr compile(LispPtr lisp, Ptr expr) {
//     return compile_aux(lisp, expr);
// }