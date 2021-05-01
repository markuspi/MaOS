
#include "kernel/isr.h"

#include <stdio.h>

#include "kernel/common.h"
#include "kernel/port.h"

static isr_t interrupt_handlers[256];

static void isr_handle_page_fault(uint32_t err_code, uint32_t eip) {
    uint32_t addr;
    asm volatile("mov %%cr2, %0" : "=r"(addr));
    PANIC("Page Fault: addr=0x%08x code=0x%x eip=0x%08x", addr, err_code, eip);
}

static void isr_handle_exception(uint8_t int_no, uint32_t err_code,
                                 uint32_t eip) {
    switch (int_no) {
        case 14:
            isr_handle_page_fault(err_code, eip);
            break;
        default:
            printf("ISR EXCEPTION #%d\n", int_no);
            break;
    }
}

static void isr_handle_irq(uint8_t int_no) {
    if (int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    if (interrupt_handlers[int_no] != 0) {
        isr_t handler = interrupt_handlers[int_no];
        handler(int_no);
    } else {
        printf("IRQ%d\n", int_no - 32);
    }
}

void isr_handler(registers_t regs) {
    if (regs.int_no <= 31) {
        isr_handle_exception(regs.int_no, regs.err_code, regs.eip);
    } else {
        isr_handle_irq(regs.int_no);
    }
}

void irq_register_handler(uint8_t int_no, isr_t handler) {
    interrupt_handlers[int_no] = handler;
}
