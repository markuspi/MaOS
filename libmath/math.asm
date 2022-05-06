
; macro for functions that take one float parameter
; first argument is c-function name, second argument is instruction
%macro FLOAT_FUNC1 2
global %1
%1:
    fld dword [esp+4]
    %2
    ret
%endmacro

; macro for functions that take two float parameters
; first argument is c-function name, second argument is instruction
%macro FLOAT_FUNC2 2
global %1
%1:
    fld dword [esp+8]
    fld dword [esp+4]
    %2
    ret
%endmacro

FLOAT_FUNC2 fmod, fprem
FLOAT_FUNC1 fsin, fsin
FLOAT_FUNC1 fcos, fcos
FLOAT_FUNC1 fsqrt, fsqrt
FLOAT_FUNC1 fabs, fabs
