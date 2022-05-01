; Declare constants for the multiboot header.
MB_ALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MB_MEMINFO  equ  1 << 1            ; provide memory map
MB_FLAGS   equ  MB_ALIGN | MB_MEMINFO ; this is the Multiboot 'flag' field
MB_MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)   ; checksum of above, to prove we are multiboot

; Declare constants for page tables
PT_P  equ 1 << 0 ; Present bit: if set, the page is valid and present
PT_RW equ 1 << 1 ; Read/Write bit: if set, the page is read/writeable
PT_PS equ 1 << 7 ; Page Size bit: if set, page directory entry directly points to a 4 MiB page

VM_OFFSET equ 0xC0000000 ; Virtual memory offset for higher-half kernel
PDE_INDEX equ VM_OFFSET >> 22

; Declare constants for control registers
CR0_PG equ 1 << 31 ; Paging
CR4_PSE equ 1 << 4 ; Page size extension


; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM

section .bss
align 4096
stack_bottom:
    resb 16 * 1024  ; 16 KiB
stack_top:
KERNEL_BSS_ZERO_START:
KERNEL_BOOT_PAGE_DIRECTORY:
    resb 4096
global KERNEL_PAGE_DIRECTORY
KERNEL_PAGE_DIRECTORY:
    resb 4096
global KERNEL_PAGE_TABLES
KERNEL_PAGE_TABLES:
    resb 4 * 1024 * 1024
KERNEL_BSS_ZERO_END:

phys_boot_page_directory equ (KERNEL_BOOT_PAGE_DIRECTORY - VM_OFFSET)

; ENTRY FUNCTION
section .multiboot_text
global _start:function
_start:
    mov eax, VM_OFFSET

    ; fill page directory and page tables with zeros
    mov edi, (KERNEL_BSS_ZERO_START - VM_OFFSET)
    mov ecx, (KERNEL_BSS_ZERO_END - VM_OFFSET)
.loop_fill_zeros:
    mov dword [edi], 0
    add edi, 4
    cmp edi, ecx
    jl .loop_fill_zeros

    ; create identity map and regular map for whole kernel
    mov ecx, 0
    mov eax, (PT_P | PT_RW | PT_PS)
    mov edi, phys_boot_page_directory
    mov dword [edi + ecx * 4], eax
.loop_page_directory:
    mov dword [edi + ecx * 4 + (0x300 * 4)], eax
    add eax, (1 << 22)
    inc ecx
    cmp ecx, 2 ; put number of pages here
    jl .loop_page_directory

    ; store page directory location
    mov ecx, phys_boot_page_directory
    mov cr3, ecx

    ; enable page size extension (4 MiB pages)
    mov ecx, cr4
    or ecx, CR4_PSE
    mov cr4, ecx

    ; enable paging
    mov ecx, cr0
    or ecx, CR0_PG
    mov cr0, ecx

    ; long jump
    jmp 0x8:paged

section .text
paged:
    ; at this point, paging is enabled

    ; unmap identity mapping
    mov dword [KERNEL_BOOT_PAGE_DIRECTORY], 0

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
