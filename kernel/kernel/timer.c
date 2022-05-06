#include "kernel/timer.h"

#include <stdio.h>

#include "kernel/isr.h"
#include "kernel/port.h"
#include "kernel/tables.h"
#include "kernel/types.h"

/*
 * https://wiki.osdev.org/Programmable_Interval_Timer
 */

void timer_handler(uint8_t int_no) {

}

void timer_init() {
    irq_register_handler(IRQ0, timer_handler);

    // set reload value to 65535, resulting in a frequency of 18.2065 Hz
    uint16_t reload_value = 0xFFFF;
    outb(0x43, 0b00110100);  // channel 0, lo/hi, rate generator, 16 bit binary
    outb(0x40, reload_value & 0xFF);
    outb(0x40, reload_value >> 8);

    irq_clear_mask(0);
}