# README: NFS Root Boot with BusyBox on Raspberry Pi 3B+

## A Complete Step-by-Step Guide

------

## Table of Contents

1. [Overview](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#overview)
2. [Prerequisites](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#prerequisites)
3. [Network Setup](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#network-setup)
4. [Phase 1: Boot Kernel via TFTP (First Boot)](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-1-boot-kernel-via-tftp)
5. [Phase 2: Build BusyBox](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-2-build-busybox)
6. [Phase 3: Create NFS Root Filesystem](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-3-create-nfs-root-filesystem)
7. [Phase 4: Setup NFS Server](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-4-setup-nfs-server)
8. [Phase 5: Setup TFTP Server](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-5-setup-tftp-server)
9. [Phase 6: Fix bootargs (config.txt)](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-6-fix-bootargs)
10. [Phase 7: Boot with NFS + BusyBox](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#phase-7-boot-with-nfs--busybox)
11. [Troubleshooting Journey](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#troubleshooting-journey)
12. [Final Working Setup](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#final-working-setup)
13. [Filesystem Structure](https://playground.outlier.ai/conversation/69c56b8de47ceb2f4a8a6437#filesystem-structure)

------

## Overview

The goal is to boot a Raspberry Pi 3B+ over the network:

- **U-Boot** as bootloader
- **TFTP** to load the kernel image
- **NFS** as root filesystem (no SD card rootfs needed)
- **BusyBox** as the minimal userspace (init + shell + utilities)

------

## Prerequisites

- Raspberry Pi 3B+ with U-Boot installed on SD card

- Host machine (Ubuntu) connected to RPi via Ethernet

- Cross-compilation toolchain:

  text

  ```text
  /home/abdo/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
  ```

- Pre-built Linux kernel Image for RPi 3B+ (aarch64)

------

## Network Setup

| Device                   | IP Address    |
| ------------------------ | ------------- |
| Host (NFS + TFTP server) | 10.42.0.1     |
| RPi (client)             | 10.42.0.2     |
| Netmask                  | 255.255.255.0 |

------

## Phase 1: Boot Kernel via TFTP

### Goal: Just see if the kernel boots at all

From U-Boot console:

text

```text
tftp ${kernel_addr_r} Image
setenv bootargs console=ttyAMA0,115200 earlycon=pl011,0x3f201000 loglevel=8 panic=5
booti ${kernel_addr_r} - ${fdt_addr}
```

### Result: Kernel Panic

text

```text
Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
```

### What this means:

- ✅ Kernel loaded successfully via TFTP
- ✅ Kernel booted and printed output
- ❌ No root filesystem — kernel has nowhere to find init

This is expected. We haven't told the kernel where to find a rootfs yet. We need to:

1. Build BusyBox (our userspace)
2. Set up NFS (our rootfs delivery method)
3. Update bootargs to point to NFS

------

## Phase 2: Build BusyBox

### Download and extract:

bash

```bash
wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar xjf busybox-1.36.1.tar.bz2
cd busybox-1.36.1
```

### Configure:

bash

```bash
export CROSS_COMPILE=/home/abdo/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
export ARCH=arm64

make defconfig
make menuconfig
```

### In menuconfig, change these settings:

text

```text
→ Settings → Build static binary (no shared libs) → YES
→ Settings → SHA1: Use hardware accelerated instructions → NO
→ Settings → SHA256: Use hardware accelerated instructions → NO
```

> **Why static?** A static binary has no dependencies on shared libraries.
>  If you build dynamically, you must also copy all shared libs from your
>  toolchain sysroot to the rootfs. Static is simpler.

> **Why disable SHA hardware acceleration?** There is a known build bug
>  in BusyBox that causes this error when cross-compiling:
>
> text
>
> ```text
> error: 'sha1_process_block64_shaNI' undeclared
> ```
>
> Disabling it fixes the build.

### Build and install:

bash

```bash
make clean
make -j$(nproc)
make install
```

### Verify the binary:

bash

```bash
file _install/bin/busybox
```

**Expected (static):**

text

```text
ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), statically linked, stripped
```

**If you see dynamically linked instead:**
 You either need to rebuild as static, or copy the shared libs (see Phase 3).

Output is in _install/ directory:

text

```text
_install/
├── bin/busybox (+ symlinks: sh, ls, cat, mount, etc.)
├── sbin/ (init, ifconfig, reboot, etc. → all symlinks to ../bin/busybox)
├── usr/
└── linuxrc → bin/busybox
```

------

## Phase 3: Create NFS Root Filesystem

### Copy BusyBox output to NFS directory:

bash

```bash
sudo mkdir -p /srv/nfs/rootfs
sudo cp -a _install/* /srv/nfs/rootfs/
```

### Create essential directories:

bash

```bash
cd /srv/nfs/rootfs
sudo mkdir -p proc sys dev etc/init.d tmp
```

### If BusyBox is dynamically linked, copy shared libs:

bash

```bash
SYSROOT=/home/abdo/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot

sudo mkdir -p /srv/nfs/rootfs/lib /srv/nfs/rootfs/lib64
sudo cp -a ${SYSROOT}/lib/* /srv/nfs/rootfs/lib/
sudo cp -a ${SYSROOT}/lib64/* /srv/nfs/rootfs/lib64/
```

> **Why?** A dynamically linked binary needs:
>
> - /lib/ld-linux-aarch64.so.1 (the dynamic linker)
> - /lib/libc.so.* (C library)
> - Other libs depending on what BusyBox uses
>
> These exist in your cross-toolchain's sysroot. Without them, the kernel
>  will fail with init failed (error -2) because it can't execute the binary.

### Create device nodes:

bash

```bash
sudo mknod -m 622 /srv/nfs/rootfs/dev/console c 5 1
sudo mknod -m 666 /srv/nfs/rootfs/dev/null c 1 3
```

> **Why?** The kernel needs /dev/console to open an initial console for init.
>  Without it you get: Warning: unable to open an initial console.

### Create /etc/inittab:

bash

```bash
sudo tee /srv/nfs/rootfs/etc/inittab << 'EOF'
::sysinit:/bin/mount -t proc proc /proc
::sysinit:/bin/mount -t sysfs sysfs /sys
::respawn:-/bin/sh
::shutdown:/bin/umount -a -r
EOF
```

> **What is inittab?**
>  When BusyBox init starts, it reads /etc/inittab to know what to do:
>
> | Line                                     | Meaning                            |
> | ---------------------------------------- | ---------------------------------- |
> | ::sysinit:/bin/mount -t proc proc /proc  | On boot: mount /proc               |
> | ::sysinit:/bin/mount -t sysfs sysfs /sys | On boot: mount /sys                |
> | ::respawn:-/bin/sh                       | Start a shell, restart if it exits |
> | ::shutdown:/bin/umount -a -r             | On shutdown: unmount everything    |
>
> **Why mount proc/sys/dev?**
>
> - /proc — process info, needed for ps, /proc/cpuinfo, etc.
> - /sys — hardware info, sysfs
> - /dev — device files (console, tty, null, etc.)
>
> Without these mounts, almost nothing works.

------

## Phase 4: Setup NFS Server

### Install NFS server on host:

bash

```bash
sudo apt install nfs-kernel-server
```

### Export the rootfs directory:

bash

```bash
echo "/srv/nfs/rootfs *(rw,sync,no_subtree_check,no_root_squash)" | sudo tee -a /etc/exports
```

### Apply and restart:

bash

```bash
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
```

### Verify:

bash

```bash
showmount -e localhost
```

Should show:

text

```text
/srv/nfs/rootfs *
```

------

## Phase 5: Setup TFTP Server

### Install TFTP server on host:

bash

```bash
sudo apt install tftpd-hpa
```

### Copy kernel image:

bash

```bash
sudo cp /path/to/your/Image /srv/tftp/
```

### Verify TFTP is running:

bash

```bash
sudo systemctl status tftpd-hpa
```

------

## Phase 6: Fix bootargs (config.txt)

### The Problem

The original bootargs were set in the SD card's config.txt (boot partition).
 The RPi firmware passes these to the kernel via the device tree. If they conflict
 with what U-Boot sets, you get unexpected behavior.

### The Fix

Edit config.txt on the RPi's boot partition (SD card FAT partition):

Make sure the console setting matches your actual serial port. In our case,
 the kernel registered the UART as different names depending on configuration:

text

```text
config.txt:
[all]
arm_64bit=1
kernel=u-boot.bin
enable_uart=1
```

The final working console configuration:

text

```text
console=ttyS0,115200 8250.nr_uarts=1
```

------

## Phase 7: Boot with NFS + BusyBox

### From U-Boot console:

text

```text
tftp ${kernel_addr_r} Image

setenv bootargs console=ttyS0,115200 8250.nr_uarts=1 loglevel=8 panic=5 root=/dev/nfs nfsroot=10.42.0.1:/srv/nfs/rootfs,nfsvers=3,tcp ip=10.42.0.2::10.42.0.1:255.255.255.0::eth0:off rw init=/sbin/init devtmpfs.mount=1

booti ${kernel_addr_r} - ${fdt_addr}
```

### Bootargs explained:

| Parameter                                       | Purpose                                 |
| ----------------------------------------------- | --------------------------------------- |
| console=ttyS0,115200                            | Use serial port ttyS0 at 115200 baud    |
| 8250.nr_uarts=1                                 | Tell 8250 driver to register 1 UART     |
| loglevel=8                                      | Maximum kernel log verbosity            |
| panic=5                                         | Reboot 5 seconds after kernel panic     |
| root=/dev/nfs                                   | Root filesystem is NFS                  |
| nfsroot=10.42.0.1:/srv/nfs/rootfs,nfsvers=3,tcp | NFS server path and options             |
| ip=10.42.0.2::10.42.0.1:255.255.255.0::eth0:off | Static IP config for RPi                |
| rw                                              | Mount rootfs read-write                 |
| init=/sbin/init                                 | Run BusyBox init as PID 1               |
| devtmpfs.mount=1                                | Auto-mount devtmpfs on /dev before init |

### IP parameter format:

text

```text
ip=<client-ip>::<gateway>:<netmask>::<device>:<autoconf>
ip=10.42.0.2::10.42.0.1:255.255.255.0::eth0:off
```

### Expected boot output:

text

```text
VFS: Mounted root (nfs filesystem) on device 0:19.
devtmpfs: mounted
Run /sbin/init as init process
=== BusyBox is alive! ===

/ #
```

------

## Troubleshooting Journey

This documents every issue encountered and how it was fixed, in order.

### Issue 1: VFS: Unable to mount root fs on unknown-block(0,0)

**When:** First boot with only kernel, no rootfs configured
 **Cause:** No root= in bootargs
 **Fix:** Add NFS root parameters to bootargs

------

### Issue 2: BusyBox build error — sha1_process_block64_shaNI undeclared

**When:** Building BusyBox with cross-compiler
 **Cause:** Known bug with SHA hardware acceleration on cross-compiled builds
 **Fix:** In menuconfig, disable:

text

```text
Settings → SHA1: Use hardware accelerated instructions → NO
Settings → SHA256: Use hardware accelerated instructions → NO
```

------

### Issue 3: init failed (error -2) with init=/linuxrc

**When:** First NFS boot attempt
 **Cause:** /linuxrc didn't exist or BusyBox was dynamically linked and missing libs
 **Fix:** Changed to init=/sbin/init

------

### Issue 4: init failed (error -2) with init=/sbin/init

**When:** After changing init path
 **Cause:** BusyBox was dynamically linked:

text

```text
ELF 64-bit LSB executable, ARM aarch64, dynamically linked, interpreter /lib/ld-linux-aarch64.so.1
```

**Fix:** Copied shared libraries from toolchain sysroot:

bash

```bash
SYSROOT=/home/abdo/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot
sudo cp -a ${SYSROOT}/lib/* /srv/nfs/rootfs/lib/
sudo cp -a ${SYSROOT}/lib64/* /srv/nfs/rootfs/lib64/
```

------

### Issue 5: Warning: unable to open an initial console

**When:** After fixing libraries
 **Cause:** /dev/console device node missing or wrong permissions
 **Fix:**

bash

```bash
sudo mknod -m 622 /srv/nfs/rootfs/dev/console c 5 1
sudo mknod -m 666 /srv/nfs/rootfs/dev/null c 1 3
```

------

### Issue 6: Shell starts but no keyboard input

**When:** Got / # prompt but couldn't type
 **Cause:** Shell not connected to correct tty device
 **Fix attempts:**

1. Used ttyAMA1::respawn:-/bin/sh in inittab — didn't work
2. Used /bin/cttyhack /bin/sh — failed because /dev/ttyAMA1 didn't exist
3. Used exec /bin/sh </dev/console >/dev/console 2>&1 — worked for debug

------

### Issue 7: /dev/ttyAMA1 doesn't exist

**When:** Trying to use cttyhack or getty
 **Cause:** Kernel registered UART as ttyAMA1 in boot log but devtmpfs didn't create it. The PL011 UART driver was not fully configured.
 **Discovery:** Ran debug script to list /dev/tty*:

bash

```bash
#!/bin/sh
mount -t proc proc /proc
mount -t sysfs sysfs /sys
ls /dev/tty*
```

Output showed generic ttys (tty0-63) but NO ttyAMA* or ttyS* devices.

**Final Fix:** Changed console to ttyS0 with 8250 driver:

text

```text
console=ttyS0,115200 8250.nr_uarts=1
```

------

## Final Working Setup

### U-Boot environment:

text

```text
ipaddr=10.42.0.2
serverip=10.42.0.1
netmask=255.255.255.0
kernel_addr_r=0x00080000
```

### Boot commands:

text

```text
tftp ${kernel_addr_r} Image
setenv bootargs console=ttyS0,115200 8250.nr_uarts=1 loglevel=8 panic=5 root=/dev/nfs nfsroot=10.42.0.1:/srv/nfs/rootfs,nfsvers=3,tcp ip=10.42.0.2::10.42.0.1:255.255.255.0::eth0:off rw init=/sbin/init devtmpfs.mount=1
booti ${kernel_addr_r} - ${fdt_addr}
```

### /etc/inittab:

text

```text
::sysinit:/bin/mount -t proc proc /proc
::sysinit:/bin/mount -t sysfs sysfs /sys
::respawn:-/bin/sh
::shutdown:/bin/umount -a -r
```

------

## Filesystem Structure

text

```text
/srv/nfs/rootfs/
├── bin/
│   ├── busybox          (main binary)
│   ├── sh → busybox
│   ├── ls → busybox
│   ├── mount → busybox
│   └── ... (all symlinks to busybox)
├── dev/
│   ├── console          (c 5,1)
│   └── null             (c 1,3)
├── etc/
│   └── inittab
├── lib/                 (shared libs from toolchain sysroot)
├── lib64/               (shared libs from toolchain sysroot)
├── proc/                (empty, mountpoint)
├── sbin/
│   ├── init → ../bin/busybox
│   ├── ifconfig → ../bin/busybox
│   ├── reboot → ../bin/busybox
│   └── ... (all symlinks to busybox)
├── sys/                 (empty, mountpoint)
├── tmp/
└── usr/
    ├── bin/
    └── sbin/
```

------

## Boot Flow Summary

text

```text
Power On
  → RPi firmware loads U-Boot from SD card
    → U-Boot loads kernel Image via TFTP from 10.42.0.1
      → Kernel boots, configures eth0 with static IP
        → Kernel mounts NFS rootfs from 10.42.0.1:/srv/nfs/rootfs
          → Kernel runs /sbin/init (BusyBox init)
            → init reads /etc/inittab
              → Mounts /proc, /sys
              → Spawns /bin/sh
                → Interactive BusyBox shell
                  → Ready! (/ #)
```