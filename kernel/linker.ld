ENTRY(maos_start)

KERNEL_OFFSET = 0xC0000000;
KERNEL_BOOT_MAPPED = 0xC0800000;

SECTIONS
{
    . = 0x00100000;
    KERNEL_START_PHYS = .;
    KERNEL_START = . + KERNEL_OFFSET;
    . += KERNEL_OFFSET;
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        *(.multiboot)
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

    KERNEL_END = .;
    KERNEL_END_PHYS = . - KERNEL_OFFSET;

}
