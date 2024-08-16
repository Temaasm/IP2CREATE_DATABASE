format PE GUI 4.0
entry main

include "win32ax.inc"    ;*
include "_MACRO.inc"    
include "parser.inc"    
include "rc.inc"        ;*

         
.data

         pBuff          rb  4096
         hInstance      dd  0

        _ERROR          db  "ERROR",0
        _START_PARSE    db  "PARSING FILE...",0        
        _END_PARSE      db  "COMPLETE...",0


        
section '.code' code readable writable executable 
main:
    invoke InitCommonControls
    invoke GetModuleHandle,0
    mov [hInstance],eax
    invoke DialogBoxParam,[hInstance],FTRAY,HWND_DESKTOP,dlg_proc,0         
    .exit:
    invoke ExitProcess,0





;***************************************************************************************************************
proc dlg_proc hwnd,msg,wparam,lparam
        push_all
        cmp     [msg],WM_NOTIFY
        je      .WM_NOTIFY
        cmp     [msg],WM_INITDIALOG
        je      .WM_INITDIALOG
        cmp     [msg],WM_COMMAND
        je      .WM_COMMAND
        cmp     [msg],WM_CLOSE
        je      .WM_CLOSE
        cmp     [msg],WM_TIMER
        je      .WM_TIMER
        cmp     [msg],WM_DROPFILES
        je      .WM_DROPFILES
        xor     eax,eax
        jmp     .finish
.WM_TIMER:
        jmp     .processed
.WM_NOTIFY:
        jmp     .processed
.WM_INITDIALOG:
        invoke  DragAcceptFiles,[hwnd],TRUE
        jmp     .processed
.WM_DROPFILES:
        invoke  DragQueryFile,[wparam],0,pBuff,pBuff.size
        invoke  SetDlgItemText,[hwnd],IDC_EDIT,pBuff
        jmp     .processed
.WM_COMMAND:
        .if     [wparam]=IDC_START
                invoke  GetDlgItemText,[hwnd],IDC_EDIT,pBuff,pBuff.size
                cmp     eax,4
                jb      .processed
        .if     dword[pBuff+eax-4]=".CSV"
                invoke  SendMessage,[hwnd],WM_SETTEXT,0,_START_PARSE
                stdcall ParseIP2,pBuff
                .if     eax=0
                        invoke  SendMessage,[hwnd],WM_SETTEXT,0,_ERROR
                        jmp     .processed     
                .endif
                stdcall pCreateBin
                stdcall pFreeMemory
                invoke  SendMessage,[hwnd],WM_SETTEXT,0,_END_PARSE
        .endif
        
        .endif
        jmp     .processed
.exit_true:
        mov eax,TRUE  
        jmp .finish
.exit_false:
        mov eax,FALSE  
        jmp .finish            
.WM_CLOSE:
        invoke  EndDialog,[hwnd],0
.processed:
        mov     eax,1
.finish:
        pop_all
        ret
endp  
;*************************************************************************************************************** 










;***************************************************************************************************************
proc SetClipboard,txtSerial
    local   sLen      dd  0
    local   hMem    dd  0
    local   pMem    dd  0

    push_all
        stdcall  _lstrlen, [txtSerial]
        inc eax
        mov [sLen], eax
        invoke OpenClipboard, 0
        invoke GlobalAlloc, GHND, [sLen]
        mov [hMem], eax
        invoke GlobalLock, eax
        mov [pMem], eax
        mov esi, [txtSerial]
        mov edi, eax
        mov ecx, [sLen]
        rep movsb
        invoke EmptyClipboard
        invoke GlobalUnlock, [hMem]
        invoke SetClipboardData, CF_TEXT, [hMem]
        invoke CloseClipboard
    pop_all 
    ret
endp    
;***************************************************************************************************************



IncludeAllGlobals
;=======================================================================
include 'idata.inc' ;*
;=======================================================================
;
;=======================================================================
section '.rsrc' resource from 'IP2CREATE_DATABASE.RES' data readable
;=======================================================================
