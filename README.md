
## ISO File

### Dependencies

```bash
sudo apt install xorriso mtools grub-pc-bin
```

### Create ISO

```bash
mkdir -p isodir/boot/grub
cp grub.cfg isodir/boot/grub
cp path/to/kernel isodir/boot
grub-mkrescue /usr/lib/grub/i386-pc -o maos.iso isodir
```
