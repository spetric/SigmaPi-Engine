.x64
PUBLIC fast_ftol
.CODE
_TEXT SEGMENT
fast_ftol PROC NEAR
      cvtss2si eax, xmm0     ; SSE convert float to integer
      ret
fast_ftol ENDP
_TEXT ENDS
END


