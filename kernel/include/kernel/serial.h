#pragma once

#include "kernel/common.h"
#include "kernel/port.h"

#define COM1 0x3F8

err_t serial_init(uint16_t port);

int serial_tx_empty();

void serial_putchar(char a);
