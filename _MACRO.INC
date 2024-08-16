macro _invoke proc,[arg]
       { 
        common 
        stdcall [proc],arg 
       } 



struc db [data]
       {
         common
         label .data byte
         db data
         .size = $-1-.data
       }



struc rb [data]
       {
         common
         label .data byte
         rb data
         .size = $-.data
       } 














macro proc [args]                       ; define procedure
 { common
    match name params, args>
    \{ define@proc name,<params \} }

prologue@proc equ prologuedef

macro prologuedef procname,flag,parmbytes,localbytes,reglist
 { local loc
   loc = (localbytes+3) and (not 3)
   parmbase@proc equ ebp+8
   localbase@proc equ ebp-loc
   if parmbytes | localbytes
    ;db 0x8D,0x80,0x00,0x00,0x00,0x00   ;mov edi,edi
         db 0x05,0x00,0x00,0x00,0x00;,0x90
    push ebp
    mov ebp,esp
    if localbytes
     sub esp,loc
    end if
   end if
   irps reg, reglist \{ push reg \} }

epilogue@proc equ epiloguedef





macro push_all
       { 
        pushad
       } 


macro pop_all
       { 
        mov     [esp+0x1C],eax
        popad
       } 
           
           
macro .continue
       { 
        jmp     __WHILE
       }    
           
    
           
           
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                              ;;
;; Copyright (C) KolibriOS team 2004-2007. All rights reserved. ;;
;; Distributed under terms of the GNU General Public License    ;;
;;                                                              ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;------------------------------------------------------------------
; use "iglobal" for inserting initialized global data definitions.
;------------------------------------------------------------------
macro iglobal {
  IGlobals equ IGlobals,
  macro __IGlobalBlock { }

macro iglobal_nested {
  IGlobals equ IGlobals,
  macro __IGlobalBlock \{ }

;-------------------------------------------------------------
; use 'uglobal' for inserting uninitialized global definitions.
; even when you define some data values, these variables
; will be stored as uninitialized data.
;-------------------------------------------------------------
macro uglobal {
  UGlobals equ UGlobals,
  macro __UGlobalBlock { }

macro uglobal_nested {
  UGlobals equ UGlobals,
  macro __UGlobalBlock \{ }

endg fix }      ; Use endg for ending iglobal and uglobal blocks.
endg_nested fix \}

macro IncludeIGlobals{
  macro IGlobals dummy,[n] \{ __IGlobalBlock
     purge __IGlobalBlock  \}
  match I, IGlobals \{ I \} }


macro IncludeUGlobals{
  macro UGlobals dummy,[n] \{
    \common
      \local begin, size
      begin = $
      virtual at $
    \forward
      __UGlobalBlock
      purge __UGlobalBlock
    \common
      size = $ - begin
    end virtual
    rb size
  \}
  match U, UGlobals \{ U \} }

macro IncludeAllGlobals {
  IncludeIGlobals
  IncludeUGlobals
}
 
iglobal
endg

uglobal
endg


;****************************************************************************************************
proc gnome_sort lpArray:DWORD, dLen:DWORD
        push_all
 
        ; В массиве меньше 2 элементов?
        cmp     [dLen],2
        jb      .loc_ret
 
        mov     esi,[lpArray]
        xor     ecx,ecx
        inc     ecx
        mov     edx,ecx
.loc_loop:
        or      ecx,ecx
        jz      @f
        mov     eax,[esi+ecx*4-4]
        mov     ebx,[esi+ecx*4]
        cmp     eax,ebx
        ; Для сортировки массива по убыванию замените
        ; условный переход JBE на JAE
        jbe     @f
 
        ; Поменять местами соседние элементы
        mov     [esi+ecx*4-4],ebx
        mov     [esi+ecx*4],eax
        dec     ecx
        jmp     .loc_loop
@@:
        ; Перейти на крайний неотсортированный элемент
        inc     edx
        mov     ecx,edx
        cmp     edx,[dLen]
        jb      .loc_loop
.loc_ret:
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    _lstrcmpiw,lpStr1:DWORD,lpStr2:DWORD
        push_all
        mov     esi,[lpStr1]
        mov     edi,[lpStr2]
_while:
        mov     al,[esi]
        mov     ah,[edi]
        or      eax,eax
        je      .ret
        and     eax,0x0DFDF
        cmp     al,ah
        jb      .min
        ja      .max
        inc     edi
        inc     esi
        jmp     _while
.max:
        mov     eax,1
        jmp     .ret
.min:
        or      eax,-1
.ret:
        pop_all
        ret
endp 
;****************************************************************************************************

;****************************************************************************************************
proc _lstrcmpi lpStr1:DWORD,lpStr2:DWORD
        push_all
 
        mov     esi,[lpStr1]    ; ????????? ?? ??????
        mov     edi,[lpStr2]
        xor     eax,eax         ; ???????????, ??? ?????? ?????
.loc_compare:
        lodsb                   ; ???????? ????????? ???????
        mov     ah,[edi]
        inc     edi
 
        cmp     al,ah           ; ??????? ??????????
        jne     @f              ; ???
 
        or      al,al           ; ?????? ????????????
        jz      .loc_ret        ; ??, ?????? ?????????
        jmp     .loc_compare    ; ????????? ????????? ??????
@@:
        and     eax,0DFDFh      ; ??? ??????? ? ??????? ???????
        cmp     al,'A'
        jb      .loc_not_equal
        cmp     al,'Z'
        ja      .loc_not_equal
 
        cmp     al,ah           ; ??????? ???? ?????? ? ?????????
        je      .loc_compare    ; ??, ????????? ??????
.loc_not_equal:
        xor     eax,eax         ; ?????? ?? ?????????
        inc     eax    
.loc_ret:
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc _lstrcmp,lpStrA:dword,lpStrB:dword
     push_all
     mov     edi,[lpStrA]
     mov     esi,[lpStrB]
     xor     eax,eax
     xor     edx,edx
     mov     ecx,-1
.lstrcmp_loop:
     inc     ecx
     mov     dl,[esi+ecx]
     cmp     [edi+ecx],dl
     jne     .lstrcmp_ret
     or      edx,edx
     jne     .lstrcmp_loop
.lstrcmp_ok:
     xchg    eax,ecx
.lstrcmp_ret:
     pop_all
     ret
endp
;***************************************************************************************************************  

;***************************************************************************************************************
proc _lstrcpy_zero,lpStrA:dword,lpSizeA:dword,lpStrB:dword
     push_all
     mov     edi,[lpStrA]
     mov     esi,[lpStrB]
     mov     ebx,[lpSizeA]
     mov     ecx,-1
.lstrcpy_loop:
     inc     ecx
     mov     al,[esi+ecx]
     mov     [edi+ecx],al
     or      al,al
     jne     .lstrcpy_loop
     mov     [lpSizeA],ecx
.lstrcpy_loop_zero:
     mov     byte[edi+ecx],al
     inc     ecx
     cmp     ecx,ebx
     jna     .lstrcpy_loop_zero
.lstrcpy_ret:
     mov   eax,[lpSizeA]
     pop_all
     ret
endp
;*************************************************************************************************************** 

;*************************************************************************************************************** 
proc _lstrcat lpDst:DWORD, lpSrc:DWORD
        push_all
        mov     esi,[lpSrc]
        mov     edi,[lpDst]
        xor     eax,eax
        or      esi,esi
        je      .scan_api_ret
        or      edi,edi
        je      .scan_api_ret
.lstcat_len:
        mov     al,[edi]
        add     edi,1
        or      al,al
        jne     .lstcat_len
        sub     edi,1           
.lstcat_copy:
        mov     al,[esi]
        mov     [edi],al
        add     esi,1
        add     edi,1
        or      al,al
        jne     .lstcat_copy
        sub     edi,1
.lstcat_ok:
        sub     edi,[lpDst]
        mov     eax,edi
.scan_api_ret:
        pop_all
        ret
endp
;*************************************************************************************************************** 

;***************************************************************************************************************
proc _lstcpy,lpStrA:dword,lpStrB:dword
     push_all
     mov     edi,[lpStrA]
     mov     esi,[lpStrB]
     mov     ecx,-1
.lstrcpy_loop:
     inc     ecx
     mov     al,[esi+ecx]
     mov     [edi+ecx],al
     or      al,al
     jne     .lstrcpy_loop
.lstrcpy_ret:
     mov eax,ecx
     pop_all
     ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc _lstrlen,lpStr:dword
        push_all
        mov             edi,[lpStr]
        xor             eax,eax
        cld
        or              ecx,-1
        repne scasb
        not             ecx
        dec             ecx
        mov             eax,ecx
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc StrToHexEAX lpStr:dword
                push_all
        xor     eax,eax
        mov     esi,[lpStr]
.str2hex_loop:
        movsx   ebx,byte [esi]
        cmp     bl,'x'
        je      .str2hex_next
                or      bl,20h
        cmp     bl,'0'
        jb      .str2hex_ret
        cmp     bl,'9'
        ja      .str2hex_1
        sub     bl,'0'
        jmp     .str2hex_calc
.str2hex_1:
        cmp     bl,'a'
        jb      .str2hex_ret
        cmp     bl,'f'
        ja      .str2hex_ret
        sub     bl,('a'-10)
.str2hex_calc:
        imul    eax,16
        add     eax,ebx
.str2hex_next:        
                inc     esi
        jmp     .str2hex_loop
.str2hex_ret:
        pop_all
        ret
endp  
;***************************************************************************************************************

;***************************************************************************************************************
proc StrToDexEAX lpStr:dword
        push_all
        xor     eax,eax
        mov     esi,[lpStr]
.str2dec_loop:
        movsx   ebx,byte [esi]
        sub     bl,'0'
        cmp     bl,10
        jnb     .str2dec_ret
        imul    eax,10
        add     eax,ebx
        inc     esi
        jmp     .str2dec_loop
.str2dec_ret:
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    HexToTwo,lpDst:DWORD, lpSrc:DWORD, dSize:DWORD                        ;
        push_all
        mov     esi,[lpSrc]
        mov     edi,[lpDst]
        xor     ecx,ecx
@@:
        mov     al,[esi+ecx]
        mov     ah,al
        and     eax,0xFF0
        shr     al,4
        mov     [edi+ecx*2],ax
        inc     ecx
        cmp     ecx,[dSize]
        jb      @b
        shl     ecx,1
        mov     eax,ecx
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    HexToOne,lpDst:DWORD, lpSrc:DWORD, dSize:DWORD                        ;
        push_all
        mov     esi,[lpSrc]
        mov     edi,[lpDst]
        xor     ecx,ecx
@@:
        mov     ax,[esi+ecx*2]
        shl     al,04
        add     al,ah
        mov     [edi+ecx],al
        inc     ecx
        cmp     ecx,[dSize]
        jb      @b
        shr     ecx,1
        mov     eax,ecx
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc HexToStrLen lpDst:DWORD, lpSrc:DWORD, dSize:DWORD
        push_all
        mov     esi,[lpSrc]
        mov     edi,[lpDst]
        xor     ecx,ecx
@@:
        mov     eax,[esi+ecx]
        mov     ebx,eax
        shr     eax,4
        and     ebx,15
        and     eax,15
        mov     al,[.table+eax]
        mov     ah,[.table+ebx]
        mov     [edi+ecx*2],ax
        inc     ecx
        cmp     ecx,[dSize]
        jb      @b
        xor     eax,eax
        mov     [edi+ecx*2],eax
        add     eax,[dSize]
        shl     eax,1
        pop_all
        ret
.table   db '0123456789ABCDEF'
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    StrToHexLen lpDst:DWORD, lpSrc:DWORD, dSize:DWORD
        push_all
        mov     edi,[lpDst]
        mov     esi,[lpSrc]
        xor     ecx,ecx
        cmp     ecx,[dSize]
        je      .loc_ret
        mov     edx,ecx
.loc_scan:
        mov     al,[esi+ecx]
        cmp     al,'0'
        jb      .loc_ret
        cmp     al,'9'
        jbe     .loc_a
        and     al,0xDF
        cmp     al,'A'
        jb      .loc_ret
        cmp     al,'F'
        ja      .loc_ret
        sub     al,('A'-10)
.loc_a:
        and     al,0x0F
        test    ecx,1
        jne     .loc_b
        shl     al,4
        mov     [edi+edx],al
        jmp     .loc_next
.loc_b:
        add     [edi+edx],al
        inc     edx
.loc_next:
        inc     ecx
        cmp     ecx,[dSize]
        jb     .loc_scan
.loc_ret:
        mov     eax,ecx
        pop_all
        ret
endp 
;***************************************************************************************************************

;***************************************************************************************************************
proc HexToStrEAXCnt,lpStr:dword,Cnt:dword
        push_all
        mov     edi,[lpStr]
        mov     ebx,[Cnt]
        mov     ecx,[Cnt]
        jecxz   .hex_to_str_end
.hex_to_str:
        push    eax
        and     al,0Fh
        daa
        add     al,0F0h
        adc     al,40h
        mov     [edi+ecx-1],al
        pop     eax
        ror     eax,4
        loop    .hex_to_str
.hex_to_str_end:
        add     edi,[Cnt]
        mov     al,0
        stosb
        pop_all
        ret
endp 
;***************************************************************************************************************

;***************************************************************************************************************
proc HexToStrEAX,lpStr:dword
        push_all
        mov     edi,[lpStr]
        mov     ecx,8
.hex_to_str:
        rol     eax,4
        push    eax
        and     al,0Fh
        daa
        add     al,0F0h
        adc     al,40h
        stosb
        pop     eax
        loop    .hex_to_str
        mov     al,0
        stosb
        pop_all
        ret
endp 
;***************************************************************************************************************

;***************************************************************************************************************
proc HexToStrEAX2,lpStr:dword
iglobal
        pHexToStr       db 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
endg
        push_all
        mov     edi,[lpStr]
        mov     ecx,8
.hex_to_str:
        rol     eax,4
        push    eax
        and     eax,0Fh
        movzx   eax,byte[pHexToStr+eax]
        stosb
        pop     eax
        loop    .hex_to_str
        mov     al,0
        stosb
        pop_all
        ret
endp 
;***************************************************************************************************************
                                    
;***************************************************************************************************************
;-----------------------------------------------------
; ??????? ???????? ??????????? ??????? ??????
;-----------------------------------------------------
; lpDst - ????????? ?? ????????
; lpSrc - ????????? ?? ????????
; dSize - ?????? ??????????? ?????
;-----------------------------------------------------
proc    _RtlCopyMemory lpDst:DWORD, lpSrc:DWORD, dSize:DWORD
        push_all
 
        ; ?????????? ????????? ?? ???????? ? ????????
        cld
        mov     edi,[lpDst]
        mov     esi,[lpSrc]
 
        mov     ecx,[dSize]
        push    ecx
        ; ????????? ?? 4 ? ???????? ????? ? DWORD
        shr     ecx,2
        ; ??????????? ???????? ????? ?????? DWORD'???
        rep     movsd
        pop     ecx
        ; ???????? ??????? ?? ??????? ?? 4
        and     ecx,3
        ; ??????????? ??????? ?????? ???????
        rep     movsb
                
                mov     eax,[dSize]
                pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    _RtlCompareMemory lpDst:DWORD, lpSrc:DWORD, dSize:DWORD
        push_all
 
        ; ?????????? ????????? ?? ???????? ? ????????
        xor     eax,eax
        mov     edi,[lpDst]
        mov     esi,[lpSrc]
        cld

        mov     ecx,[dSize]
        push    ecx
        ; ????????? ?? 4 ? ???????? ????? ? DWORD
        shr     ecx,2
        ; ??????????? ???????? ????? ?????? DWORD'???
        rep     cmpsd
        setne   al
        pop     ecx
        ; ???????? ??????? ?? ??????? ?? 4
        and     ecx,3
        ; ??????????? ??????? ?????? ???????
        rep     cmpsb
        setne   ah

        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    _RtlZeroMemory lpDst:DWORD, dSize:DWORD
        push_all
 
        ; ?????????? ????????? ?? ???????? ? ????????
        cld
        xor     eax,eax
        mov     edi,[lpDst]
        mov     ecx,[dSize]
        push    ecx
        ; ????????? ?? 4 ? ???????? ????? ? DWORD
        shr     ecx,2
        ; ??????????? ???????? ????? ?????? DWORD'???
        rep     stosd
        pop     ecx
        ; ???????? ??????? ?? ??????? ?? 4
        and     ecx,3
        ; ??????????? ??????? ?????? ???????
        rep     stosb
                
        mov     eax,[dSize]
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    _RtlFillMemory lpDst:DWORD, dSize:DWORD, dFill:DWORD
        push_all

        imul    eax,[dFill],0x01010101
        mov     edi,[lpDst]
        mov     ecx,[dSize]
        cld


        push    ecx
        ; ????????? ?? 4 ? ???????? ????? ? DWORD
        shr     ecx,2
        ; ??????????? ???????? ????? ?????? DWORD'???
        rep     stosd
        pop     ecx
        ; ???????? ??????? ?? ??????? ?? 4
        and     ecx,3
        ; ??????????? ??????? ?????? ???????
        rep     stosb
                
        mov     eax,[dSize]
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    DecToHexDword

 
push ebp
 
mov ebp,esp
 
push 0
 
push 0
 
push 0
;????????? ?????????? ? 10 ???? (10+2 ??? ????????) ?????? 0
 
mov eax,[ebp+08h]
 
 
mov [ebp-0ch],eax
;????????? ? ??????? ????? ???? 10-??? ?????
 
fbld [ebp-0Ch]
;???????? ??? ???????????
 
fistp dword  [ebp-04h]
;????????? ?????????? ????? ??? 16-???
 
mov eax,[ebp-04h]
;?????????? ???
 
leave
 
 
ret 4

endp
;***************************************************************************************************************


;***************************************************************************************************************
proc    HexToDecDword

 
 
push ebp
 
 
mov ebp,esp
 
 
sub esp,0ch
;????????? ?????????? ? 10 ???? (10+2 ??? ????????)
 
fild dword  [ebp+08h]
;????????? ? ??????? ????? ???? 16-??? ?????
 
fbstp [ebp-0ch]
;????????? ?????????? ????? ??? 10-???
 
mov eax,[ebp-0ch]
;?????????? ???
 
leave
 
 
ret 4

endp                      
;***************************************************************************************************************

;***************************************************************************************************************
proc    CalcEntropyMod pDATA:DWORD,pSize:DWORD,pChar:DWORD,pCount:DWORD
        push_all
        xor     ecx,ecx
        mov     eax,ecx
        mov     edi,[pCount]
        mov     esi,[pChar]
        mov     edx,[pDATA]
.loop_zero:
        mov     [edi+ecx*4],eax
        mov     [esi+ecx*4],ecx
        inc     ecx
        cmp     ecx,256
        jne     .loop_zero
        xor     ecx,ecx
.loop_char:
        movzx   eax,byte[edx+ecx]
        inc     dword[edi+eax*4]
        inc     ecx
        cmp     ecx,[pSize]
        jb      .loop_char
        stdcall shell_sort_neg,edi,esi,256
        xor     ecx,ecx
.loop_count:
        mov     eax,[edi+ecx*4]
        or      eax,eax
        je      .loop_end
        inc     ecx
        cmp     ecx,256
        jne     .loop_count
        dec     ecx
.loop_end:
        mov     eax,ecx
        pop_all
        ret
endp

;***************************************************************************************************************

;***************************************************************************************************************
proc    CalcEntropyBit szStr:DWORD,lpRes:DWORD
 locals
        cnt     rd 256         
 endl
        push_all
        xor       ebx,ebx
        lea     edi,[cnt]
        mov     ecx,256
        xor     eax,eax
        repne   stosd
        lea     edi,[cnt]
        mov     esi,[szStr]
        xor     ecx,ecx
.looopb:
        movzx     eax,byte[esi+ecx]
        and       al,0x0F
        inc       dword[edi+eax*4]
        movzx     eax,byte[esi+ecx]
        and       al,0xF0
        shr       al,4
        inc       dword[edi+eax*4]
        inc       ecx
        cmp       ecx,[lpRes]
        jb        .looopb
        xor       ecx,ecx
.looopcnt:
        cmp       [edi+ecx*4],ebx
        jbe       @f
        mov               ebx,[edi+ecx*4]
        mov               edx,ecx
@@:
        inc       ecx
        cmp       ecx,256
        jne       .looopcnt
        mov       eax,[edi+edx*4]
        pop_all
        ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc    CalcEntropy szStr:DWORD,lpRes:DWORD
 locals
        cnt     rd 256          ; Ñ÷åò÷èêè ñèìâîëîâ
 endl
        push_all
        xor     ebx,ebx
        lea     edi,[cnt]
        mov     ecx,256
        xor     eax,eax
        repne   stosd
        lea     edi,[cnt]
        mov     esi,[szStr]
        xor     ecx,ecx
.looopb:
        movzx     eax,byte[esi+ecx]
        inc       dword[edi+eax*4]
        inc       ecx
        cmp       ecx,[lpRes]
        jb        .looopb
        xor       ecx,ecx
.looopcnt:
        cmp       [edi+ecx*4],ebx
        jbe       @f
        mov       ebx,[edi+ecx*4]
        mov       edx,ecx
@@:
        inc       ecx
        cmp       ecx,256
        jne       .looopcnt
        mov       eax,edx
        pop_all
        ret
endp

;*************************************************************************************************************** 

;****************************************************************************************************
proc zscanmem SRCdata:dword, SRCsize:dword, PTRdataMax:dword,PTRdataMin:dword,\
             PTRsize:dword
        push_all
        mov     esi,[PTRdataMax]
        mov     edi,[PTRdataMin]
        mov     edx,[SRCdata]
        add     [SRCsize],edx
        xor     ecx,ecx
.scanmem_loop:
        mov     ebx,edx
.scanmem_scan:
        cmp     edx,[SRCsize]
        je      .scanmem_not_found
        mov     al,[edx]
.scanmem_scanz:
        cmp     [esi+ecx],al
        jb      .scanmem_not_next
        cmp     [edi+ecx],al
        ja      .scanmem_not_next
        inc     ecx
        cmp     ecx,[PTRsize]
        je      .scanmem_found
        inc     edx
        jmp     .scanmem_scan
.scanmem_not_next:
        xor     ecx,ecx
        mov     edx,ebx
        inc     edx
        jmp     .scanmem_loop
.scanmem_not_found:
        xor      ebx,ebx
.scanmem_found:
        mov     eax,ebx
.scanmem_ret:
        pop_all
        ret
endp
;**************************************************************************************************** 

;*************************************************************************************************************** 
; ---------------------------------------------
; Процедура поиска строки в блоке памяти
; (C) ManHunter / PCL
; ---------------------------------------------
; SRCdata - блок памяти в котором выполняется поиск
; SRCsize - размер блока в котором выполняется поиск
; PTRdata - строка для поиска
; PTRsize - длина строки для поиска
; MSKdata - бинарная маска для поиска или 0 если не используется
;
; Возврат: EAX = offset найденной строки
;          EAX = 0 если ничего не найдено
; ---------------------------------------------
 
proc scanmem SRCdata:dword, SRCsize:dword, PTRdata:dword,\
             PTRsize:dword, MSKdata:dword
 
        push_all
 
        ; Длина паттерна больше длины данных?
        mov     eax,[PTRsize]
        cmp     eax,[SRCsize]
        ; Да, возврат -1
        ja      .scanmem_not_found
 
        mov     esi,[SRCdata]
        mov     edi,[PTRdata]
        mov     edx,[MSKdata]
        mov     ebx,esi
        add     ebx,[SRCsize]
        sub     ebx,[PTRsize]
.scanmem_loop:
        xor     ecx,ecx
.scanmem_test_char:
        or      edx,edx
        jz      .scanmem_no_mask
        cmp     byte [edx+ecx],0
        jz      .scanmem_char_equal
 
.scanmem_no_mask:
        mov     al,[esi+ecx]
        cmp     al,[edi+ecx]
        jne     .scanmem_next_pattern
.scanmem_char_equal:
        inc     ecx
        cmp     ecx,[PTRsize]
        jb      .scanmem_test_char
        jmp     .scanmem_found
.scanmem_next_pattern:
        inc     esi
        cmp     esi,ebx
        jbe     .scanmem_loop
 
.scanmem_not_found:
        ; Строка не найдена
        xor             esi,esi
 
.scanmem_found:
        ; Строка найдена
        mov     eax,esi
 
.scanmem_ret:
        pop_all
 
        ret
endp
;*************************************************************************************************************** 

;****************************************************************************************************
proc    BinToHex
        push_all
        xor     ebx,ebx
        mov     ecx,8
.BinToHex:
        shr     ebx,1
        test    eax,1
        je      @f
        or      ebx,0x80000000
        @@:
        shr     eax,04
        dec     ecx
        jne     .BinToHex
        rol     ebx,8
        mov     eax,ebx
        pop_all
        ret
endp
;****************************************************************************************************


;****************************************************************************************************
proc    HexToBin lpHex:DWORD
        push_all
        xor     ecx,ecx
                mov     ebx,[lpHex]
                xor     ecx,ecx
                xor     eax,eax
                while_z:
                rol     eax,4
                test    ebx,0x80
                je      @f
                add     al,1
                @@:
                inc     ecx
                shl     ebx,1
                cmp     ecx,8
                jb  while_z
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc TEST_33 lpDst:DWORD, lpSrc:DWORD, dSize:DWORD
        push_all
iglobal
        LByte   db      0x00,0x01,0x02,0x03,\
                        0x10,0x11,0x12,0x13,\
                        0x20,0x21,0x22,0x23,\
                        0x30,0x31,0x32,0x33

endg
        mov     edi,[lpDst]
        mov     esi,[lpSrc]
        xor     ecx,ecx
        xor     edx,edx
        xor     eax,eax
@@:
        mov     al,[esi+ecx]
        and     al,0xF0
        shr     al,4
        mov     al,[eax+LByte]
        mov     [edi+edx],al
        inc     edx
        mov     al,[esi+ecx]
        and     al,0x0F
        mov     al,[eax+LByte]
        mov     [edi+edx],al
        inc     edx
        inc     ecx
		cmp		ecx,[dSize]
		jb		@b
        mov     eax,edx
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc _TEST_33 lpDst:DWORD, lpSrc:DWORD, dSize:DWORD

iglobal
        pTable33        db\
0x00,0x01,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x04,0x05,0x06,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x08,0x09,0x0A,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x0C,0x0D,0x0E,0x0F
endg


        push_all
        mov     esi,[lpSrc]
        mov     edi,[lpDst]
        xor     ecx,ecx
        xor     edx,edx
        xor     eax,eax
@@:
        movzx   eax,byte[esi+ecx]
        movzx   eax,byte[pTable33+eax]
        shl     eax,4
        inc     ecx
        movzx   ebx,byte[esi+ecx]
        movzx   ebx,byte[pTable33+ebx]
        add     eax,ebx
        inc     ecx
        mov     [edi+edx],al
        inc     edx
                cmp             ecx,[dSize]
                jb              @b
        mov     eax,edx
.ret:
        pop_all
        ret

endp
;**************************************************************************************************** 

;****************************************************************************************************
proc    WIRandom rmin:dword,rmax:dword 
locals
        random_seed     dd      0
endl
        push_all
        rdtsc
        xor     eax,edx 
        mov     [random_seed],eax
        mov     ecx,[rmax]
        sub     ecx,[rmin] 
        inc     ecx 
        stdcall WRandomEx,[random_seed]
        xor     edx,edx 
        div     ecx 
        mov     eax,edx 
        add     eax,[rmin] 
        pop_all
        ret
WRandomEx:
        push    edx ecx
        mov     eax,[random_seed]
        or      eax,eax 
        jnz     @f
        rdtsc 
        xor     eax,edx 
        mov     [random_seed],eax 
        @@:
        xor     edx,edx 
        mov     ecx,127773 
        div     ecx 
        mov     ecx,eax 
        mov     eax,16807 
        mul     edx 
        mov     edx,ecx 
        mov     ecx,eax 
        mov     eax,2836 
        mul     edx 
        sub     ecx,eax 
        xor     edx,edx 
        mov     eax,ecx 
        mov     [random_seed],ecx 
        mov     ecx,100000 
        div     ecx 
        mov     eax,edx 
        pop     ecx edx 
        ret     4
endp
;**************************************************************************************************** 

;**************************************************************************************************** 
proc    Murmur2 lpData:DWORD, dSize:DWORD, dSeed:DWORD 
        push_all 
  
        MAGIC = 0x5BD1E995 
  
        mov     esi,[lpData] 
        mov     ebx,[dSize] 
  
        mov     ecx,[dSeed] 
        xor     ecx,ebx 
  
.loc_loop: 
        cmp     ebx,4 
        jb      .loop_done 
  
        imul    ecx,MAGIC 
  
        movzx   edx,byte [esi+1] 
        movzx   eax,byte [esi] 
        shl     edx,8 
        or      eax,edx 
        movzx   edx,byte [esi+2] 
        shl     edx,16 
        or      eax,edx 
        movzx   edx,byte [esi+3] 
        shl     edx,24 
        or      eax,edx 
  
        imul    eax,MAGIC 
        mov     edx,eax 
        shr     edx,24 
        xor     eax,edx 
        imul    eax,MAGIC 
  
        xor     ecx,eax 
  
        add     esi,4 
        sub     ebx,4 
        jmp     .loc_loop 
  
.loop_done: 
        cmp     ebx,3 
        je      .loc_tail_3 
        cmp     ebx,2 
        je      .loc_tail_2 
        cmp     ebx,1 
        je      .loc_tail_1 
        jmp     .loc_finish 
  
.loc_tail_3: 
        movzx   eax,byte[esi+2] 
        shl     eax,16 
        xor     ecx,eax 
.loc_tail_2: 
        movzx   eax,byte[esi+1] 
        shl     eax,8 
        xor     ecx,eax 
.loc_tail_1: 
        movzx   eax,byte[esi] 
        xor     ecx,eax 
        imul    ecx,MAGIC 
  
.loc_finish: 
        mov     eax,ecx 
        shr     eax,13 
        xor     ecx,eax 
  
        imul    ecx,MAGIC 
  
        mov     eax,ecx 
        shr     eax,15 
        xor     eax,ecx 
  
        pop_all
        ret 
endp
;****************************************************************************************************

;****************************************************************************************************
proc    Murmur3 lpData:DWORD, dSize:DWORD, dSeed:DWORD, pOut:DWORD

        locals
                h1 dd ?
                h2 dd ?
                h3 dd ?
                h4 dd ?
        endl

        push_all

        MAGIC1 = 0x239B961B
        MAGIC2 = 0xAB0E9789
        MAGIC3 = 0x38B34AE5
        MAGIC4 = 0xA1E38B93

        mov     ebx,[dSize]
        mov     ecx,[dSeed]

        mov     [h1],ecx
        mov     [h2],ecx
        mov     [h3],ecx
        mov     [h4],ecx

        mov     esi,[lpData]

.loc_loop:
        cmp     ebx,16
        jb      .loop_done

        mov     eax,dword [esi+4*0]
        imul    eax,MAGIC1

        mov     edx,eax
        shr     edx,17
        shl     eax,15
        or      eax,edx

        imul    eax,MAGIC2
        xor     [h1],eax

        mov     eax,[h1]
        mov     edx,eax
        shr     edx,13
        shl     eax,19
        or      eax,edx
        add     eax,[h2]
        imul    eax,5
        add     eax,0x561CCD1B
        mov     [h1],eax

        mov     eax,dword [esi+4*1]
        imul    eax,MAGIC2

        mov     edx,eax
        shr     edx,16
        shl     eax,16
        or      eax,edx

        imul    eax,MAGIC3
        xor     [h2],eax

        mov     eax,[h2]
        mov     edx,eax
        shr     edx,15
        shl     eax,17
        or      eax,edx
        add     eax,[h3]
        imul    eax,5
        add     eax,0x0BCAA747
        mov     [h2],eax

        mov     eax,dword [esi+4*2]
        imul    eax,MAGIC3

        mov     edx,eax
        shr     edx,19
        shl     eax,17
        or      eax,edx

        imul    eax,MAGIC4
        xor     [h3],eax

        mov     eax,[h3]
        mov     edx,eax
        shr     edx,17
        shl     eax,15
        or      eax,edx
        add     eax,[h4]
        imul    eax,5
        add     eax,0x96CD1C35
        mov     [h3],eax

        mov     eax,dword [esi+4*3]
        imul    eax,MAGIC4

        mov     edx,eax
        shr     edx,14
        shl     eax,18
        or      eax,edx

        imul    eax,MAGIC1
        xor     [h4],eax

        mov     eax,[h4]
        mov     edx,eax
        shr     edx,19
        shl     eax,13
        or      eax,edx
        add     eax,[h1]
        imul    eax,5
        add     eax,0x32AC3B17
        mov     [h4],eax

        add     esi,16
        sub     ebx,16
        jmp     .loc_loop

.loop_done:
        xor     edx,edx

        cmp     ebx,15
        je      .loc_tail_15
        cmp     ebx,14
        je      .loc_tail_14
        cmp     ebx,13
        je      .loc_tail_13
        cmp     ebx,12
        je      .loc_tail_12
        cmp     ebx,11
        je      .loc_tail_11
        cmp     ebx,10
        je      .loc_tail_10
        cmp     ebx,9
        je      .loc_tail_9
        cmp     ebx,8
        je      .loc_tail_8
        cmp     ebx,7
        je      .loc_tail_7
        cmp     ebx,6
        je      .loc_tail_6
        cmp     ebx,5
        je      .loc_tail_5
        cmp     ebx,4
        je      .loc_tail_4
        cmp     ebx,3
        je      .loc_tail_3
        cmp     ebx,2
        je      .loc_tail_2
        cmp     ebx,1
        je      .loc_tail_1
        jmp     .loc_finish

.loc_tail_15:
        movzx   edx,byte[esi+14]
        shl     edx,16
.loc_tail_14:
        movzx   eax,byte[esi+13]
        shl     eax,8
        xor     edx,eax
.loc_tail_13:
        movzx   eax,byte[esi+12]
        xor     edx,eax
        imul    edx,MAGIC4
        mov     eax,edx
        shr     eax,14
        shl     edx,18
        or      edx,eax
        imul    edx,MAGIC1
        xor     [h4],edx

.loc_tail_12:
        movzx   edx,byte[esi+11]
        shl     edx,24
.loc_tail_11:
        movzx   eax,byte[esi+10]
        shl     eax,16
        xor     edx,eax
.loc_tail_10:
        movzx   eax,byte[esi+9]
        shl     eax,8
        xor     edx,eax
.loc_tail_9:
        movzx   eax,byte[esi+8]
        xor     edx,eax
        imul    edx,MAGIC3
        mov     eax,edx
        shr     eax,15
        shl     edx,17
        or      edx,eax
        imul    edx,MAGIC4
        xor     [h3],edx

.loc_tail_8:
        movzx   edx,byte[esi+7]
        shl     edx,24
.loc_tail_7:
        movzx   eax,byte[esi+6]
        shl     eax,16
        xor     edx,eax
.loc_tail_6:
        movzx   eax,byte[esi+5]
        shl     eax,8
        xor     edx,eax
.loc_tail_5:
        movzx   eax,byte[esi+4]
        xor     edx,eax
        imul    edx,MAGIC2
        mov     eax,edx
        shr     eax,16
        shl     edx,16
        or      edx,eax
        imul    edx,MAGIC3
        xor     [h2],edx

.loc_tail_4:
        movzx   edx,byte[esi+3]
        shl     edx,24
.loc_tail_3:
        movzx   eax,byte[esi+2]
        shl     eax,16
        xor     edx,eax
.loc_tail_2:
        movzx   eax,byte[esi+1]
        shl     eax,8
        xor     edx,eax
.loc_tail_1:
        movzx   eax,byte[esi+0]
        xor     edx,eax
        imul    edx,MAGIC1
        mov     eax,edx
        shr     eax,17
        shl     edx,15
        or      edx,eax
        imul    edx,MAGIC2
        xor     [h1],edx

.loc_finish:
        mov     eax,[dSize]

        xor     [h1],eax
        xor     [h2],eax
        xor     [h3],eax
        xor     [h4],eax

        mov     eax,[h1]
        add     eax,[h2]
        add     eax,[h3]
        add     eax,[h4]
        mov     [h1],eax

        add     [h2],eax
        add     [h3],eax
        add     [h4],eax

        mov     ecx,[h1]
        mov     eax,ecx
        shr     eax,16
        xor     ecx,eax
        imul    ecx,0x85ebca6b
        mov     eax,ecx
        shr     eax,13
        xor     ecx,eax
        imul    ecx,0xc2b2ae35
        mov     eax,ecx
        shr     eax,16
        xor     eax,ecx
        mov     [h1],eax

        mov     ecx,[h2]
        mov     eax,ecx
        shr     eax,16
        xor     ecx,eax
        imul    ecx,0x85ebca6b
        mov     eax,ecx
        shr     eax,13
        xor     ecx,eax
        imul    ecx,0xc2b2ae35
        mov     eax,ecx
        shr     eax,16
        xor     eax,ecx
        mov     [h2],eax

        mov     ecx,[h3]
        mov     eax,ecx
        shr     eax,16
        xor     ecx,eax
        imul    ecx,0x85ebca6b
        mov     eax,ecx
        shr     eax,13
        xor     ecx,eax
        imul    ecx,0xc2b2ae35
        mov     eax,ecx
        shr     eax,16
        xor     eax,ecx
        mov     [h3],eax

        mov     ecx,[h4]
        mov     eax,ecx
        shr     eax,16
        xor     ecx,eax
        imul    ecx,0x85ebca6b
        mov     eax,ecx
        shr     eax,13
        xor     ecx,eax
        imul    ecx,0xc2b2ae35
        mov     eax,ecx
        shr     eax,16
        xor     eax,ecx
        mov     [h4],eax

        mov     eax,[h1]
        add     eax,[h2]
        add     eax,[h3]
        add     eax,[h4]
        mov     [h1],eax

        add     [h2],eax
        add     [h3],eax
        add     [h4],eax

        mov     edi,[pOut]

        mov     eax,[h1]
        stosd
        mov     eax,[h2]
        stosd
        mov     eax,[h3]
        stosd
        mov     eax,[h4]
        stosd
        mov     eax,4*4
        pop_all
        ret
endp

;****************************************************************************************************

;****************************************************************************************************
proc  MD5hash dta:dword,dtb:dword,dtc:dword,dtd:dword,ptBuffer:dword,dtBufferLength:dword
iglobal
stMD5Result:
    stdtA dd 0
    stdtB dd 0
    stdtC dd 0
    stdtD dd 0
endg
      push_all

      ; phase I ¬ padding
      mov     edi,[ptBuffer]
      mov     eax,[dtBufferLength]

      inc     eax
      add     edi,eax
      mov     byte [edi-1],080h

      xor     edx,edx

      mov     ebx,64
      div     ebx

      neg     edx
      add     edx,64

      cmp     edx,8
      jae     @f

      add     edx,64

@@:
      mov     ecx,edx
      xor     al,al
      rep     stosb

      mov     eax,[dtBufferLength]

      inc     edx
      add     [dtBufferLength],edx

      xor     edx,edx

      mov     ebx,8
      mul     ebx

      mov     dword [edi-8],eax
      mov     dword [edi-4],edx

      mov     edx,[dtBufferLength]

      mov     edi,[ptBuffer]

      ;phase II ¬ chaining variables initialization

      mov     [stdtA],067452301h
      mov     [stdtB],0efcdab89h
      mov     [stdtC],098badcfeh
      mov     [stdtD],010325476h

      ; phase III ¬ hashing
hashloop:
      mov     eax,[stdtA]
      mov     [dta],eax
      mov     eax,[stdtB]
      mov     [dtb],eax
      mov     eax,[stdtC]
      mov     [dtc],eax
      mov     eax,[stdtD]
      mov     [dtd],eax

      stdcall  FF,[dta],[dtb],[dtc],[dtd],dword [edi+00*4],07,0d76aa478h
      mov     [dta],eax
      stdcall  FF,[dtd],[dta],[dtb],[dtc],dword [edi+01*4],12,0e8c7b756h
      mov     [dtd],eax
      stdcall  FF,[dtc],[dtd],[dta],[dtb],dword [edi+02*4],17,0242070dbh
      mov     [dtc],eax
      stdcall  FF,[dtb],[dtc],[dtd],[dta],dword [edi+03*4],22,0c1bdceeeh
      mov     [dtb],eax
      stdcall  FF,[dta],[dtb],[dtc],[dtd],dword [edi+04*4],07,0f57c0fafh
      mov     [dta],eax
      stdcall  FF,[dtd],[dta],[dtb],[dtc],dword [edi+05*4],12,04787c62ah
      mov     [dtd],eax
      stdcall  FF,[dtc],[dtd],[dta],[dtb],dword [edi+06*4],17,0a8304613h
      mov     [dtc],eax
      stdcall  FF,[dtb],[dtc],[dtd],[dta],dword [edi+07*4],22,0fd469501h
      mov     [dtb],eax
      stdcall  FF,[dta],[dtb],[dtc],[dtd],dword [edi+08*4],07,0698098d8h
      mov     [dta],eax
      stdcall  FF,[dtd],[dta],[dtb],[dtc],dword [edi+09*4],12,08b44f7afh
      mov     [dtd],eax
      stdcall  FF,[dtc],[dtd],[dta],[dtb],dword [edi+10*4],17,0ffff5bb1h
      mov     [dtc],eax
      stdcall  FF,[dtb],[dtc],[dtd],[dta],dword [edi+11*4],22,0895cd7beh
      mov     [dtb],eax
      stdcall  FF,[dta],[dtb],[dtc],[dtd],dword [edi+12*4],07,06b901122h
      mov     [dta],eax
      stdcall  FF,[dtd],[dta],[dtb],[dtc],dword [edi+13*4],12,0fd987193h
      mov     [dtd],eax
      stdcall  FF,[dtc],[dtd],[dta],[dtb],dword [edi+14*4],17,0a679438eh
      mov     [dtc],eax
      stdcall  FF,[dtb],[dtc],[dtd],[dta],dword [edi+15*4],22,049b40821h
      mov     [dtb],eax

      ; round 2
      stdcall  GG,[dta],[dtb],[dtc],[dtd],dword [edi+01*4],05,0f61e2562h
      mov     [dta],eax
      stdcall  GG,[dtd],[dta],[dtb],[dtc],dword [edi+06*4],09,0c040b340h
      mov     [dtd],eax
      stdcall  GG,[dtc],[dtd],[dta],[dtb],dword [edi+11*4],14,0265e5a51h
      mov     [dtc],eax
      stdcall  GG,[dtb],[dtc],[dtd],[dta],dword [edi+00*4],20,0e9b6c7aah
      mov     [dtb],eax
      stdcall  GG,[dta],[dtb],[dtc],[dtd],dword [edi+05*4],05,0d62f105dh
      mov     [dta],eax
      stdcall  GG,[dtd],[dta],[dtb],[dtc],dword [edi+10*4],09,002441453h
      mov     [dtd],eax
      stdcall  GG,[dtc],[dtd],[dta],[dtb],dword [edi+15*4],14,0d8a1e681h
      mov     [dtc],eax
      stdcall  GG,[dtb],[dtc],[dtd],[dta],dword [edi+04*4],20,0e7d3fbc8h
      mov     [dtb],eax
      stdcall  GG,[dta],[dtb],[dtc],[dtd],dword [edi+09*4],05,021e1cde6h
      mov     [dta],eax
      stdcall  GG,[dtd],[dta],[dtb],[dtc],dword [edi+14*4],09,0c33707d6h
      mov     [dtd],eax
      stdcall  GG,[dtc],[dtd],[dta],[dtb],dword [edi+03*4],14,0f4d50d87h
      mov     [dtc],eax
      stdcall  GG,[dtb],[dtc],[dtd],[dta],dword [edi+08*4],20,0455a14edh
      mov     [dtb],eax
      stdcall  GG,[dta],[dtb],[dtc],[dtd],dword [edi+13*4],05,0a9e3e905h
      mov     [dta],eax
      stdcall  GG,[dtd],[dta],[dtb],[dtc],dword [edi+02*4],09,0fcefa3f8h
      mov     [dtd],eax
      stdcall  GG,[dtc],[dtd],[dta],[dtb],dword [edi+07*4],14,0676f02d9h
      mov     [dtc],eax
      stdcall  GG,[dtb],[dtc],[dtd],[dta],dword [edi+12*4],20,08d2a4c8ah
      mov     [dtb],eax

      ; round 3
      stdcall  HH,[dta],[dtb],[dtc],[dtd],dword [edi+05*4],04,0fffa3942h
      mov     [dta],eax
      stdcall  HH,[dtd],[dta],[dtb],[dtc],dword [edi+08*4],11,08771f681h
      mov     [dtd],eax
      stdcall  HH,[dtc],[dtd],[dta],[dtb],dword [edi+11*4],16,06d9d6122h
      mov     [dtc],eax
      stdcall  HH,[dtb],[dtc],[dtd],[dta],dword [edi+14*4],23,0fde5380ch
      mov     [dtb],eax
      stdcall  HH,[dta],[dtb],[dtc],[dtd],dword [edi+01*4],04,0a4beea44h
      mov     [dta],eax
      stdcall  HH,[dtd],[dta],[dtb],[dtc],dword [edi+04*4],11,04bdecfa9h
      mov     [dtd],eax
      stdcall  HH,[dtc],[dtd],[dta],[dtb],dword [edi+07*4],16,0f6bb4b60h
      mov     [dtc],eax
      stdcall  HH,[dtb],[dtc],[dtd],[dta],dword [edi+10*4],23,0bebfbc70h
      mov     [dtb],eax
      stdcall  HH,[dta],[dtb],[dtc],[dtd],dword [edi+13*4],04,0289b7ec6h
      mov     [dta],eax
      stdcall  HH,[dtd],[dta],[dtb],[dtc],dword [edi+00*4],11,0eaa127fah
      mov     [dtd],eax
      stdcall  HH,[dtc],[dtd],[dta],[dtb],dword [edi+03*4],16,0d4ef3085h
      mov     [dtc],eax
      stdcall  HH,[dtb],[dtc],[dtd],[dta],dword [edi+06*4],23,004881d05h
      mov     [dtb],eax
      stdcall  HH,[dta],[dtb],[dtc],[dtd],dword [edi+09*4],04,0d9d4d039h
      mov     [dta],eax
      stdcall  HH,[dtd],[dta],[dtb],[dtc],dword [edi+12*4],11,0e6db99e5h
      mov     [dtd],eax
      stdcall  HH,[dtc],[dtd],[dta],[dtb],dword [edi+15*4],16,01fa27cf8h
      mov     [dtc],eax
      stdcall  HH,[dtb],[dtc],[dtd],[dta],dword [edi+02*4],23,0c4ac5665h
      mov     [dtb],eax

      ; round 4
      stdcall  II,[dta],[dtb],[dtc],[dtd],dword [edi+00*4],06,0f4292244h
      mov     [dta],eax
      stdcall  II,[dtd],[dta],[dtb],[dtc],dword [edi+07*4],10,0432aff97h
      mov     [dtd],eax
      stdcall  II,[dtc],[dtd],[dta],[dtb],dword [edi+14*4],15,0ab9423a7h
      mov     [dtc],eax
      stdcall  II,[dtb],[dtc],[dtd],[dta],dword [edi+05*4],21,0fc93a039h
      mov     [dtb],eax
      stdcall  II,[dta],[dtb],[dtc],[dtd],dword [edi+12*4],06,0655b59c3h
      mov     [dta],eax
      stdcall  II,[dtd],[dta],[dtb],[dtc],dword [edi+03*4],10,08f0ccc92h
      mov     [dtd],eax
      stdcall  II,[dtc],[dtd],[dta],[dtb],dword [edi+10*4],15,0ffeff47dh
      mov     [dtc],eax
      stdcall  II,[dtb],[dtc],[dtd],[dta],dword [edi+01*4],21,085845dd1h
      mov     [dtb],eax
      stdcall  II,[dta],[dtb],[dtc],[dtd],dword [edi+08*4],06,06fa87e4fh
      mov     [dta],eax
      stdcall  II,[dtd],[dta],[dtb],[dtc],dword [edi+15*4],10,0fe2ce6e0h
      mov     [dtd],eax
      stdcall  II,[dtc],[dtd],[dta],[dtb],dword [edi+06*4],15,0a3014314h
      mov     [dtc],eax
      stdcall  II,[dtb],[dtc],[dtd],[dta],dword [edi+13*4],21,04e0811a1h
      mov     [dtb],eax
      stdcall  II,[dta],[dtb],[dtc],[dtd],dword [edi+04*4],06,0f7537e82h
      mov     [dta],eax
      stdcall  II,[dtd],[dta],[dtb],[dtc],dword [edi+11*4],10,0bd3af235h
      mov     [dtd],eax
      stdcall  II,[dtc],[dtd],[dta],[dtb],dword [edi+02*4],15,02ad7d2bbh
      mov     [dtc],eax
      stdcall  II,[dtb],[dtc],[dtd],[dta],dword [edi+09*4],21,0eb86d391h
      mov     [dtb],eax

      mov     eax,[dta]
      add     [stdtA],eax
      mov     eax,[dtb]
      add     [stdtB],eax
      mov     eax,[dtc]
      add     [stdtC],eax
      mov     eax,[dtd]
      add     [stdtD],eax

      add     edi,64

      sub     edx,64
      jnz     hashloop

      ; phase IV ¬ results

      mov     ecx,4
      mov     esi,stMD5Result
@@:
      mov     eax,dword [esi]
      xchg    al,ah
      rol     eax,16
      xchg    al,ah
      mov     dword [esi],eax

      add     esi,4

      loop    @b

      pop_all

      ret
endp

proc  FF var1,var2,var3,var4,x,s,t
      ; a = b + ((a + F(b,c,d) + x + t) << s )
      push    ebx ecx

      mov     eax,[var2]
      mov     ebx,[var3]
      mov     ecx,[var4]

      ; F(x,y,z) = (x and y) or ((not x) and z)
      and     ebx,eax
      not     eax
      and     eax,ecx
      or      eax,ebx

      add     eax,[var1]
      add     eax,[x]
      add     eax,[t]

      mov     ecx,[s]
      rol     eax,cl

      add     eax,[var2]

      pop     ecx ebx
      ret
endp

proc  GG var1,var2,var3,var4,x,s,t
      push    ebx ecx
      ; a = b + ((a + G(b,c,d) + x + t) << s)

      mov     eax,[var2]
      mov     ebx,[var3]
      mov     ecx,[var4]

      ; G(x,y,z) = (x and z) or (y and (not z))
      and     eax,ecx
      not     ecx
      and     ecx,ebx
      or      eax,ecx

      add     eax,[var1]
      add     eax,[x]
      add     eax,[t]

      mov     ecx,[s]
      rol     eax,cl

      add     eax,[var2]

      pop     ecx ebx
      ret
endp

proc  HH var1,var2,var3,var4,x,s,t
      push    ebx ecx
      ; a = b + ((a + H(b,c,d) + x + t) << s)

      mov     eax,[var2]
      mov     ebx,[var3]
      mov     ecx,[var4]

      ; H(x,y,z) = x xor y xor z
      xor     eax,ebx
      xor     eax,ecx

      add     eax,[var1]
      add     eax,[x]
      add     eax,[t]

      mov     ecx,[s]
      rol     eax,cl

      add     eax,[var2]

      pop     ecx ebx
      ret
endp


proc  II var1,var2,var3,var4,x,s,t
      push    ebx ecx
      ; a = b + ((a + I(b,c,d) + x + t) << s)

      mov     eax,[var2]
      mov     ebx,[var3]
      mov     ecx,[var4]

      ; I(x,y,z) = y xor (x or (not z))
      not     ecx
      or      eax,ecx
      xor     eax,ebx

      add     eax,[var1]
      add     eax,[x]
      add     eax,[t]

      mov     ecx,[s]
      rol     eax,cl

      add     eax,[var2]

      pop     ecx ebx
      ret
endp                       
;****************************************************************************************************


TEA_MAGIC1 = 09E3779B9h ; sqr(5)-1 * 2^31
TEA_MAGIC2 = 0C6EF3720h ; TEA_MAGIC1 shl 5
TEA_ROUNDS = 32

;-------------------------------------------------------
; XTEA Encryption
;-------------------------------------------------------
; Параметры:
;     lpData - указатель на шифруемые данные
;     ddSize - размер данных, выравненный до кратности 8
;     lpKey - указатель на строку ключа
;     dkSize - длина ключа
;-------------------------------------------------------
;****************************************************************************************************
proc XTEA_encrypt lpData:DWORD, ddSize:DWORD, lpKey:DWORD, dkSize:DWORD
        locals
                XTEA_key rb 16
        endl

        push_all

        ; Скопировать ключ шифрования
        xor     eax,eax
        mov     ecx,4
        lea     edi,[XTEA_key]
        cld
        rep     stosd
        mov     ecx,[dkSize]
        cmp     ecx,16
        jbe     @f
        mov     ecx,16
@@:
        mov     esi,[lpKey]
        lea     edi,[XTEA_key]
        rep     movsb

        ; Цикл шифрования
        mov     ebx,[lpData]
        xor     ecx,ecx
.loc_encrypt:
        cmp     ecx,[ddSize]
        jae     .loc_ret

        push    ecx
        push    ebx

        mov     esi,[ebx+ecx+0]
        mov     edi,[ebx+ecx+4]

        xor     edx,edx
        mov     ecx,TEA_ROUNDS
@@:
        push    ecx
        mov     eax,edi
        mov     ebx,edi
        shl     eax,4
        shr     ebx,5
        mov     ecx,edx
        xor     eax,ebx
        and     ecx,3
        add     eax,edi
        lea     ebx,[XTEA_key]
        mov     ebx,[ebx+4*ecx]
        add     ebx,edx
        xor     eax,ebx
        add     edx,TEA_MAGIC1
        add     esi,eax
        mov     ecx,edx
        mov     eax,esi
        mov     ebx,esi
        shl     eax,4
        shr     ebx,5
        shr     ecx,11
        xor     eax,ebx
        and     ecx,3
        mov     ebx,edx
        add     eax,esi
        push    edx
        lea     edx,[XTEA_key]
        add     ebx,[edx+4*ecx]
        pop     edx
        xor     eax,ebx
        add     edi,eax
        pop     ecx
        dec     ecx
        jnz     @b

        pop     ebx
        pop     ecx

        mov     [ebx+ecx+0],esi
        mov     [ebx+ecx+4],edi
                
        add     ecx,8
        jmp     .loc_encrypt
.loc_ret:
        mov     eax,ecx
                pop_all
        ret
endp
;****************************************************************************************************

;-------------------------------------------------------
; XTEA Decryption
;-------------------------------------------------------
; Параметры:
;     lpData - указатель на расшифровываемые данные
;     ddSize - размер данных, выравненный до кратности 8
;     lpKey - указатель на строку ключа
;     dkSize - длина ключа
;****************************************************************************************************
proc XTEA_decrypt lpData:DWORD, ddSize:DWORD, lpKey:DWORD, dkSize:DWORD
        locals
                XTEA_key rb 16
        endl

        push_all

        ; Скопировать ключ шифрования
        xor     eax,eax
        mov     ecx,4
        lea     edi,[XTEA_key]
        cld
        rep     stosd
        mov     ecx,[dkSize]
        cmp     ecx,16
        jbe     @f
        mov     ecx,16
@@:
        mov     esi,[lpKey]
        lea     edi,[XTEA_key]
        rep     movsb

        ; Цикл расшифровки
        mov     ebx,[lpData]
        xor     ecx,ecx
.loc_decrypt:
        cmp     ecx,[ddSize]
        jae     .loc_ret

        push    ecx
        push    ebx

        mov     esi,[ebx+ecx+0]
        mov     edi,[ebx+ecx+4]

        mov     edx,TEA_MAGIC2
        mov     ecx,TEA_ROUNDS
@@:
        push    ecx
        mov     eax,esi
        mov     ebx,esi
        shl     eax,4
        shr     ebx,5
        mov     ecx,edx
        xor     eax,ebx
        shr     ecx,11
        add     eax,esi
        mov     ebx,edx
        and     ecx,3
        sub     edx,TEA_MAGIC1
        push    edx
        lea     edx,[XTEA_key]
        add     ebx,[edx+4*ecx]
        pop     edx
        xor     ebx,eax
        sub     edi,ebx
        mov     eax,edi
        mov     ebx,edi
        shl     eax,4
        shr     ebx,5
        mov     ecx,edx
        xor     eax,ebx
        and     ecx,3
        mov     ebx,edx
        add     eax,edi
        push    edx
        lea     edx,[XTEA_key]
        add     ebx,[edx+4*ecx]
        pop     edx
        xor     ebx,eax
        sub     esi,ebx
        pop     ecx
        dec     ecx
        jnz     @b

        pop     ebx
        pop     ecx

        mov     [ebx+ecx+0],esi
        mov     [ebx+ecx+4],edi

        add     ecx,8
        jmp     .loc_decrypt
.loc_ret:
        mov     eax,ecx
                pop_all
        ret
endp                         
;****************************************************************************************************

;****************************************************************************************************
proc    GetBits,pDWORD
                push_all
                mov     EAX,[pDWORD]
        MOV     EDX,EAX
        SHR     EAX,1
        AND     EAX,055555555h
        SUB     EDX,EAX
        MOV     EAX,EDX
        SHR     EDX,2
        AND     EAX,033333333h
        AND     EDX,033333333h
        ADD     EAX,EDX

        MOV     EDX,EAX
        SHR     EAX,4
        ADD     EAX,EDX
        AND     EAX,00F0F0F0Fh
        IMUL    EAX,001010101h
        SHR     EAX,24
                pop_all
                ret
endp
;****************************************************************************************************

;***************************************************************************************************************
proc _replace SRCdata:dword, SRCsize:dword, PTRFind:dword,PTRFindSize:dword,\
              PTRReplace:dword,PTRReplaceSize:dword
        push_all
        mov     edx,[SRCdata]
        add     [SRCsize],edx
        xor     eax,eax
                cld             
.scanmem_loop:
        mov     esi,[SRCdata]
        mov     edi,[PTRFind]
.scanmem_not_next:
        xor     ebx,ebx
.scanmem_scan:
        cmp     esi,[SRCsize]
        je      .scanmem_not_found
        lodsb
        cmp     al,[edi+ebx]
        jne     .scanmem_not_next
        inc     ebx
        cmp     ebx,[PTRFindSize]
        jne     .scanmem_scan
.scanmem_found:
        mov     [SRCdata],esi
        mov     edi,esi
        sub     edi,[PTRFindSize]
        add     edi,[PTRReplaceSize]
        mov     ecx,[SRCsize]
        sub     ecx,[SRCdata]
        mov     [SRCdata],edi
        cmp     ebx,[PTRReplaceSize]
        jb      .scanmem_mem_back
.scanmem_mem_forward:
        repne   movsb
        mov     [edi],ah
        mov     [SRCsize],edi
        jmp     .scanmem_replace
.scanmem_mem_back:
        lea     ebx,[edi+ecx]
        mov     [ebx],ah
        mov     [SRCsize],ebx
.scanmem_back_mem:
        mov     al,[esi+ecx]
        mov     [edi+ecx],al
        dec     ecx
        jnl     .scanmem_back_mem
.scanmem_replace:
        mov     edi,[SRCdata]
        sub     edi,[PTRReplaceSize]
        mov     esi,[PTRReplace]
        mov     ecx,[PTRReplaceSize]
.scanmem_replace_loop:
        repne   movsb
        jmp     .scanmem_loop
.scanmem_not_found:
        mov     eax,[SRCsize]
        sub     eax,[SRCdata]
.scanmem_ret:
        pop_all
        ret
endp
;***************************************************************************************************************

;*************************************************
proc    RadixConvert,pBuff,pRadix
        push_all
        mov     edi,[pBuff]
        xor     ebx,ebx
        @@:
        xor     edx,edx
        mov     ecx,[pRadix]
        div     ecx
        push    edx
        inc     ebx
        or      eax,eax
        jne     @b
        @@:
        pop     eax
        add     al,"0"
        stosb
        dec     ebx
        jne     @b
        mov     al,00
        ;stosb
        pop_all
        ret
endp
;*************************************************

;*************************************************
proc    RadixConvertEx,pBuff,pRadix,pCnt
        push_all
        mov     edi,[pBuff]
        xor     ebx,ebx
        @@:
        xor     edx,edx
        mov     ecx,[pRadix]
        div     ecx
        push    edx
        inc     ebx
        or      eax,eax
        jne     @b
        cmp     ebx,[pCnt]
        jb      @b
        mov     [pCnt],ebx
        @@:
        pop     eax
        cmp     al,0xA
        jb      .dec
        add     al,0x7
        .dec:
        add     al,"0"
        stosb
        dec     ebx
        jne     @b
        mov     al,00
        stosb
        mov     eax,[pCnt]
        pop_all
        ret
endp
;*************************************************

;*************************************************
proc    RadixDecode,pBuff,pRadix
        push_all
        xor     eax,eax
        mov     esi,[pBuff]
        @@:
        movsx   ebx,byte[esi]
        sub     bl,"0"
        cmp     ebx,[pRadix]
        jae     @f
        xor     edx,edx
        mov     ecx,[pRadix]
        imul    ecx
        add     eax,ebx
        inc     esi
        jmp     @b
        @@:
        pop_all
        ret
endp
;*************************************************

;*************************************************
proc    RadixDecodeEx,pBuff,pRadix,pCnt
        push_all
        xor     eax,eax
        xor     edi,edi
        mov     esi,[pBuff]
        @@:
        movsx   ebx,byte[esi]
        sub     bl,"0"
        cmp     ebx,[pRadix]
        jae     @f
        xor     edx,edx
        mov     ecx,[pRadix]
        imul    ecx
        add     eax,ebx
        inc     edi
        inc     esi
        cmp     edi,[pCnt]
        jb      @b
        @@:
        pop_all
        ret
endp
;*************************************************

;****************************************************************************************************
proc    SortArryStringEx,pArry,pCnt
        push_all
        mov     ebx,[pCnt]
        shl     ebx,2
        invoke  VirtualAlloc,0,ebx,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        or      eax,eax
        je      .sort_ret
        mov     edi,eax         ;QuickSortArryLen
        mov     esi,[pArry]
		xor     ebx,ebx
.sort_len:
        mov     eax,[esi+ebx*4]
        or      eax,eax
        je      .sort_ret
        stdcall _lstrlen,eax
        mov     [edi+ebx*4],eax
        inc     ebx
        cmp     ebx,[pCnt]
        jb      .sort_len
        stdcall HeapSortMod,edi,esi,ebx
        xor     ebx,ebx
.sort_start:
        xor     ecx,ecx
        mov     eax,[edi+ebx*4]
        lea     edx,[esi+ebx*4]
.sort_next:
        inc     ecx
        inc     ebx
        cmp     ebx,[pCnt]
        jae     .sort_end
.sort_string:
        cmp     [edi+ebx*4],eax
        je      .sort_next
        stdcall HeapSortString,edx,ecx
        jmp     .sort_start
.sort_end:
        stdcall HeapSortString,edx,ecx
        invoke  VirtualFree,edi,0,MEM_RELEASE
        mov     eax,[pCnt]
.sort_ret:
        pop_all
        ret
endp
;****************************************************************************************************
                                       
;****************************************************************************************************
proc    SortArryStringDoobleEx,pArry,pCnt
        push_all
        mov     ebx,[pCnt]
        shl     ebx,2
        invoke  VirtualAlloc,0,ebx,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        or      eax,eax
        je      .sort_ret
        mov     esi,eax
        xor     ebx,ebx
        mov     edi,[pArry]
.sort_hash:
        mov     eax,[edi+ebx*4]
        or      eax,eax
        je      .sort_ret
        stdcall _lstrlen,eax
        stdcall Murmur2,dword[edi+ebx*4],eax,0
        mov     [esi+ebx*4],eax
        inc     ebx
        cmp     ebx,[pCnt]
        jb      .sort_hash
        stdcall shell_sort,esi,edi,ebx
        xor     ebx,ebx
        xor     ecx,ecx
.sort_start:
        mov     eax,[esi+ebx*4]
        inc     ebx
        cmp     ebx,[pCnt]
        jae     .sort_end
        cmp     eax,[esi+ebx*4]
        jne     .sort_start
        inc     ecx
        or      eax,-1
        mov     [esi+ebx*4-4],eax
        jmp     .sort_start
.sort_end:
        sub     ebx,ecx
        stdcall shell_sort,esi,edi,[pCnt]
        invoke  VirtualFree,esi,0,MEM_RELEASE
        mov     eax,ebx
.sort_ret:
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    DellDoobleDword,pArry,pCnt
        push_all
        mov     esi,[pArry]
        xor     ebx,ebx
        xor     ecx,ecx
.sort_start:
        mov     eax,[esi+ebx*4]
        inc     ebx
        cmp     ebx,[pCnt]
        jae     .sort_end
        cmp     eax,[esi+ebx*4]
        jne     .sort_start
        inc     ecx
        or      eax,-1
        mov     [esi+ebx*4-4],eax
        jmp     .sort_start
.sort_end:
        sub     ebx,ecx
        stdcall shell_sort,esi,esi,[pCnt]
        mov     eax,ebx
        pop_all
        ret
endp
;**************************************************************************************************** 

;********************************************************************************************************************
proc    HeapSortString, lpArray:DWORD, uiAmount:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     ebx,    [uiAmount]
        mov     ecx,    ebx
        shr     ecx,    1
        dec     ebx
        jmp     ._next
        ._for:
                stdcall  siftDownString,esi, ecx, ebx
        ._next:
                dec     ecx
        jns     ._for
        mov     ecx,    ebx
        jmp     ._test_while
        ._while:
                push    dword  [esi]
                push    dword  [esi+4*ecx]
                pop     dword  [esi]
                pop     dword  [esi+4*ecx]
                dec     ecx
                stdcall  siftDownString,esi, 0, ecx
        ._test_while:
                test    ecx,    ecx
        jnz     ._while
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    siftDownString,lpArray:DWORD, K:DWORD, N:DWORD
        pushad
        mov     ebx,    [N]
        mov     esi,    [lpArray]
        mov     ecx,    [K]
        test    ebx,    ebx
        jz      ._exit
        mov     ebx,    [esi+ecx*4]
        ._while:
                mov     edx,    ecx
                add     edx,    edx
                inc     edx
                cmp     edx,    [N]
                ja      ._break
                mov     eax,    edx
                inc     eax
                cmp     eax,    [N]
                ja      @f
                stdcall _lstrcmpiw,dword[esi+eax*4],dword[esi+edx*4]
                cmp     eax,-1
                je      @f
                or      eax,eax
                je      @f
                inc     edx
        @@:
                stdcall _lstrcmpiw,ebx,dword[esi+edx*4]
                cmp     eax,1
                je      ._break
                or      eax,eax
                je      ._break
                mov     eax,    [esi+edx*4]
                mov     [esi+ecx*4],    eax
                mov     ecx,    edx
        jmp     ._while
._break:
        mov     [esi+ecx*4],    ebx
._exit:
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    siftDown,lpArray:DWORD,ldArray:DWORD, K:DWORD, N:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     edi,    [ldArray]
        mov     ebx,    [N]
        mov     ecx,    [K]
        test    ebx,    ebx
        jz      ._exit
        mov     ebx,    [esi+ecx*4]
        mov     ebp,    [edi+ecx*4]
        ._while:
                mov     edx,    ecx
                add     edx,    edx
                inc     edx
                cmp     edx,    [esp+0x20+0x14]
                ja      ._break

                mov     eax,    edx
                inc     eax
                cmp     eax,    [esp+0x20+0x14]
                ja      @f
                mov     eax,    [esi+eax*4]
                cmp     eax,    [esi+edx*4]
                jbe      @f
                inc     edx
        @@:                            
                cmp     ebx,    [esi+edx*4]
                jae      ._break                                               
                mov     eax,    [esi+edx*4]
                mov     [esi+ecx*4],    eax
                mov     eax,    [edi+edx*4]
                mov     [edi+ecx*4],    eax
                mov     ecx,    edx
        jmp     ._while
._break:
        mov     [esi+ecx*4],    ebx
        mov     [edi+ecx*4],    ebp
._exit:
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    HeapSortMod,lpArray:DWORD,ldArray:DWORD, uiAmount:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     edi,    [ldArray]
        mov     ebx,    [uiAmount]
        mov     ecx,    ebx
        shr     ecx,    1
        dec     ebx
        jmp     ._next
        ._for:
                stdcall  siftDown,esi,edi, ecx, ebx
        ._next:
                dec     ecx
        jns     ._for
        mov     ecx,    ebx
        jmp     ._test_while
        .z_while:
                push    dword  [esi]
                push    dword  [esi+4*ecx]
                pop     dword  [esi]
                pop     dword  [esi+4*ecx]

                push    dword  [edi]
                push    dword  [edi+4*ecx]
                pop     dword  [edi]
                pop     dword  [edi+4*ecx]
                dec     ecx
                stdcall  siftDown,esi,edi, 0, ecx
        ._test_while:
                test    ecx,    ecx
        jnz     .z_while
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    HeapSort,lpArray:DWORD, uiAmount:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     ebx,    [uiAmount]
        mov     ecx,    ebx
        shr     ecx,    1
        dec     ebx
        jmp     ._next
        ._for:
                stdcall  siftDown,esi,esi, ecx, ebx
        ._next:
                dec     ecx
        jns     ._for
        mov     ecx,    ebx
        jmp     ._test_while
        ._while:
                push    dword  [esi]
                push    dword  [esi+4*ecx]
                pop     dword  [esi]
                pop     dword  [esi+4*ecx]
                dec     ecx
                stdcall  siftDown,esi,esi, 0, ecx
        ._test_while:
                test    ecx,    ecx
        jnz     ._while
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    HeapSortStringMod,lpArray:DWORD,ldArray:DWORD, uiAmount:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     edi,    [ldArray]
        mov     ebx,    [uiAmount]
        mov     ecx,    ebx
        shr     ecx,    1
        dec     ebx
        jmp     ._next
        ._for:
                stdcall  siftDownStringMod,esi,edi, ecx, ebx
        ._next:
                dec     ecx
        jns     ._for
        mov     ecx,    ebx
        jmp     ._test_while
        .z_while:
                push    dword  [esi]
                push    dword  [esi+4*ecx]
                pop     dword  [esi]
                pop     dword  [esi+4*ecx]

                push    dword  [edi]
                push    dword  [edi+4*ecx]
                pop     dword  [edi]
                pop     dword  [edi+4*ecx]
                dec     ecx
                stdcall  siftDownStringMod,esi,edi, 0, ecx
        ._test_while:
                test    ecx,    ecx
        jnz     .z_while
        popad
        ret
endp
;********************************************************************************************************************

;********************************************************************************************************************
proc    siftDownStringMod,lpArray:DWORD,ldArray:DWORD, K:DWORD, N:DWORD
        pushad
        mov     esi,    [lpArray]
        mov     edi,    [ldArray]
        mov     ebx,    [N]
        mov     ecx,    [K]
        test    ebx,    ebx
        jz      ._exit
        mov     ebx,    [esi+ecx*4]
        mov     ebp,    [edi+ecx*4]
        ._while:
                mov     edx,    ecx
                add     edx,    edx
                inc     edx
                cmp     edx,    [esp+0x20+0x14]
                ja      ._break

                mov     eax,    edx
                inc     eax
                cmp     eax,    [esp+0x20+0x14]
                ja      @f
                stdcall _lstrcmpiw,dword[esi+eax*4],dword[esi+edx*4]
                cmp     eax,1
                jne     @f
                inc     edx
        @@:
                stdcall _lstrcmpiw,ebx,dword[esi+edx*4]
                cmp      eax,-1
                jne     ._break
                mov     eax,    [esi+edx*4]
                mov     [esi+ecx*4],    eax
                mov     eax,    [edi+edx*4]
                mov     [edi+ecx*4],    eax
                mov     ecx,    edx
        jmp     ._while
._break:
        mov     [esi+ecx*4],    ebx
        mov     [edi+ecx*4],    ebp
._exit:
        popad
        ret
endp
;******************************************************************************************************************** 
                 
;*******************************************************************************************************************
proc shell_sort lpArray:DWORD,ldArray:DWORD, dLen:DWORD
        push_all

        mov     esi,[lpArray]
        mov     edi,[ldArray]

        mov     ecx,[dLen]
        shr     ecx,1
                dec             ecx
        jne     .loc_loop_1
        inc     ecx
.loc_loop_1:
        or      ecx,ecx
        jz      .loc_ret

        mov     ebp,ecx
.loc_loop_2:
        mov     edx,ebp
        sub     edx,ecx
.loc_loop_3:
        or      edx,edx
        js      .loc_loop_4

        lea     eax,[edx+ecx]
        mov     ebx,[esi+eax*4]
        mov     eax,[esi+edx*4]
        
                cmp     eax,ebx
        jbe     .loc_loop_4

        mov     [esi+edx*4],ebx
        lea     ebx,[edx+ecx]
        mov     [esi+ebx*4],eax

        cmp     esi,edi
        je      .loc_no_addr

        lea     eax,[edx+ecx]
        mov     ebx,[edi+eax*4]
        mov     eax,[edi+edx*4]

        mov     [edi+edx*4],ebx
        lea     ebx,[edx+ecx]
        mov     [edi+ebx*4],eax
.loc_no_addr:
        sub     edx,ecx
        jmp     .loc_loop_3
.loc_loop_4:
        inc     ebp
        cmp     ebp,[esp+0x30]
        jb      .loc_loop_2

        shr     ecx,1
        jmp     .loc_loop_1
.loc_ret:
        pop_all
        ret
endp                            
;*******************************************************************************************************************

;*******************************************************************************************************************
proc shell_sort_neg lpArray:DWORD,ldArray:DWORD, dLen:DWORD
        push_all

        mov     esi,[lpArray]
        mov     edi,[ldArray]

        mov     ecx,[dLen]
        shr     ecx,1
                dec             ecx
        jne     .loc_loop_1
        inc     ecx
.loc_loop_1:
        or      ecx,ecx
        jz      .loc_ret

        mov     ebp,ecx
.loc_loop_2:
        mov     edx,ebp
        sub     edx,ecx
.loc_loop_3:
        or      edx,edx
        js      .loc_loop_4

        lea     eax,[edx+ecx]
        mov     ebx,[esi+eax*4]
        mov     eax,[esi+edx*4]
        cmp     eax,ebx
        jae     .loc_loop_4

        mov     [esi+edx*4],ebx
        lea     ebx,[edx+ecx]
        mov     [esi+ebx*4],eax

        cmp     esi,edi
        je      .loc_no_addr

        lea     eax,[edx+ecx]
        mov     ebx,[edi+eax*4]
        mov     eax,[edi+edx*4]

        mov     [edi+edx*4],ebx
        lea     ebx,[edx+ecx]
        mov     [edi+ebx*4],eax
.loc_no_addr:
        sub     edx,ecx
        jmp     .loc_loop_3
.loc_loop_4:
        inc     ebp
        cmp     ebp,[esp+0x30]
        jb      .loc_loop_2

        shr     ecx,1
        jmp     .loc_loop_1
.loc_ret:
        pop_all
        ret
endp                            
;*******************************************************************************************************************





;****************************************************************************************************
proc    _wsprintf,pBuff,pFormat
        push_all
        mov     esi,[pFormat]
        mov     edi,[pBuff]
        mov     [pBuff],edi
        mov     ebx,ebp
        add     ebx,4*4
        xor     edx,edx
.scan_format:
        mov     ax,[esi]
        cmp     ax,"%s"
        je      .to_string
        cmp     ax,"%d"
        je      .to_dec
        cmp     ax,"%x"
        je      .to_hex
        movzx   ecx,word[esi+2]
        xor     cx,"00"
        shl     cl,4
        add     cl,ch
        movzx   ecx,cl
        cmp     ax,"%D"
        je      .to_dec_max
        cmp     ax,"%X"
        je      .to_hex_max
        or      al,al
        je      .scan_format_ret
        mov     [edi],al
.scan_format_next:
        inc     edi
        inc     esi
        jmp     .scan_format
.to_null:
        add     ebx,4
        add     esi,2
        jmp     .scan_format
.to_string:
        xor     eax,eax
        mov     [edi],al
        stdcall _lstrcat,edi,dword[ebx]
        add     edi,eax
        jmp     .to_null
.to_dec:
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        jmp     .to_null
.to_hex:
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,16,0
        add     edi,eax
        jmp     .to_null
.to_dec_max:
        add     esi,2
        ;stdcall RadixDecodeEx,esi,10,2
        ;mov     ecx,eax
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,10,ecx
        add     edi,eax
        jmp     .to_null
.to_hex_max:
        add     esi,2
        ;stdcall RadixDecodeEx,esi,16,2
        ;mov     ecx,eax
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,16,ecx
        add     edi,eax
        jmp     .to_null
.scan_format_ret:
        sub     edi,[pBuff]
        mov     eax,edi
        pop_all
        ret
endp 
;****************************************************************************************************

;****************************************************************************************************
proc    _sprintf,pBuff,pFormat
        push_all
        mov     esi,[pFormat]
        mov     edi,[pBuff]
        mov     [pBuff],edi
        mov     ebx,ebp
        add     ebx,4*4
        xor     edx,edx
.scan_format:
        mov     ax,[esi]
        cmp     ax,"%s"
        je      .to_string
        cmp     ax,"%d"
        je      .to_dec
        cmp     ax,"%h"
        je      .to_hex
        cmp     ax,"%x"
        je      .to_hex_max
        cmp     ax,"%f"
        je      .to_float
		cmp     ax,"%I"
        je      .to_ip
        or      al,al
        je      .scan_format_ret
        mov     [edi],al
.scan_format_next:
        inc     edi
        inc     esi
        jmp     .scan_format
.to_null:
        add     ebx,4
        add     esi,2
        jmp     .scan_format
.to_string:
        xor     eax,eax
        mov     [edi],al
        stdcall _lstrcat,edi,dword[ebx]
        add     edi,eax
        jmp     .to_null
.to_dec:
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        jmp     .to_null
.to_hex:
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,16,0
        add     edi,eax
        jmp     .to_null
.to_hex_max:
        mov     eax,[ebx]
        stdcall RadixConvertEx,edi,16,8
        add     edi,eax
        jmp     .to_null
.to_float:
        mov     eax,[ebx]
        stdcall FloatToString,edi,3,eax
        add     edi,eax
        jmp     .to_null
.to_ip:
		movzx	eax,byte[ebx]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		movzx	eax,byte[ebx+1]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		movzx	eax,byte[ebx+2]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		movzx	eax,byte[ebx+3]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        jmp     .to_null
.scan_format_ret:
		mov		[edi],al
        sub     edi,[pBuff]
        mov     eax,edi
        pop_all
        ret
endp 
;****************************************************************************************************

;****************************************************************************************************
proc    _sprintf_mod,pBuff,pFormat
        push_all
        mov     esi,[pFormat]
        mov     edi,[pBuff]
        mov     [pBuff],edi
        mov     ebx,ebp
        add     ebx,4*4
        xor     edx,edx
.scan_format:
        mov     ax,[esi]
        cmp     ax,"%s"
        je      .to_string
        cmp     ax,"%d"
        je      .to_dec
        cmp     ax,"%h"
        je      .to_hex
        cmp     ax,"%x"
        je      .to_hex_max
        cmp     ax,"%f"
        je      .to_float
		cmp     ax,"%I"
        je      .to_ip
        or      al,al
        je      .scan_format_ret
        mov     [edi],al
.scan_format_next:
        inc     edi
        inc     esi
        jmp     .scan_format
.to_null:
        add     ebx,4
        add     esi,2
        jmp     .scan_format
.to_string:
        xor     eax,eax
        mov     [edi],al
		mov     eax,[ebx]
		mov     eax,[eax]
        stdcall _lstrcat,edi,eax
        add     edi,eax
        jmp     .to_null
.to_dec:
        mov     eax,[ebx]
		mov     eax,[eax]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        jmp     .to_null
.to_hex:
        mov     eax,[ebx]
		mov     eax,[eax]
        stdcall RadixConvertEx,edi,16,0
        add     edi,eax
        jmp     .to_null
.to_hex_max:
        mov     eax,[ebx]
		mov     eax,[eax]
        stdcall RadixConvertEx,edi,16,8
        add     edi,eax
        jmp     .to_null
.to_float:
        mov     eax,[ebx]
		mov     eax,[eax]
        stdcall FloatToString,edi,3,eax
        add     edi,eax
        jmp     .to_null
.to_ip:
        mov     eax,[ebx]
		movzx	eax,byte[eax]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		mov     eax,[ebx]
		add		eax,1
		movzx	eax,byte[eax]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		mov     eax,[ebx]
		add		eax,2
		movzx	eax,byte[eax]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
		mov		al,"."
		stosb
		mov     eax,[ebx]
		add		eax,3
		movzx	eax,byte[eax]
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        jmp     .to_null		
.scan_format_ret:
        mov		[edi],al
		sub     edi,[pBuff]
        mov     eax,edi
        pop_all
        ret
endp 
;****************************************************************************************************


;****************************************************************************************************
;----------------------------------------------------------------
; Функция поиска подстроки в строке по алгоритму Карпа-Рабина
; by ManHunter / PCL (www.manhunter.ru)
;----------------------------------------------------------------
; Параметры:
;   pDATA - указатель на исходную строку
;   pSize - длина исходной строки
;   pStr - указатель на строку для поиска
;   pLen - длина строки для поиска
; На выходе:
;   EAX = позиция подстроки в строке
;   EAX = 0 если подстрока не найдена
;----------------------------------------------------------------
proc kr_search pDATA,pSize,pStr,pLen
        push_all

        ; Искомая строка длиннее области поиска?
        mov     eax,[pSize]
        cmp     eax,[pLen]
        jb      .loc_error

        ; Строки не могут быть пустыми
        cmp     [pSize],0
        je      .loc_error
        cmp     [pLen],0
        je      .loc_error
        ; Искомая строка не может быть длиннее 32 символов
        cmp     [pLen],32
        ja      .loc_error

        ; Первоначальный расчет хешей
        xor     esi,esi
        xor     edi,edi
        xor     ebx,ebx
@@:
        shl     esi,1
        mov     eax,[pStr]
        movzx   eax,byte[eax+ebx]
        add     esi,eax
        shl     edi,1
        mov     eax,[pDATA]
        movzx   eax,byte[eax+ebx]
        add     edi,eax
        inc     ebx
        cmp     ebx,[pLen]
        jb      @b

        mov     ebx,[pLen]
.loc_scan:
        ; Хеши совпали?
        cmp     esi,edi
        jne     @f

        ; Строки совпали целиком?
        push    esi edi
        mov     esi,[pStr]
        mov     edi,[pDATA]
        add     edi,ebx
        sub     edi,[pLen]
        mov     ecx,[pLen]
        repe    cmpsb
        pop     edi esi
        jnz     @f

        ; Найденная позиция подстроки
        sub     ebx,[pLen]
                add             ebx,[pDATA]
        jmp     .loc_done
@@:
        ; Пересчитать сравниваемый хеш
        mov     eax,ebx
        sub     eax,[pLen]
        mov     edx,[pDATA]
        movzx   eax,byte[edx+eax]
        mov     edx,1
        mov     ecx,[pLen]
        dec     ecx
        shl     edx,cl
        mov     ecx,edx
        xor     edx,edx
        imul    ecx
        sub     edi,eax
        shl     edi,1
        mov     eax,ebx
        mov     edx,[pDATA]
        movzx   eax,byte[edx+eax]
        add     edi,eax

        ; Следующая позиция
        inc     ebx
        cmp     ebx,[pSize]
        jb      .loc_scan
.loc_error:
        ; Подстрока не может быть найдена
        xor             ebx,ebx
.loc_done:
                mov             eax,ebx
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc kr_search_mod pDATA,pSize,pStr,pLen
        push_all
        xor     edx,edx
        ; Èñêîìàÿ ñòðîêà äëèííåå îáëàñòè ïîèñêà?
        mov     eax,[pSize]
        cmp     eax,[pLen]
        jb      .loc_done

        ; Ñòðîêè íå ìîãóò áûòü ïóñòûìè
        cmp     [pSize],0
        je      .loc_done
        cmp     [pLen],0
        je      .loc_done
        ; Èñêîìàÿ ñòðîêà íå ìîæåò áûòü äëèííåå 32 ñèìâîëîâ
        cmp     [pLen],32
        ja      .loc_done

        ; Ïåðâîíà÷àëüíûé ðàñ÷åò õåøåé
        stdcall KRCalcHash,[pStr],[pLen]
        mov     esi,eax
        stdcall KRCalcHash,[pDATA],[pLen]
        mov     edi,eax
        mov     edi,eax
        mov     ebx,[pLen]
.loc_scan:
        ; Õåøè ñîâïàëè?
        cmp     esi,edi
        jne     .calc_hash
        inc     [hInc]
        mov     edx,[pDATA]
        add     edx,ebx
        mov     ecx,[pLen]
        sub     edx,ecx
        stdcall _RtlCompareMemory,edx,[pStr],ecx
        or      eax,eax
        je      .loc_done
.calc_hash:
        ; Ïåðåñ÷èòàòü ñðàâíèâàåìûé õåø
        mov     eax,ebx
        sub     eax,[pLen]
        mov     edx,[pDATA]
        movzx   eax,byte[edx+eax]
        mov     edx,1
        mov     ecx,[pLen]
        dec     ecx
        shl     edx,cl
        mov     ecx,edx
        xor     edx,edx
        imul    ecx
        sub     edi,eax
        shl     edi,1
        mov     eax,ebx
        mov     edx,[pDATA]
        movzx   eax,byte[edx+eax]
        add     edi,eax

        ; Ñëåäóþùàÿ ïîçèöèÿ
        inc     ebx
        cmp     ebx,[pSize]
        jb      .loc_scan
        xor     edx,edx
.loc_done:
        mov     eax,edx
        pop_all
        ret
endp

;****************************************************************************************************



;****************************************************************************************************
proc ShiftORInit
iglobal
        pHash   rd      256
endg
        stdcall __RtlFillMemory,pHash,0xFF*4,-1
        ret
endp

;****************************************************************************************************




;****************************************************************************************************
proc ShiftOR pDATA,pSize,pStr,pLen
        push_all
locals
        pHashTable     rd 256
endl
        xor     eax,eax
        cmp     [pLen],0x3F
        ja      .scan_end
        lea     edi,[pHashTable]
        mov     ecx,256
        or      eax,-1
        repne   stosd
        xor     ecx,ecx
        mov     ebx,1
        mov     esi,[pStr]
.create_hash:
        movzx   eax,byte[esi+ecx]
        lea     eax,[pHashTable+eax*4]
        mov     edx,ebx
        not     edx
        and     [eax],edx
        rol     ebx,1
        inc     ecx
        cmp     ecx,[pLen]
        jb      .create_hash
        mov     esi,[pDATA]
        mov     edx,1
        shl     edx,cl
        mov     ebx,0xFFFFFFFE
        or      ecx,-1
.scan_hash:
        xor     eax,eax
        inc     ecx
        cmp     ecx,[pSize]
        jae     .scan_end
        movzx   eax,byte[esi+ecx]
        mov     eax,[pHashTable+eax*4]
        or      eax,ebx
        add     eax,eax
        mov     ebx,eax
        test    ebx,edx
        jne     .scan_hash
        sub     ecx,[pLen]
        add     ecx,[pDATA]
        lea     eax,[ecx+1]
.scan_end:
        pop_all
        ret
endp

;****************************************************************************************************

;****************************************************************************************************
proc ShiftORCreateHash pStr,pLen
        push_all
        mov     eax,0x3F
        cmp     [pLen],eax
        ja      .ret
        xor     ecx,ecx
        mov     ebx,1
        mov     esi,[pStr]
.scan:
        movzx   eax,byte[esi+ecx]
        lea     eax,[pHash+eax*4]
        mov     edx,ebx
        not     edx
        and     [eax],edx
        rol     ebx,1
        inc     ecx
        cmp     ecx,[pLen]
        jb      .scan
        xor     eax,eax
.ret:
        pop_all
        ret
endp

;****************************************************************************************************

;****************************************************************************************************
proc ShiftORGetHash pDATA,pSize
        push_all
        mov     eax,0x3F
        cmp     [pSize],eax
        ja      .ret
        mov     esi,[pDATA]
        xor     ebx,ebx
        xor     ecx,ecx
        mov     ebx,0xFFFFFFFE
.scan:
        movzx   eax,byte[esi+ecx]
        mov     eax,[pHash+eax*4]
        or      eax,ebx
        add     eax,eax
        mov     ebx,eax
        test    ebx,edx
        inc     ecx
        cmp     ecx,[pSize]
        jb      .scan
.ret:
        mov             eax,ebx
                pop_all
        ret
endp

;****************************************************************************************************

;****************************************************************************************************
proc ShiftORFindHash pDATA,pSize,pLen
        push_all
        mov     ecx,[pLen]
        mov     edx,1
        shl     edx,cl
        mov     ebx,0xFFFFFFFE
        or      ecx,-1
        mov     esi,[pDATA]
.scan:
        xor     eax,eax
        inc     ecx
        cmp     ecx,[pSize]
        jae     .ret
        movzx   eax,byte[esi+ecx]
        mov     eax,[pHash+eax*4]
        or      eax,ebx
        add     eax,eax
        mov     ebx,eax
        test    ebx,edx
        jne     .scan
        sub     ecx,[pLen]
        lea     eax,[ecx+esi+1]
.ret:
        pop_all
        ret
endp

;****************************************************************************************************

;****************************************************************************************************
proc ShiftORFindHashEx pDATA,pSize,pLen,pArry
        push_all
        mov     esi,[pDATA]
        mov     edi,[pArry]
        mov     ecx,[pLen]
        mov     eax,1
        shl     eax,cl
        mov     [pDATA],eax
        mov     edx,0xFFFFFFFE
        or      ecx,-1
        xor     ebx,ebx
.scan:
        inc     ecx
        cmp     ecx,[pSize]
        jae     .ret
        movzx   eax,byte[esi+ecx]
        mov     eax,[pHash+eax*4]
        or      eax,edx
        add     eax,eax
        mov     edx,eax
        test    edx,[pDATA]
        jne     .scan
        mov     eax,ecx
        sub     eax,[pLen]
        lea     eax,[eax+esi+1]
        mov     [edi+ebx*4],eax
        inc     ebx
        ;pushad
        ;invoke  MessageBox,0,eax,eax,0
        ;popad
        jmp     .scan
.ret:
        mov     eax,ebx
        pop_all
        ret
endp

;****************************************************************************************************   

;****************************************************************************************************
proc    KRCalcHash,pDATA,pLen
        push_all
        mov     ebx,[pDATA]
        xor     edx,edx
        xor     ecx,ecx
@@:
        shl     edx,1
        movzx   eax,byte[ebx+ecx]
        add     edx,eax
        inc     ecx
        cmp     ecx,[pLen]
        jb      @b
        mov     eax,edx
        pop_all
        ret
endp
;****************************************************************************************************  

;****************************************************************************************************
;---------------------------------------------
; Инициализация генератора случайных чисел
; stdcall xorshift_init,seed
;---------------------------------------------
proc xorshift_init seed:DWORD
        locals
             s0 dd ?
             s1 dd ?
             s2 dd ?
             s3 dd ?
        endl

        push_all

        ; Начальное "засеивание" генератора
        mov     edi,s
        mov     eax,[seed]
        rol     eax,1
        xor     eax,0x1C580662
        stosd
        rol     eax,3
        add     eax,0x0B6F099F
        stosd
        rol     eax,5
        xor     eax,0xB523952E
        stosd
        rol     eax,7
        sub     eax,0xCCF5A0EF
        stosd

        ; Холостая прокрутка генератора для инициализации
        lea     edi,[s0]
        push    edi
        xor     eax,eax
        stosd
        stosd
        stosd
        stosd

        xor     ebx,ebx
.loc_for_1:
        xor     ecx,ecx
.loc_for_2:
        mov     eax,[.lj+ebx*4]
        and     eax,1
        shl     eax,cl
        jz      @f

        mov     esi,s
        lodsd
        xor     [s0],eax
        lodsd
        xor     [s1],eax
        lodsd
        xor     [s2],eax
        lodsd
        xor     [s3],eax
@@:
        stdcall xorshift

        inc     ecx
        cmp     ecx,32
        jb      .loc_for_2

        inc     ebx
        cmp     ebx,4
        jb      .loc_for_1

        pop     esi
        mov     edi,s
        movsd
        movsd
        movsd
        movsd

        pop_all
        ret

.lj:    dd 0xB523952E
        dd 0x0B6F099F
        dd 0xCCF5A0EF
        dd 0x1C580662
endp
;****************************************************************************************************

;****************************************************************************************************
;---------------------------------------------
; Получить случайное число
; stdcall xorshift
; на выходе EAX - случайное число
;---------------------------------------------
proc xorshift
        push_all

        mov     eax,[s+0*4]
        mov     ecx,eax
        shl     eax,11
        xor     ecx,eax

        mov     eax,[s+1*4]
        mov     [s+0*4],eax
        mov     eax,[s+2*4]
        mov     [s+1*4],eax
        mov     eax,[s+3*4]
        mov     [s+2*4],eax

        mov     eax,ecx
        shr     eax,8
        xor     ecx,eax

        mov     eax,[s+3*4]
        mov     ebx,eax
        shr     eax,19
        xor     ebx,eax
        xor     ebx,ecx
        mov     [s+3*4],ebx
		mov		eax,ebx
        pop_all
        ret
s       rd 4            
endp
;****************************************************************************************************

;****************************************************************************************************
proc    HexToBins,pDATA,pSIZE,pBuff,pRad,pLen
        push_all
        mov     esi,[pDATA]
        mov     edi,[pBuff]
        xor     ecx,ecx
        xor     ebx,ebx
.convert:
        movzx   eax,byte[esi+ecx]
        stdcall RadixConvertEx,edi,[pRad],[pLen]
        add     edi,[pLen]
        inc     ecx
        cmp     ecx,[pSIZE]
        jb      .convert
        sub     edi,[pBuff]
        mov     eax,edi
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    BinsToHex,pDATA,pSIZE,pBuff,pRad,pLen
        push_all
        mov     esi,[pDATA]
        mov     edi,[pBuff]
        add     [pSIZE],edi
        xor     ecx,ecx
        xor     edx,edx
.convert:
        stdcall RadixDecodeEx,esi,[pRad],[pLen]
        mov     [edi+ecx],al
        inc     ecx
        add     esi,[pLen]
        cmp     esi,[pSIZE]
        jb      .convert
        mov     eax,ecx
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    DecodeData,pDATA,pSIZE,pPack
        push_all
        mov     esi,[pDATA]
        mov     edi,[pPack]
        xor     ecx,ecx
.decode:
        movzx   eax,byte[esi+ecx]
        movzx   eax,byte[edi+eax]
        mov     [esi+ecx],al
        inc     ecx
        cmp     ecx,[pSIZE]
        jb      .decode
        mov     eax,[pSIZE]
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    EncodeData,pDATA,pSIZE,pPack
locals
        pChar           rd      256
        pCount          rd      256
        zPack           rb      256
        pCnt            dd      0
endl
        push_all
        lea     esi,[pChar]
        lea     edi,[pCount]
        mov     edx,[pPack]
        stdcall CalcEntropyMod,[pDATA],[pSIZE],esi,edi
        mov     [pCnt],eax
        lea     edi,[zPack]
        xor     ecx,ecx
.write_table:
        mov     eax,[esi+ecx*4]
        mov     [edi+eax],cl
        mov     [edx+ecx],al
        inc     ecx
        cmp     ecx,[pCnt]
        jbe     .write_table
        mov     esi,[pDATA]
        xor     ecx,ecx
.xor_table:
        movzx   eax,byte[esi+ecx]
        movzx   eax,byte[edi+eax]
        mov     [esi+ecx],al
        inc     ecx
        cmp     ecx,[pSIZE]
        jb      .xor_table
        mov     eax,[pCnt]
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc RadixSort pArry,pSIZE
locals
        count_arry      rd      256
endl
     push_all
     mov     ebx,[pSIZE]
     mov     esi,[pArry]
     shl     ebx,2
     invoke  VirtualAlloc,0,ebx,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
     or      eax,eax
     je      .ret
     mov     [pArry],eax
     mov     edi,eax
     mov     ecx,[pSIZE]
     lea     edx,[count_arry]
     xor     ebx,ebx
.while_:
     stdcall _RtlZeroMemory,edx,256*4
     stdcall RadSortINC,esi,ecx,edx,ebx                     
     stdcall RadSortADD,edx                      
     stdcall RadSortARRY,esi,ecx,edx,edi,ebx                 
     xchg    esi,edi
     inc     ebx
     cmp     ebx,4
     jne     .while_
     invoke  VirtualFree,[pArry],0,MEM_RELEASE
.ret:
     pop_all
     ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc RadixSortMod pArry,pCount,pCnt
;dArry    dd pArry0,pArryA,pArryB
;pArry0   dd 3,5,2,4,5,8,7,6,-1
;pArryA   dd A1,A2,A3,A4,A5,A6,A7,A8,-1
;pArryB   dd A1,A2,A3,A4,A5,A6,A7,A8,-1
;stdcall RadixSortMod,dArry,3,8
locals
        count_arry      rd      256
        Arry256         rd      256
endl
     push_all
     mov     esi,[pArry]
     lea     edi,[Arry256]
     stdcall _RtlZeroMemory,edi,256*4
     xor     ebx,ebx
.alock_memory:
     mov     eax,[pCnt]
     shl     eax,2
     invoke  VirtualAlloc,0,eax,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
     or      eax,eax
     je      .ret
     mov     [edi+ebx*4],eax
     inc     ebx
     cmp     ebx,[pCount]
     jb      .alock_memory
     xor     ebx,ebx
     lea     edx,[count_arry]
.while_:
     stdcall _RtlZeroMemory,edx,256*4
     stdcall dRadSortINC,esi,[pCnt],edx,ebx
     stdcall qRadSortADD,edx
     stdcall dRadSortARRY,esi,edi,[pCnt],[pCount],edx,ebx
     xchg    esi,edi
     inc     ebx
     cmp     ebx,4
     jne     .while_
     xor     ebx,ebx
.free_memory:
     invoke  VirtualFree,dword[edi+ebx*4],0,MEM_RELEASE
     inc     ebx
     cmp     ebx,[pCount]
     jb      .free_memory
.ret:
     pop_all
     ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc RadixSortStringMod pArry,pCount,pCnt,pLen
;dArry    dd pArry0,pArryA,pArryB
;pArry0   dd pStr1,pStr2,pStr3,pStr4,pStr5,pStr6,pStr7,pStr8,-1
;pArryA   dd 3,1,2,4,5,6,7,8,-1
;pArryB   dd 3,1,2,4,5,6,7,8,-1
;pStr1    db "123456",0
;stdcall RadixSortStringMod,dArry,3,8,6
locals
        count_arry      rd      256
        Arry256         rd      256
endl
     push_all
     mov     esi,[pArry]
     lea     edi,[Arry256]
     stdcall _RtlZeroMemory,edi,256*4
     xor     ebx,ebx
.alock_memory:
     mov     eax,[pCnt]
     shl     eax,2
     invoke  VirtualAlloc,0,eax,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
     or      eax,eax
     je      .ret
     mov     [edi+ebx*4],eax
     inc     ebx
     cmp     ebx,[pCount]
     jb      .alock_memory
     mov     ebx,[pLen]
     sub     ebx,1
     jb      .ret
     lea     edx,[count_arry]
.while_:
     stdcall _RtlZeroMemory,edx,256*4
     stdcall qRadSortINC,esi,[pCnt],edx,ebx
     stdcall qRadSortADD,edx
     stdcall qRadSortARRY,esi,edi,[pCnt],[pCount],edx,ebx
     xchg    esi,edi
     dec     ebx
     jnl     .while_
     xor     ebx,ebx
     test    [pLen],1
     je      .free_memory
.copy_memory:
     mov     eax,[pCnt]
     shl     eax,2
     mov     edx,dword[edi+ebx*4]
     stdcall _RtlCopyMemory,dword[edi+ebx*4],dword[esi+ebx*4],eax
     inc     ebx
     cmp     ebx,[pCount]
     jb      .copy_memory
     xchg    esi,edi
     xor     ebx,ebx
.free_memory:
     invoke  VirtualFree,dword[edi+ebx*4],0,MEM_RELEASE
     inc     ebx
     cmp     ebx,[pCount]
     jb      .free_memory
.ret:
     pop_all
     ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    RadSortARRY,pDATA,pSIZE,pCnt,pMem,pBYTE
        push_all
        mov     esi,[pDATA]
        mov     edi,[pMem]
        mov     edx,[pCnt]
        xor     ecx,ecx
.while_:
        lea     eax,[esi+ecx*4]
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        mov     ebx, dword[edx+eax*4]
        inc     dword[edx+eax*4] ;++
		mov     eax, dword[esi+ecx*4]
		mov     dword[edi+ebx*4], eax
        inc     ecx
        cmp     ecx,[pSIZE]
        jne     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    qRadSortARRY,pArry,dArry,pCnt,pCoutn,pINC,pBYTE
        push_all
        mov     edx,[pINC]
        xor     ecx,ecx
.while_:
        mov     eax,[pArry]
        mov     eax,[eax]
        mov     eax,[eax+ecx*4]
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        mov     ebx, dword[edx+eax*4]
        inc     dword[edx+eax*4] ;++
        nop
        xor     eax,eax
.change_arry:
        mov     esi,[pArry]
        mov     edi,[dArry]
        mov     esi,[esi+eax*4]
        mov     edi,[edi+eax*4]

        push    dword[esi+ecx*4]
        push    dword[edi+ebx*4]
        pop     dword[esi+ecx*4]
        pop     dword[edi+ebx*4]


        inc     eax
        cmp     eax,[pCoutn]
        jb      .change_arry
        inc     ecx
        cmp     ecx,[pCnt]
        jne     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    dRadSortARRY,pArry,dArry,pCnt,pCoutn,pINC,pBYTE
        push_all
        mov     edx,[pINC]
        xor     ecx,ecx
.while_:
        mov     eax,[pArry]
        mov     eax,[eax]
        lea     eax,[eax+ecx*4]
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        mov     ebx, dword[edx+eax*4]
        inc     dword[edx+eax*4] ;++
        nop
        xor     eax,eax
.change_arry:
        mov     esi,[pArry]
        mov     edi,[dArry]
        mov     esi,[esi+eax*4]
        mov     edi,[edi+eax*4]

        push    dword[esi+ecx*4]
        push    dword[edi+ebx*4]
        pop     dword[esi+ecx*4]
        pop     dword[edi+ebx*4]


        inc     eax
        cmp     eax,[pCoutn]
        jb      .change_arry
        inc     ecx
        cmp     ecx,[pCnt]
        jne     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    RadSortADD,pINC
        push_all
        xor     ebx,ebx
        xor     ecx,ecx
        mov     edx,[pINC]
.while_:
        mov     eax, dword[edx+ecx*4]
        mov     dword[edx+ecx*4], ebx
        add     ebx, eax
        inc     ecx
        cmp     ecx,256
        jne     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    qRadSortADD,pINC
        push_all
        xor     ebx,ebx
        xor     ecx,ecx
        mov     edx,[pINC]
.while_:
        mov     eax, dword[edx+ecx*4]
        mov     dword[edx+ecx*4], ebx
        add     ebx, eax
        inc     ecx
        cmp     ecx,256
        jne     .while_
        pop_all
        ret
endp
;****************************************************************************************************
	 
;****************************************************************************************************
proc    RadSortINC,pDATA,pSIZE,pCnt,pBYTE
        push_all
        mov     esi,[pDATA]
        mov     edx,[pCnt]
        mov     ecx,[pSIZE]
        dec     ecx
.while_:
        lea     eax,[esi+ecx*4] ;;lowest byte first
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        inc     dword[edx+eax*4]
        sub     ecx,1
        jns     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;****************************************************************************************************
proc    qRadSortINC,pARRY,pSIZE,pCnt,pBYTE
        push_all
        mov     esi,[pARRY]
        mov     edx,[pCnt]
        mov     ecx,[pSIZE]
        dec     ecx
        mov     esi,[esi]
.while_:
        mov     eax,[esi+ecx*4] ;;lowest byte first
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        inc     dword[edx+eax*4]
        sub     ecx,1
        jns     .while_
        pop_all
        ret
endp
;****************************************************************************************************
                     
;****************************************************************************************************
proc    dRadSortINC,pARRY,pSIZE,pCnt,pBYTE
        push_all
        mov     esi,[pARRY]
        mov     edx,[pCnt]
        mov     ecx,[pSIZE]
        mov     esi,[esi]
        dec     ecx
.while_:
        lea     eax,[esi+ecx*4] ;;lowest byte first
        add     eax,[pBYTE]
        movzx   eax, byte[eax]
        inc     dword[edx+eax*4]
        sub     ecx,1
        jns     .while_
        pop_all
        ret
endp
;****************************************************************************************************

;********************************************************************************
proc FloatToString pBuffer,pCnt,pFloat
locals
  status_original             dw ?
  status_changed              dw ?
  integer                     dd ?
  mantisa                     dd ?
  signed                      dd ?
  float                       dd ?
  ftoa_ten                    dd ?
endl
        push_all
        mov     eax,[pFloat]
        test    eax, 80000000h
        jz      @F
        xor     eax, 80000000h
        @@:
        mov     [float],eax
        fld     dword [float]
        setnz   al
        movzx   eax,al
        mov     [signed], eax
        mov     [ftoa_ten],10


        fnstcw  [status_original]
        mov     ax, [status_original]
        or      ax, 0000110000000000b
        mov     [status_changed], ax
        fldcw   [status_changed]

        fld     st0
        fld     st0                     ; st0 = x, st1 = x
        frndint
        fist    [integer]               ; st0 = x, st1 = x
        fabs
        fsubp   st1, st0                ; st0 = mantisa(x)
        mov     edi, [pBuffer]
        mov     ecx,[signed]
        jecxz   .signed
        mov     al,"-"
        stosb
    .signed:
        mov     eax,[integer]
        stdcall RadixConvertEx,edi,10,0
        add     edi, eax
        mov     al, '.'
        stosb
        mov     ecx, [pCnt]
        dec     ecx
    .loop:
        fimul   [ftoa_ten]
        fld     st0
        frndint
        fist    [mantisa]
        fsubp   st1, st0
        mov     eax,[mantisa]
        stdcall RadixConvertEx,edi,10,1
        add     edi, eax
        ftst
        fnstsw  ax
        test    ax, 0100000000000000b
        jz      @F
        test    ax, 0000010100000000b
        jz      .finish
        @@:
        loop    .loop
        fldcw   [status_original]
        fimul   [ftoa_ten]
        fist    [mantisa]
		mov     eax,[mantisa]
		stdcall RadixConvertEx,edi,10,1
		add     edi, eax
    .finish:
        fstp    st0
        stc
		mov		eax,edi
		sub		eax,[pBuffer]
        pop_all
        ret
endp
;********************************************************************************

;********************************************************************************
proc StrFloatToHex, pStr, pBuff
locals
        temp            dd      0
        cword           dw      0
        multiplyer      rb      20
        const10         dd      10
endl

  push_all
  mov   ebx, [pStr]
  finit

  fldz
  mov   eax, 1
  cmp   byte [ebx], '-'
  jnz   @f
  or    eax,-1
  inc   ebx
 @@:
  push  eax

 .ipart:
  cmp   byte [ebx], '.'
  jz   .fpart0
  cmp   byte [ebx], 0
  jz   .fin

  fimul [const10]
  xor   eax, eax
  mov   al, byte [ebx]
  sub   al, '0'
  mov   [temp], eax
  fild  [temp]
  faddp st1, st0
  inc   ebx
  jmp  .ipart
 .fpart0:
  inc   ebx
  lea   ecx, [multiplyer]
  fld1
  fstp  tbyte [ecx]
 .fpart1:
  cmp   byte [ebx], '.'
  jz   .fin
  cmp   byte [ebx], 0
  jz   .fin
  xor   eax, eax
  mov   al, byte [ebx]
  sub   al, '0'
  mov   [temp], eax

  fld   TBYTE [ecx]
  fidiv [const10]
  fstp  TBYTE [ecx]
  fld   TBYTE [ecx]
  fild  [temp]
  fmulp st1, st0
  faddp st1, st0
  inc   ebx
  jmp  .fpart1
 .fin:

  pop   eax
  mov   [temp], eax
  fild  [temp]
  fmulp st1, st0

  mov   eax, [pBuff]
  ;fstp  TBYTE [eax]
  ;fstp  QWORD [eax]
  fstp  DWORD [eax]

  mov   eax, esi
  pop_all
  ret
endp
;********************************************************************************

;********************************************************************************
proc    FindFileRecursive,lpFStr,pCallBack
locals  ;"C:\Windows\",0
        hFind            dd ?
        hFindData        dd ?
        FindData         WIN32_FIND_DATA
        PatchBuff        rb 1024h
endl
        push_all
        xor     ebx,ebx
        lea     eax,[FindData]
        mov     [hFindData],eax
        lea     esi,[FindData.cFileName]
        lea     edi,[PatchBuff]
        mov     dword[edi],0
        stdcall _lstrcat,edi,[lpFStr]
        mov     dword[edi+eax],"*.*"
.find_file:
        invoke  FindFirstFile,edi,[hFindData]
        mov     [hFind],eax
        or      eax,eax
        je      .find_next_file
.find_first_file:
        cmp     byte[esi],"."
        je      .find_next_file
        cmp     word[esi],".."
        je      .find_next_file
        mov     eax,[FindData.dwFileAttributes]
        and     eax,FILE_ATTRIBUTE_DIRECTORY
        je      .find_file_ok
        mov     dword[edi],0
        stdcall _lstrcat,edi,[lpFStr]
        stdcall _lstrcat,edi,esi
        mov     dword[edi+eax],"\"
        lea     eax,[PatchBuff]
        stdcall FindFileRecursive,edi,[pCallBack]
        add     ebx,eax
        jmp     .find_next_file
.find_file_ok:
        inc     ebx
        cmp     [pCallBack],0
        je      .find_next_file
        stdcall dword[pCallBack],ebx,esi,[lpFStr]
.find_next_file:
        invoke  FindNextFile,[hFind],[hFindData]
        or      eax,eax
        jne     .find_first_file
        mov     eax,ebx
        pop_all
        ret
endp
;********************************************************************************


;***************************************************************************************************************
proc _HeapAlloc,pSize
    push_all
    invoke GetProcessHeap
    invoke HeapAlloc, eax, HEAP_ZERO_MEMORY, [pSize]
    pop_all
    ret
endp
;***************************************************************************************************************

;***************************************************************************************************************
proc _HeapFree,pSize
    push_all
    invoke GetProcessHeap
    invoke HeapFree, eax, HEAP_ZERO_MEMORY, [pSize]
    pop_all
    ret
endp
;***************************************************************************************************************


;*************************************************************
proc    _RtlIpv4AddressToString,pBuff
        push_all
        mov     edi,[pBuff]
        mov     ebx,eax
        xor     ecx,ecx
        @@:
        movzx   eax,bl
        shr     ebx,08
        stdcall RadixConvertEx,edi,10,0
        add     edi,eax
        inc     ecx
        cmp     ecx,4
        je      @f
        mov     al,"."
        stosb
        jmp     @b
        @@:
        sub     edi,[pBuff]
        mov     eax,edi
        pop_all
        ret
endp
;*************************************************************

;*************************************************************
proc    _RtlIpv4StringToAddress,pIP
        push_all
        mov     esi,[pIP]
        mov     edi,[pIP]
        stdcall _lstrlen,esi
        mov     ecx,eax
        xor     ebx,ebx
        @@:
        shl     ebx,8
        stdcall RadixDecodeEx,edi,10,3
        movzx   eax,al
        add     ebx,eax
        mov     al,"."
        repne   scasb
        je      @b
        mov     eax,ebx
        bswap   eax
        pop_all
        ret
endp
;*************************************************************

;*************************************************************
proc pDiv,pLen,pSize
    push_all    ;Len100  pSize10  result 10  100/10=10
    mov     eax,[pLen]
    xor     edx,edx
    mov     ebx,[pSize]
    div     ebx
    pop_all
    ret
endp
;*************************************************************

;*************************************************************
proc pDivEx,pLen,pSize
    push_all
    mov     eax,[pLen]
    xor     edx,edx
    mov     ebx,[pSize]
    div     ebx
    or      edx,edx
    setne   dl
    movzx   edx,dl
    add     eax,edx
    pop_all
    ret
endp
;*************************************************************

;*************************************************************
proc pMul,pLen,pSize
    push_all
    xor     edx,edx
    mov     eax,[pLen]
    mov     ebx,[pSize]
    mul     ebx
    pop_all
    ret
endp
;*************************************************************  