
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
