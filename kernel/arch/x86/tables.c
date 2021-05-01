
#include "kernel/tables.h"

#include <stdint.h>
#include <string.h>

#include "kernel/port.h"

static gdt_entry_t gdt[5];
static dt_descriptor_t gdt_descriptor;
static idt_entry_t idt[256];
static dt_descriptor_t idt_descriptor;

static void gdt_set_entry(int num, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t flags) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_mid = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].flags_and_limit = (limit >> 16) & 0x0F;

    gdt[num].flags_and_limit |= (flags & 0xF0);
    gdt[num].access = access;
}

static void gdt_init() {
    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint32_t)&gdt;

    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush(&gdt_descriptor);
}

static void idt_set_entry(int num, void (*func)(), uint16_t sel,
                          uint8_t flags) {
    uint32_t base = (uint32_t)func;

    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt[num].flags = flags /* | 0x60 */;
}

static void idt_init() {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (uint32_t)&idt;

    memset(&idt, 0, sizeof(idt));

    // init and remap the irq table
    outb(0x20, 0x11);  // init PIC1
    outb(0xA0, 0x11);  // init PIC2
    outb(0x21, 0x20);  // offset 32 for PIC1
    outb(0xA1, 0x28);  // offset 40 for PIC2
    outb(0x21, 0x04);  // tell master PIC there is a SLAVE PIC at IRQ2
    outb(0xA1, 0x02);  // tell slave its identity (IRQ2)
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    // initialization done

    outb(0x21, 0xFF);  // mask (disable) interrupts
    outb(0xA1, 0xFF);  // mask (disable) interrupts

    // python3 -c "[print(f'idt_set_entry({i}, isr{i}, 0x08, 0x8E);') for i in
    // range(48)]"
    idt_set_entry(0, isr0, 0x08, 0x8E);
    idt_set_entry(1, isr1, 0x08, 0x8E);
    idt_set_entry(2, isr2, 0x08, 0x8E);
    idt_set_entry(3, isr3, 0x08, 0x8E);
    idt_set_entry(4, isr4, 0x08, 0x8E);
    idt_set_entry(5, isr5, 0x08, 0x8E);
    idt_set_entry(6, isr6, 0x08, 0x8E);
    idt_set_entry(7, isr7, 0x08, 0x8E);
    idt_set_entry(8, isr8, 0x08, 0x8E);
    idt_set_entry(9, isr9, 0x08, 0x8E);
    idt_set_entry(10, isr10, 0x08, 0x8E);
    idt_set_entry(11, isr11, 0x08, 0x8E);
    idt_set_entry(12, isr12, 0x08, 0x8E);
    idt_set_entry(13, isr13, 0x08, 0x8E);
    idt_set_entry(14, isr14, 0x08, 0x8E);
    idt_set_entry(15, isr15, 0x08, 0x8E);
    idt_set_entry(16, isr16, 0x08, 0x8E);
    idt_set_entry(17, isr17, 0x08, 0x8E);
    idt_set_entry(18, isr18, 0x08, 0x8E);
    idt_set_entry(19, isr19, 0x08, 0x8E);
    idt_set_entry(20, isr20, 0x08, 0x8E);
    idt_set_entry(21, isr21, 0x08, 0x8E);
    idt_set_entry(22, isr22, 0x08, 0x8E);
    idt_set_entry(23, isr23, 0x08, 0x8E);
    idt_set_entry(24, isr24, 0x08, 0x8E);
    idt_set_entry(25, isr25, 0x08, 0x8E);
    idt_set_entry(26, isr26, 0x08, 0x8E);
    idt_set_entry(27, isr27, 0x08, 0x8E);
    idt_set_entry(28, isr28, 0x08, 0x8E);
    idt_set_entry(29, isr29, 0x08, 0x8E);
    idt_set_entry(30, isr30, 0x08, 0x8E);
    idt_set_entry(31, isr31, 0x08, 0x8E);
    idt_set_entry(32, isr32, 0x08, 0x8E);
    idt_set_entry(33, isr33, 0x08, 0x8E);
    idt_set_entry(34, isr34, 0x08, 0x8E);
    idt_set_entry(35, isr35, 0x08, 0x8E);
    idt_set_entry(36, isr36, 0x08, 0x8E);
    idt_set_entry(37, isr37, 0x08, 0x8E);
    idt_set_entry(38, isr38, 0x08, 0x8E);
    idt_set_entry(39, isr39, 0x08, 0x8E);
    idt_set_entry(40, isr40, 0x08, 0x8E);
    idt_set_entry(41, isr41, 0x08, 0x8E);
    idt_set_entry(42, isr42, 0x08, 0x8E);
    idt_set_entry(43, isr43, 0x08, 0x8E);
    idt_set_entry(44, isr44, 0x08, 0x8E);
    idt_set_entry(45, isr45, 0x08, 0x8E);
    idt_set_entry(46, isr46, 0x08, 0x8E);
    idt_set_entry(47, isr47, 0x08, 0x8E);

    idt_flush(&idt_descriptor);
}

void irq_clear_mask(uint8_t line) {
    uint16_t port;
    uint8_t value;

    if (line < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        line -= 8;
    }
    value = inb(port) & ~(1 << line);
    outb(port, value);
}

void descriptor_tables_init() {
    gdt_init();
    idt_init();
}
