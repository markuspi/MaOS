#pragma once

#include "kernel/common.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

err_t serial_init(uint16_t port);

int serial_tx_empty();

void serial_putchar(char a);
