
#include "kernel/isr.h"

#include <stdio.h>

#include "kernel/common.h"
#include "kernel/port.h"
#include "kernel/vm.h"

static isr_t interrupt_handlers[256];

static void isr_handle_page_fault(uint32_t err_code, uint32_t eip) {
    uint32_t addr;
    asm volatile("mov %%cr2, %0" : "=r"(addr));
    size_t dir_idx = addr >> 22;
    size_t tab_idx = (addr >> 12) & MASK(10);
    size_t pag_idx = addr & MASK(12);

    printf("\nPAGE FAULT addr=0x%08x DIR=%d TAB=%d ADR=%d\n", addr, dir_idx, tab_idx, pag_idx);

    if (current_as) {
        printf("Directory is located at 0x%08x, Dir entry at 0x%08x\n", current_as->directory_phys,
               current_as->directory_phys + 4 * dir_idx);
        pde_t dir_entry = current_as->directory->entries[dir_idx];
        printf("Dir Entry: 0x%08x P:%d RW:%d Frame:%d\n", dir_entry, dir_entry.present,
               dir_entry.rw, dir_entry.page_table);
        if (dir_entry.present) {
            printf("Table is located at 0x%08x, Table entry at 0x%08x\n",
                   dir_entry.page_table * PAGE_SIZE,
                   dir_entry.page_table * PAGE_SIZE + 4 * tab_idx);
            pte_t page_entry = current_as->tables[dir_idx]->entries[tab_idx];
            printf("Tab Entry: 0x%08x P:%d RW:%d Frame:%d\n", page_entry, page_entry.present,
                   page_entry.rw, page_entry.frame);
        }
    }

    PANIC("Page Fault: addr=0x%08x code=0x%x eip=0x%08x\n", addr, err_code, eip);
}

static void isr_handle_exception(uint8_t int_no, uint32_t err_code, uint32_t eip) {
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
