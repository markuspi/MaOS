ENTRY(_start)

KERNEL_START  = 0x00100000;
KERNEL_OFFSET = 0xC0000000;

SECTIONS
{
    . = KERNEL_START;
    .multiboot.text :
    {
        *(.multiboot)
        *(.multiboot_text)
    }

    .multiboot.bss ALIGN(4K) :
    {
        *(.multiboot_bss)
    }

    . += KERNEL_OFFSET;
    _kernel_start = .;
    _kernel_start_phys = . - KERNEL_OFFSET;

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_OFFSET)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_OFFSET)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_OFFSET)
    {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;
    _kernel_end_phys = . - KERNEL_OFFSET;

}
