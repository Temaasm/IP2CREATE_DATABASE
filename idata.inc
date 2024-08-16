section '.idata' import data readable writeable

  library kernel32,'kernel32.dll',\
          user32,'user32.dll',\
          shell32,'shell32.dll',\
          comctl32,'comctl32.dll'

  import kernel32,\
         CloseHandle,'CloseHandle',\
         CreateFile,'CreateFileA',\
         ExitProcess,'ExitProcess',\
         FindFirstFile,'FindFirstFileA',\
         FindNextFile,'FindNextFileA',\
         GetFileSize,'GetFileSize',\
         GetModuleHandle,'GetModuleHandleA',\
         GetProcessHeap,'GetProcessHeap',\
         GlobalAlloc,'GlobalAlloc',\
         GlobalLock,'GlobalLock',\
         GlobalUnlock,'GlobalUnlock',\
         HeapAlloc,'HeapAlloc',\
         HeapFree,'HeapFree',\
         ReadFile,'ReadFile',\
         SetFilePointer,'SetFilePointer',\
         VirtualAlloc,'VirtualAlloc',\
         VirtualFree,'VirtualFree',\
         WriteFile,'WriteFile'

  import user32,\
         CloseClipboard,'CloseClipboard',\
         DialogBoxParam,'DialogBoxParamA',\
         EmptyClipboard,'EmptyClipboard',\
         EndDialog,'EndDialog',\
         GetDlgItemText,'GetDlgItemTextA',\
         OpenClipboard,'OpenClipboard',\
         SendMessage,'SendMessageA',\
         SetClipboardData,'SetClipboardData',\
         SetDlgItemText,'SetDlgItemTextA'

  import shell32,\
         DragAcceptFiles,'DragAcceptFiles',\
         DragQueryFile,'DragQueryFileA'

  import comctl32,\
         InitCommonControls,'InitCommonControls'