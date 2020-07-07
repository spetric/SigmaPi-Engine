.586
.model FLAT
.DATA
DSEG SEGMENT PARA
result dd 0 ;(in the data section)
DSEG ENDS
PUBLIC _fast_ftol
.CODE
_TEXT SEGMENT
_d$ = 4
_fast_ftol PROC NEAR
      fld   DWORD PTR _d$[esp]
      fistp DWORD PTR result
      mov   eax , DWORD PTR result
      ret 0
_fast_ftol ENDP
_TEXT ENDS
END



