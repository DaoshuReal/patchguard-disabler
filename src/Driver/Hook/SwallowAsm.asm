.code

PgCli PROC
    cli
    ret
PgCli ENDP

PgSti PROC
    sti
    ret
PgSti ENDP

PgWriteCr8 PROC
    mov rax, rcx
    mov cr8, rax
    ret
PgWriteCr8 ENDP

PgWriteGsByte PROC
    mov r10d, ecx
    mov BYTE PTR gs:[r10], dl
    ret
PgWriteGsByte ENDP

END
