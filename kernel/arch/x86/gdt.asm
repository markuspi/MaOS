
global gdt_flush
gdt_flush:
    ; first parameter is linear address of gdt descriptor
    mov eax, [esp+4]
    lgdt [eax]
    ; set CR0.PE (Protection Enable)
    mov eax, cr0
    or al, 1
    mov cr0, eax
    ; far jump, CS points to gdt entry 1 (our code segment)
    jmp 0x08:.flush

.flush:
    ; let all other segments point to entry 2 (our data segment)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global idt_flush
idt_flush:
    ; first parameter is linear address of idt descriptor
    mov eax, [esp+4]
    lidt [eax]
    sti
    ret
