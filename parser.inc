;*********************************************************************
proc    ParseIP2,gFile
locals
        pResult          dd      ?
        hFile            dd      ?
        hSize            dd      ?
        hMem             dd      ?
        pCnt             dd      ?   
        lpNumberOfBytesRead     dd  ?
endl
        push_all
        xor     ebx,ebx
        mov     [pResult],ebx
        mov     edx,[gFile]
        stdcall _RtlZeroMemory,pFILE_HEADER,sizeof.pFILE_HEADER_
        mov     dword[pFILE_HEADER.pSuM],"IPV4"
        stdcall _RtlZeroMemory,pFileName,0x256
        stdcall _lstrcat,pFileName,[gFile]
        cmp     dword[pFileName+eax-4],".CSV"
        jne     .ret
        mov     dword[pFileName+eax-4],ebx
        invoke  CreateFile,[gFile],GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
        mov     [hFile],eax
        invoke  GetFileSize,[hFile],0
        mov     [hSize],eax
        or      eax,eax
        je      .ret
        invoke  VirtualAlloc,0,[hSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        mov     [hMem],eax
        or      eax,eax
        je      .ret
        invoke  ReadFile,[hFile],[hMem],[hSize],addr lpNumberOfBytesRead,0
        nop
        stdcall LScanR,[hMem],[hSize],0,0
        mov     [pCnt],eax
        or      eax,eax
        je      .ret
        stdcall LScanR,[hMem],[hSize],[pCnt],RScanA
        cmp     eax,[pCnt]
        jne     .ret
        stdcall LScanR,[hMem],[hSize],[pCnt],RScanB
        cmp     eax,[pCnt]
        jne     .ret
        stdcall _RtlZeroMemory,[hMem],[hSize]
        mov     edx,[hMem]
        xor     ebx,ebx
.sort_offset:
        mov     edi,[pOFFSET_ARRY+ebx*4]
        mov     esi,[pARRY+ebx*4]
        or      esi,esi
        je      .sort_offset_end
        stdcall _RtlZeroMemory,[hMem],[hSize]
        stdcall SortArryStringEx,esi,[pCnt]
        stdcall AddOffsetToBuff,esi,edi,[pCnt],[hMem]
        stdcall WriteToMemStringFormat,[hMem],eax,ebx ;stdcall WriteToBuff,pStringFormat,[hMem],eax,ebx  
        or      eax,eax
        je      .ret
        stdcall HeapSortMod,esi,edi,[pCnt]
        mov     eax,[pCnt]
        shl     eax,2
        stdcall WriteToMemOffsetFormat,edi,eax,ebx  ;stdcall WriteToBuff,pOffsetFormat,edi,eax,ebx
        or      eax,eax
        je      .ret
        inc     [pFILE_HEADER.pCollum]
        inc     ebx
        cmp     ebx,MAX_COLLUM_COUNT
        jb      .sort_offset
.sort_offset_end:
        mov     eax,[pCnt]
        mov     [pFILE_HEADER.pCnt],eax
        xor     ebx,ebx
.free_memory:
        mov     eax,[pBUFF+ebx*4]
        invoke  VirtualFree,eax,0,MEM_RELEASE
        mov     eax,[pARRY+ebx*4]
        invoke  VirtualFree,eax,0,MEM_RELEASE
        mov     eax,[pOFFSET_ARRY+ebx*4]
        invoke  VirtualFree,eax,0,MEM_RELEASE
        inc     ebx
        cmp     ebx,[pFILE_HEADER.pCollum]
        jb      .free_memory
        invoke  VirtualFree,[hMem],0,MEM_RELEASE
        invoke  CloseHandle,[hFile]
        mov     [pResult],pFILE_HEADER
.ret:
        mov     eax,[pResult]
        pop_all
        ret
endp
;*********************************************************************


;*********************************************************************
proc    WriteToBuff,pFormat,pBuff,pSize,pCnt
locals
        lpNumberOfBytesRead     dd  ?
endl
iglobal
        pStringFormat   db      "IP2LOCATION_STRING_%d.BIN",0
        pOffsetFormat   db      "IP2LOCATION_OFFSET_%d.BIN",0
        pFileName       rb      0x256
        pFormatBuff     rb      0x256
endg
        push_all
        xor     ebx,ebx
        stdcall _RtlZeroMemory,pFormatBuff,0x256
        stdcall _sprintf,pFormatBuff,[pFormat],[pCnt]
        add     esp,4*1
        nop
        invoke  CreateFile,pFormatBuff,GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
        or      eax,eax
        je      .ret
        cmp     eax,-1
        je      .ret
        mov     esi,eax
        invoke  WriteFile,esi,[pBuff],[pSize],addr lpNumberOfBytesRead,0
        invoke  CloseHandle,esi
        inc     ebx
.ret:
        mov     eax,ebx
        pop_all
        ret
endp
;*********************************************************************

;*********************************************************************
proc    WriteToMemStringFormat,pBuff,pSize,pCnt
        push_all
        mov     ebx,[pCnt]
        invoke  VirtualAlloc,0,[pSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        or      eax,eax
        je      .ret
        mov     [pSTRING+ebx*4],eax
        mov     ecx,[pSize]
        mov     [pSTRING_SIZE+ebx*4],ecx
        stdcall _RtlCopyMemory,eax,[pBuff],ecx
.ret:
        pop_all
        ret
endp
;*********************************************************************

;*********************************************************************
proc    WriteToMemOffsetFormat,pBuff,pSize,pCnt
        push_all
        mov     ebx,[pCnt]
        invoke  VirtualAlloc,0,[pSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        or      eax,eax
        je      .ret
        mov     [pOFFSET+ebx*4],eax
        mov     ecx,[pSize]
        mov     [pOFFSET_SIZE+ebx*4],ecx
        stdcall _RtlCopyMemory,eax,[pBuff],ecx
.ret:
        pop_all
        ret
endp
;*********************************************************************

;*********************************************************************
proc    AddOffsetToBuff,pArry,pOffset,pCnt,pMemBuff
        push_all
        mov     esi,[pArry]
        mov     edi,[pMemBuff]
        mov     ecx,[pOffset]
        xor     ebx,ebx
.add_offset:
        mov     edx,[esi+ebx*4]
        stdcall _lstrcmpi,edi,edx
        je      .next
        stdcall _lstrlen,edi
        inc     eax
        add     edi,eax
        stdcall _lstrcat,edi,edx
.next:
        mov     eax,edi
        sub     eax,[pMemBuff]
        mov     [ecx+ebx*4],eax
        inc     ebx
        cmp     ebx,[pCnt]
        jb      .add_offset
        stdcall _lstrlen,edi
        lea     eax,[edi+eax+1]
        sub     eax,[pMemBuff]
        pop_all
        ret
endp
;*********************************************************************


;***********************************************************************
proc    LScanR,pDATA,pSIZE,pCnt,pProc
                push_all
                mov     edi,[pDATA]
                mov     ecx,[pSIZE]
                or      ebx,-1
.new_line:
                inc     ebx
                xor     edx,edx
.find_line:
                mov     eax,'"'
                repne   scasb
                jecxz   .ret
                mov     esi,edi
                mov     [edi-1],ah
                repne   scasb
                jecxz   .ret
                mov     [edi-1],ah
                mov     eax,edi
                sub     eax,esi
.start_point:
                cmp     [pCnt],0
                je      .end_point
                stdcall dword[pProc],esi,eax,edx,ebx,[pCnt]
                or      eax,eax
                je      .ret
.end_point:
                mov     al,'"'
                mov     [esi-1],al
                mov     [edi-1],al
                cmp     byte[edi],0x0A
                je      .new_line
                cmp     byte[edi+1],0x0A
                je      .new_line
                inc     edx
                jmp     .find_line
.ret:
                mov     eax,ebx
                pop_all
                ret
endp
;***********************************************************************

;***********************************************************************
proc    RScanA,pDATA,pSize,pInc,pCnt,pCount
iglobal
        pLEN            rd      MAX_COLLUM_COUNT
endg
        push_all
        .if     [pCnt]=0        &       [pInc]=0
                stdcall _RtlZeroMemory,pLEN,MAX_COLLUM_COUNT*4
        .endif
        mov     esi,[pDATA]
        mov     ebx,[pInc]
        mov     ecx,[pSize]
        add     [pLEN+ebx*4],ecx
        mov     eax,1
.ret:
        pop_all
        ret
endp
;***********************************************************************

;***********************************************************************
proc    RScanB,pDATA,pSize,pInc,pCnt,pCount
iglobal

                MAX_COLLUM_COUNT	=	50
				pOFFSET         rd      MAX_COLLUM_COUNT
                pBUFF           rd      MAX_COLLUM_COUNT
                pARRY           rd      MAX_COLLUM_COUNT
                pOFFSET_ARRY    rd      MAX_COLLUM_COUNT
endg
        push_all
        .if     [pCnt]=0        &       [pInc]=0
                xor     ebx,ebx
                stdcall _RtlZeroMemory,pOFFSET,MAX_COLLUM_COUNT*4
                stdcall _RtlZeroMemory,pBUFF,MAX_COLLUM_COUNT*4
                stdcall _RtlZeroMemory,pARRY,MAX_COLLUM_COUNT*4
                mov     esi,[pCount]
                shl     esi,2
                @@:
                mov     eax,[pLEN+ebx*4]
                or      eax,eax
                je      @f
                invoke  VirtualAlloc,0,eax,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
                mov     [pBUFF+ebx*4],eax
                or      eax,eax
                je      .ret
                invoke  VirtualAlloc,0,esi,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
                mov     [pARRY+ebx*4],eax
                or      eax,eax
                je      .ret
                invoke  VirtualAlloc,0,esi,MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
                mov     [pOFFSET_ARRY+ebx*4],eax
                or      eax,eax
                je      .ret
                inc     ebx
                cmp     ebx,MAX_COLLUM_COUNT
                jb      @b
                @@:
        .endif



        mov     esi,[pDATA]
        mov     ebx,[pInc]
        mov     ecx,[pSize]
        mov     edx,[pCnt]


        mov     edi,[pBUFF+ebx*4]
        add     edi,[pOFFSET+ebx*4]
        mov     eax,[pARRY+ebx*4]
        mov     [eax+edx*4],edi
        nop
        stdcall _lstrcat,edi,esi
        inc     eax
        add     [pOFFSET+ebx*4],eax
        mov     eax,1
.ret:
        pop_all
        ret
endp
;*********************************************************************** 



;****************************************************************************
proc    pOpenFile
iglobal
        COLLUM_COUNT	=	50
        pSTRING         rd      COLLUM_COUNT
        pSTRING_SIZE    rd      COLLUM_COUNT
        pOFFSET_SIZE    rd      COLLUM_COUNT
endg
locals
        hFile           dd      ?
        pNumbers        dd      ?
        hMem            dd      ?
        hSize           dd      ?
        pCnt            dd      ?
        pResult         dd      ?
endl

iglobal
        pFILE_HEADER  pFILE_HEADER_
        pOFFSET_FILE    db      "IP2LOCATION_OFFSET_%d.BIN",0
        pSTRING_FILE    db      "IP2LOCATION_STRING_%d.BIN",0
        ;pFormatBuff     rb      0x256
endg
        push_all
        xor     ebx,ebx
        mov     [pResult],ebx
        mov     [pCnt],ebx
        stdcall _RtlZeroMemory,pSTRING,COLLUM_COUNT*4
        stdcall _RtlZeroMemory,pOFFSET,COLLUM_COUNT*4
        stdcall _RtlZeroMemory,pOFFSET_SIZE,COLLUM_COUNT*4
        stdcall _RtlZeroMemory,pSTRING_SIZE,COLLUM_COUNT*4
        stdcall _RtlZeroMemory,pFILE_HEADER,sizeof.pFILE_HEADER_
        mov     dword[pFILE_HEADER.pSuM],"IPV4"
.read_offset:
        stdcall _RtlZeroMemory,pFormatBuff,0x256
        stdcall _sprintf,pFormatBuff,pSTRING_FILE,ebx
        add     esp,4
        invoke  CreateFile,pFormatBuff,GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
        mov     [hFile],eax
        invoke  GetFileSize,[hFile],0
        mov     [hSize],eax
        or      eax,eax
        je      .read_end
        invoke  VirtualAlloc,0,[hSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        mov     [hMem],eax
        or      eax,eax
        je      .ret
        invoke  ReadFile,[hFile],[hMem],[hSize],addr pNumbers,0
        mov     eax,[hMem]
        mov     [pSTRING+ebx*4],eax
        mov     ecx,[hSize]
        mov     [pSTRING_SIZE+ebx*4],ecx
        invoke  CloseHandle,[hFile]
        nop
        stdcall _RtlZeroMemory,pFormatBuff,0x256
        stdcall _sprintf,pFormatBuff,pOFFSET_FILE,ebx
        add     esp,4
        invoke  CreateFile,pFormatBuff,GENERIC_READ+GENERIC_WRITE,FILE_SHARE_READ+FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
        mov     [hFile],eax
        invoke  GetFileSize,[hFile],0
        mov     [hSize],eax
        or      eax,eax
        je      .read_end
        invoke  VirtualAlloc,0,[hSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        mov     [hMem],eax
        or      eax,eax
        je      .ret
        invoke  ReadFile,[hFile],[hMem],[hSize],addr pNumbers,0
        mov     eax,[hMem]
        mov     [pOFFSET+ebx*4],eax
        mov     ecx,[hSize]
        mov     [pOFFSET_SIZE+ebx*4],ecx
        invoke  CloseHandle,[hFile]
        .if     ebx=0
                mov     eax,[hSize]
                shr     eax,2
                mov     [pFILE_HEADER.pCnt],eax
        .endif
        inc     [pFILE_HEADER.pCollum]
        inc     ebx
        cmp     ebx,COLLUM_COUNT
        jb      .read_offset
.read_end:
        mov     eax,[pFILE_HEADER.pCnt]
.ret:
        pop_all
        ret
endp
;****************************************************************************

;****************************************************************************
proc    pCreateBin
locals
    pResult                 dd  ?
    hFile                   dd  ?
    pSize                   dd  ?
    hMem                    dd  ?
    lpNumberOfBytesRead     dd  ?
endl
iglobal
    pIP2LOCATION_BIN     db  "IP2LOCATION.BIN",0
struct  pFILE_HEADER_
            pSuM        dd  0
            pCnt        dd  0
            pCollum     dd  0
            pBASE       rd  COLLUM_COUNT
ends
endg
        push_all
        xor     ebx,ebx
        mov     [pResult],ebx
        invoke  CreateFile,pIP2LOCATION_BIN,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
        mov     [hFile],eax
        mov     ecx,[pFILE_HEADER.pCollum]
        shl     ecx,2
        stdcall pMul,ecx,[pFILE_HEADER.pCnt]
        mov     [pSize],eax
        invoke  VirtualAlloc,0,[pSize],MEM_COMMIT+MEM_RESERVE,PAGE_EXECUTE_READWRITE
        mov     [hMem],eax
        or      eax,eax
        je      .ret
        mov     esi,sizeof.pFILE_HEADER_
        add     esi,[pSize]
        invoke  SetFilePointer,[hFile],esi,NULL, FILE_BEGIN
.write_header:
        ;-------------------------------------------
        .if     ebx<2   ;pack
                mov     [pSTRING_SIZE+ebx*4],NULL
        .endif  
        ;-------------------------------------------
        invoke  WriteFile,[hFile],[pSTRING+ebx*4],[pSTRING_SIZE+ebx*4],addr lpNumberOfBytesRead,0
        mov     [pFILE_HEADER.pBASE+ebx*4],esi
        add     esi,[pSTRING_SIZE+ebx*4]
        inc     ebx
        cmp     ebx,[pFILE_HEADER.pCollum]
        jb      .write_header    
        invoke  SetFilePointer,[hFile],0,NULL, FILE_BEGIN
        invoke  WriteFile,[hFile],pFILE_HEADER,sizeof.pFILE_HEADER_,addr lpNumberOfBytesRead,0
        xor     ebx,ebx
        xor     ecx,ecx
        mov     esi,[hMem]
.write_data:
        mov     edx,[pOFFSET+ecx*4]
        mov     edx,[edx+ebx*4]
        add     edx,[pFILE_HEADER.pBASE+ecx*4]
        ;-------------------------------------------
        .if     ecx<2   ;pack
                mov     edx,[pOFFSET+ecx*4]
                mov     edx,[edx+ebx*4]
                add     edx,[pSTRING+ecx*4]
                stdcall RadixDecode,edx,10
                mov     edx,eax
        .endif  
        ;-------------------------------------------
        mov     [esi+ecx*4],edx
        inc     ecx
        cmp     ecx,[pFILE_HEADER.pCollum]
        jb      .write_data
        shl     ecx,2
        add     esi,ecx
        xor     ecx,ecx
        inc     ebx
        cmp     ebx,[pFILE_HEADER.pCnt]
        jb      .write_data
        invoke  WriteFile,[hFile],[hMem],[pSize],addr lpNumberOfBytesRead,0
        invoke  VirtualFree,[hMem],0,MEM_RELEASE
        invoke  CloseHandle,[hFile]
        mov     [pResult],1
.ret:
        mov     eax,[pResult]
        pop_all
        ret
endp
;****************************************************************************


;****************************************************************************
proc    pFreeMemory
        push_all
        xor     ebx,ebx
.free_memory:
        mov     eax,[pSTRING+ebx*4]
        or      eax,eax
        je      .free_memory_end
        invoke  VirtualFree,eax,0,MEM_RELEASE
        mov     eax,[pOFFSET+ebx*4]
        or      eax,eax
        je      .free_memory_end 
        invoke  VirtualFree,eax,0,MEM_RELEASE    
        inc     ebx
        cmp     ebx,COLLUM_COUNT
        jb      .free_memory
.free_memory_end:
        pop_all
	    ret
endp
;****************************************************************************  

