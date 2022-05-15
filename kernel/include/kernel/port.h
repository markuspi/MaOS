#pragma once

#include <types.h>

void outb(u16 port, u8 value);
void outw(u16 port, u16 value);
void outl(u16 port, u32 value);

u8 inb(u16 port);
u16 inw(u16 port);
u32 inl(u16 port);
