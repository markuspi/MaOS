
global memcpy_aligned
memcpy_aligned:
    push ebp
    mov ebp, esp

    push edi
    push esi

    mov edi, [ebp+8] ; dest
    mov esi, [ebp+12] ; src
    mov ecx, [ebp+16] ; size
    shr ecx, 2

.loop:
    mov dword eax, [esi]
    mov dword [edi], eax
    add esi, 4
    add edi, 4
    dec ecx
    jnz .loop

    pop esi
    pop edi

    leave
    ret
