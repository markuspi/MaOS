#include "kernel/pci.h"

#include <stdio.h>

#include "kernel/common.h"
#include "kernel/port.h"

u32 pci_config_read_long(u8 bus, u8 slot, u8 func, u8 offset) {
    u32 address;
    u32 lbus  = (u32)bus;
    u32 lslot = (u32)slot;
    u32 lfunc = (u32)func;
 
    // Create configuration address as per Figure 1
    address = (u32)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xFC) | ((u32)0x80000000));
 
    // Write out the address
    outl(0xCF8, address);
    return inl(0xCFC);
}

u16 pci_config_read_word(u8 bus, u8 slot, u8 func, u8 offset) {
    u32 val = pci_config_read_long(bus, slot, func, offset);
    u32 shift = (offset & 0b10) * 8;
    return (u16) ((val >> shift) & 0xFFFF);
}

u8 pci_config_read_byte(u8 bus, u8 slot, u8 func, u8 offset) {
    u32 val = pci_config_read_long(bus, slot, func, offset);
    u32 shift = (offset & 0b11) * 8;
    return (u8) ((val >> shift) & 0xFF);
}

void pci_init() {
    for (int i = 0; i < 256; i++) {
        u16 vendor = pci_config_read_word(0, i, 0, 0);
        if (vendor == 0xFFFF) {
            continue;
        }
        u16 device = pci_config_read_word(0, i, 0, 2);
        u8 class_code = pci_config_read_byte(0, i, 0, 11);
        u8 subclass = pci_config_read_byte(0, i, 0, 10);
        DEBUG(DB_PCI, "[PCI] %d: %04x %04x %02x %02x\n", i, vendor, device, class_code, subclass);
    }

}


