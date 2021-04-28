
#include "kernel/port.h"

#include "kernel/isr.h"

static isr_t interrupt_handlers[256];

static void isr_handle_page_fault(uint32_t err_code)
{
    uint32_t addr;
    asm volatile ("mov %%cr2, %0" : "=r" (addr));
}

static void isr_handle_exception(uint8_t int_no, uint32_t err_code)
{
    switch (int_no)
    {
    case 14:
        isr_handle_page_fault(err_code);
        break;    
    default:
        break;
    }
}

static void isr_handle_irq(uint8_t int_no)
{
    if (int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    if (interrupt_handlers[int_no] != 0) {
        isr_t handler = interrupt_handlers[int_no];
        handler(int_no);
    }
}

void isr_handler(registers_t regs)
{
    if (regs.int_no <= 31) {
        isr_handle_exception(regs.int_no, regs.err_code);
    } else {
        isr_handle_irq(regs.int_no);
    }
}
