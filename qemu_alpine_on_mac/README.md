# Running Alpine Linux in QEMU on Mac OSX

## Create disk image
```
$ qemu-img create -f qcow2 alpine.qcow2 8G
```

## Download Alpine
```
$ wget http://dl-cdn.alpinelinux.org/alpine/v3.11/releases/x86_64/alpine-standard-3.11.0_rc5-x86_64.iso .
```

## Setup Alpine
Follow steps here:
https://wiki.alpinelinux.org/wiki/Qemu

```
# Add -machine accel=hvf for MacOS version of KVM
$ qemu-system-x86_64 -m 512 -nic user -boot d -cdrom alpine-standard-3.11.3-x86_64.iso -hda alpine.qcow2 -machine accel=hvf
```

## Run Alpine on QEMU
```
$ qemu-system-x86_64 -m 512 -nic user -boot d -cdrom alpine-standard-3.11.0_rc5-x86_64.iso -hda alpine.qcow2
```
