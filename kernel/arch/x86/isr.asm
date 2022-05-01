
%macro ISR_NO_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push dword 0  ; dummy error code
    push dword %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli
    push dword %1
    jmp isr_common_stub
%endmacro

extern isr_handler
isr_common_stub:
    pushad  ; save all registers

    mov eax, [esp + 40] ; push old eip again
    push eax

    push ebp
    mov ebp, esp

    mov ax, ds  ; save segment
    push eax

    mov ax, 0x10  ; load kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop eax  ; restore the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    add esp, 8  ; pop old eip and ebp

    popad
    add esp, 8  ; pop interrupt number and error code
    sti
    iret

; python3 -c "[print(f'ISR_NO_ERRCODE {i}') for i in range(48)]"
ISR_NO_ERRCODE 0
ISR_NO_ERRCODE 1
ISR_NO_ERRCODE 2
ISR_NO_ERRCODE 3
ISR_NO_ERRCODE 4
ISR_NO_ERRCODE 5
ISR_NO_ERRCODE 6
ISR_NO_ERRCODE 7
ISR_ERRCODE 8
ISR_NO_ERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NO_ERRCODE 15
ISR_NO_ERRCODE 16
ISR_ERRCODE 17
ISR_NO_ERRCODE 18
ISR_NO_ERRCODE 19
ISR_NO_ERRCODE 20
ISR_NO_ERRCODE 21
ISR_NO_ERRCODE 22
ISR_NO_ERRCODE 23
ISR_NO_ERRCODE 24
ISR_NO_ERRCODE 25
ISR_NO_ERRCODE 26
ISR_NO_ERRCODE 27
ISR_NO_ERRCODE 28
ISR_NO_ERRCODE 29
ISR_NO_ERRCODE 30
ISR_NO_ERRCODE 31
ISR_NO_ERRCODE 32
ISR_NO_ERRCODE 33
ISR_NO_ERRCODE 34
ISR_NO_ERRCODE 35
ISR_NO_ERRCODE 36
ISR_NO_ERRCODE 37
ISR_NO_ERRCODE 38
ISR_NO_ERRCODE 39
ISR_NO_ERRCODE 40
ISR_NO_ERRCODE 41
ISR_NO_ERRCODE 42
ISR_NO_ERRCODE 43
ISR_NO_ERRCODE 44
ISR_NO_ERRCODE 45
ISR_NO_ERRCODE 46
ISR_NO_ERRCODE 47
