#include "kernel/serial.h"

#include "kernel/port.h"
#include "kernel/isr.h"
#include "kernel/tables.h"

/*
 * https://wiki.osdev.org/Serial_Ports
 */

static uint16_t serial_active_port = 0;

void serial_handler(uint8_t  int_no) {
    char c = inb(serial_active_port);
    serial_putchar(c);
}

int serial_rx_data_ready() {
    return inb(COM1 + 5) & 0x01;
}

err_t serial_init(uint16_t port) {
    irq_register_handler(IRQ4, serial_handler);

    serial_active_port = 0;
    outb(port + 1, 0x00);  // Disable all interrupts
    outb(port + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);  //                  (hi byte)
    outb(port + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E);  // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE);  // Test serial chip

    if (inb(port + 0) != 0xAE) {
        return E_DEVICE;
    }

    serial_active_port = port;
    outb(port + 4, 0x0F);  // Enable normal operation mode
    outb(port + 1, 0x01);  // Enable interrupts for DATA_AVAILABLE

    irq_clear_mask(4);
    while(serial_rx_data_ready()) inb(port);

    return E_OK;
}

int serial_tx_empty() {
    return inb(COM1 + 5) & 0x20;
}

void serial_putchar(char a) {
    if (a == '\n') {
        serial_putchar('\r');
    }
    while (serial_tx_empty() == 0) {
        // NOP
    }

    outb(serial_active_port, a);
}
