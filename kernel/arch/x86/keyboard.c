
#include "kernel/keyboard.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "kernel/common.h"
#include "kernel/isr.h"
#include "kernel/port.h"
#include "kernel/tables.h"

static const uint16_t port_status = 0x64;
static const uint16_t port_command = 0x64;
static const uint16_t port_data = 0x60;

static void flush_input_buffer() {
    while (inb(port_status) & 0b10) {
    }
}

static void flush_output_buffer() {
    while (inb(port_status) & 0b1) {
        inb(port_data);
    }
}

static void send_command(uint8_t cmd) {
    outb(port_command, cmd);
    flush_input_buffer();
}

static void send_data(uint8_t data) {
    outb(port_data, data);
    flush_input_buffer();
}

static uint8_t expect_data() {
    while (!(inb(port_status) & 0b1)) {
    }
    return inb(port_data);
}

void keyboard_handler(uint8_t int_no) {
    uint8_t data = inb(port_data);
    printf("0x%x\n", data);
}

void keyboard_init() {
    uint8_t tmp;
    uint8_t conf;

    irq_register_handler(33, keyboard_handler);
    irq_clear_mask(1);  // enable interrupts for IRQ1

    send_command(0xAD);     // disable first PS/2 port
    send_command(0xA7);     // disable second PS/2 port
    flush_output_buffer();  // clear buffer

    send_command(0x20);  // read controller configuration byte
    conf = expect_data();
    // printf("Keyboard conf: %d\n", (uint32_t)conf);
    conf &= 0b10111100;  // clear bits 0, 1, 6
    send_command(0x60);  // store new config
    send_data(conf);

    send_command(0xAA);  // perform self-test
    tmp = expect_data();
    ASSERT(tmp == 0x55, "Keyboard Self Test fail (0x%x)", tmp);

    send_command(0xAB);  // test first port
    tmp = expect_data();
    ASSERT(tmp == 0x00, "Keyboard first port fail (0x%x)", tmp);

    send_command(0xAE);  // enable first port
    send_command(0x20);  // read controller configuration byte
    conf = expect_data();
    conf |= 0b00000001;  // set bits 0
    send_command(0x60);
    send_data(conf);
}
