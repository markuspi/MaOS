
; macro for functions that take one float parameter
; first argument is c-function name, second argument is instruction
%macro FLOAT_FUNC1 2
global %1
%1:
    fld dword [esp+4]
    %2
    ret
%endmacro


FLOAT_FUNC1 fsin, fsin
FLOAT_FUNC1 fcos, fcos
FLOAT_FUNC1 fsqrt, fsqrt
FLOAT_FUNC1 fabs, fabs
