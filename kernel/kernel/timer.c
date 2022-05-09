#include "kernel/timer.h"

#include <stdio.h>

#include "kernel/isr.h"
#include "kernel/port.h"
#include "kernel/tables.h"
#include "kernel/types.h"

/*
 * https://wiki.osdev.org/Programmable_Interval_Timer
 */

static const u16 reload_value = 0xFFFF;
static u64 ns_per_tick = (reload_value * 1000000000LL) / 1193182LL;
static volatile u64 ns_since_boot = 0;

void timer_handler(uint8_t int_no) {
    ns_since_boot += ns_per_tick;
}

volatile int timer_compare(u64 a, u64 b) {
    return a > b;
}

void timer_init() {
    timer_compare(1,1);
    irq_register_handler(IRQ0, timer_handler);

    printf("[PIT] Reload value: %04x Delta %08lx\n", reload_value, ns_per_tick);

    // set reload value to 65535, resulting in a frequency of 18.2065 Hz
    outb(0x43, 0b00110100);  // channel 0, lo/hi, rate generator, 16 bit binary
    outb(0x40, reload_value & 0xFF);
    outb(0x40, reload_value >> 8);

    irq_clear_mask(0);
}

void sleep(u32 millis) {
    cli();
    u64 end_time = ns_since_boot + millis * 1000000LL;

    while (end_time > ns_since_boot) {
        sti();
        halt();
        cli();
    }

    sti();
}
