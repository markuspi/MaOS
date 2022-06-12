# MaOS

Custom WIP x86 Operating System from scratch just for fun.
Follow [these](https://wiki.osdev.org/GCC_Cross-Compiler) instructions to set up cross compile toolchain.

## Features

* Paging
* Memory management
* Serial port
* Keyboard
* Basic VGA graphics (WIP)

## ISO File

### Dependencies

```bash
sudo apt install xorriso mtools grub-pc-bin
```

### Create ISO

Use CMake target `maos.iso` or manually:

```bash
mkdir -p iso/boot/grub
cp grub.cfg iso/boot/grub
cp path/to/kernel iso/boot
grub-mkrescue -o maos.iso iso
```
