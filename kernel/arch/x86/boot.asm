; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS_   equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS_)   ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS_
	dd CHECKSUM

section .bss
align 4096
boot_page_directory:
    resb 4096
boot_page_table:
    resb 4096 * 2
boot_page_table_end:
stack_bottom:
    resb 16384  ; 16 KiB
stack_top:
boot_used_pages:
    resb 4

phys_page_directory: equ boot_page_directory - 0xC0000000
phys_page_table: equ boot_page_table - 0xC0000000
phys_page_table_end: equ boot_page_table_end - 0xC0000000
phys_used_pages: equ boot_used_pages - 0xC0000000

global boot_page_directory
global boot_page_table
global boot_page_table_end
global boot_used_pages

; ENTRY FUNCTION
section .multiboot_text
global _start:function (_start.end - _start)
_start:

    ; fill page directory with zeros
    mov edi, phys_page_directory
    mov ecx, 1024
.loop:
    mov dword [edi], 0
    add edi, 4
    dec ecx
    jnz .loop

    ; fill page table with valid entries
    mov edi, phys_page_table
    mov esi, 0
.loop2:
    mov eax, esi
    or eax, 0b11  ; set flags
    mov [edi], eax
    add esi, 4096
    add edi, 4
    cmp edi, phys_page_table_end
    jne .loop2

    mov dword [phys_used_pages], 1024

    ; create a PDE for first 8MB starting at 0xC0000000 virtual, pointing to 0x00000000 physical
    ; setting flags 0 (Present), 1 (Read/Write)
    mov dword [phys_page_directory + 0], phys_page_table + 0b11  ; identity mapping
    mov dword [phys_page_directory + 4 * 0x300], phys_page_table + 0x11  ; actual mapping
    mov dword [phys_page_directory + 4 * 0x301], phys_page_table + 0x11 + 4096

    ; store page directory location
    mov ecx, phys_page_directory
    mov cr3, ecx

    ; set CR0.31 (Paging)
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    ; long jump
    jmp 0x8:paged
.end:

section .text
paged:
    ; at this point, paging is enabled

    ; unmap identity mapping
    mov dword [boot_page_directory + 0], 0

    ; reload cr3 to force TLB flush
    mov ecx, cr3
    mov cr3, ecx

    ; set up the stack
    mov esp, stack_top

    extern kernel_main
    call kernel_main

.hang:
    hlt
    jmp .hang
