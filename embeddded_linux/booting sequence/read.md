Question 1

### 1️⃣ Create Empty Image File

```
dd if=/dev/zero of=sd.img bs=1M count=1024
```

Creates a 1GB raw disk image.

------

### 2️⃣ Attach Image to Loop Device

```
sudo losetup -f --show sd.img
```

Example output:

```
/dev/loop0
```

------

### 3️⃣ Create Partition Table

```
sudo cfdisk /dev/loop0
```

nside cfdisk:

- Create **200 MB partition** → Boot partition
- Create remaining space → Root filesystem
- Set boot partition type to **FAT16**
- Write table and exit

------

### 4️⃣ Reload Partition Table

```
sudo losetup -d /dev/loop0
sudo losetup -f --partscan --show sd.img
```

Now you should see:

```
/dev/loop0
/dev/loop0p1
/dev/loop0p2
```

------

### 5️⃣ Format Boot Partition (FAT16)

```
sudo mkfs.vfat -n BOOT -F 16 /dev/loop0p1
```

### 6️⃣ (Optional) Format Root Filesystem

Example using ext4:

```
sudo mkfs.ext4 -L rootfs /dev/loop0p2
```

------

Question 2

# Difference Between MBR and GPT

## Overview

MBR (Master Boot Record) and GPT (GUID Partition Table) are two different partitioning schemes used to define how data is stored and organized on storage devices such as hard drives and SSDs.

------

## MBR (Master Boot Record)

### Key Characteristics

- Maximum number of partitions:
  - Supports up to **4 primary partitions** (commonly implemented as 3 primary partitions + 1 extended partition).
- Maximum disk size support:
  - Supports disks up to **2 TB**.
- Structure size:
  - Occupies **512 bytes** at the beginning of the disk.
- Compatibility:
  - Supported by older BIOS-based systems.
- Limitations:
  - Limited partition count.
  - Limited disk size support.
  - Higher risk of data loss if the MBR sector becomes corrupted (single point of failure).

## GPT (GUID Partition Table)

### Key Characteristics

- Maximum number of partitions:
  - Supports up to **128 partitions** (varies by operating system).
  - All partitions are treated equally (no primary or extended distinction).
- Maximum disk size support:
  - Supports very large disks (well beyond 2 TB).
- Reliability:
  - Stores multiple partition table copies for redundancy.
  - Includes CRC checks for improved data integrity.
- Compatibility:
  - Typically used with modern **UEFI-based systems**.

------

## Comparison Summary

| Feature            | MBR                                   | GPT                                 |
| ------------------ | ------------------------------------- | ----------------------------------- |
| Maximum Disk Size  | 2 TB                                  | Very large (ZB scale theoretically) |
| Maximum Partitions | 4 Primary (or 3 Primary + 1 Extended) | Typically 128                       |
| Partition Equality | No                                    | Yes                                 |
| Reliability        | Single partition table                | Multiple backups + CRC protection   |
| Firmware Support   | BIOS                                  | UEFI                                |

------

Question 3

## FAT16 (File Allocation Table 16)

### Key Characteristics

- **Design:**
   Simple and lightweight file system.
- **Storage Support:**
   Supports small storage sizes (typically up to 2 GB depending on cluster size).
- **Boot Support:**
   Commonly used in embedded systems and bootloaders because of its small footprint.
   Can be easily loaded and handled by boot ROM or firmware.
- **Compatibility:**
   Supported by a wide range of operating systems and devices.
- **Limitations:**
  - Limited file size and partition size.
  - Inefficient storage usage with larger disks.

## FAT32 (File Allocation Table 32)

### Key Characteristics

- **Design:**
   Enhanced version of FAT16 with improved scalability.
- **Storage Support:**
   Supports much larger partitions (up to approximately 2 TB depending on implementation).
- **File Size Limit:**
   Maximum single file size is **4 GB**.
- **Compatibility:**
   Extremely widely supported across operating systems, embedded devices, gaming consoles, and removable storage devices.
- **Performance:**
   More efficient storage management compared to FAT16.
- **Common Use Cases:**
  - USB flash drives
  - SD cards
  - External storage devices requiring high compatibility

## EXT4 (Fourth Extended File System)

### Key Characteristics

- **Design:**
   Advanced and high-performance file system mainly used in Linux systems.
   Uses modern indexing techniques such as tree-based directory indexing and extents for faster file access.
- **Storage Support:**
   Supports extremely large partitions and file sizes.
- **Reliability:**
   Includes journaling to help prevent data corruption during crashes or power failures.
- **Performance:**
   Provides faster file access and better handling of large numbers of files.
- **Advanced Features:**
  - Delayed allocation improves performance
  - Extents reduce file fragmentation
  - Strong metadata handling

------

## Comparison Summary

| Feature            | FAT16                         | FAT32                          | EXT4                   |
| ------------------ | ----------------------------- | ------------------------------ | ---------------------- |
| Complexity         | Very Simple                   | Simple                         | Advanced               |
| Max File Size      | Small                         | 4 GB                           | Very Large             |
| Max Partition Size | ~2 GB                         | Up to ~2 TB                    | Extremely Large        |
| Performance        | Basic                         | Moderate                       | High                   |
| Reliability        | Low                           | Moderate                       | High (Journaling)      |
| OS Support         | Very Wide                     | Very Wide                      | Mainly Linux           |
| Common Usage       | Bootloaders, Embedded Systems | USB, SD Cards, External Drives | Linux Systems, Servers |

------

Question 4

![](/home/abdo/Pictures/Screenshots/Screenshot from 2026-02-08 10-48-35.png)

Question 5

A **loop device** is a virtual block device in Linux that allows a regular file to be treated as if it were a physical storage device (like a disk or partition).

Linux uses loop devices to enable working with disk images and filesystem images without requiring actual hardware storage.

------

## 2. Why Linux Uses Loop Devices

Loop devices are mainly used for the following purposes:

### A. Access Partitions Inside Disk Image Files

- Allows mounting and accessing individual partitions inside an image file.
- Commonly used when working with embedded Linux images containing filesystems such as:
  - FAT16
  - FAT32
  - EXT4

### B. Treat Files as Block Storage

- Enables a normal file to be handled as a block device.
- Useful for:
  - Testing filesystems
  - Flashing software images
  - Simulating storage devices
  - Mounting ISO or filesystem images

## 3. Loop Device Commands

### A. Command to Create and Attach a Loop Device

```
losetup -f --partscan --show <file>
```

#### Explanation:

- `-f` → Finds the first available loop device.
- `--partscan` → Scans and creates partition mappings inside the image file.
- `--show` → Displays the created loop device name.
- `<file>` → Disk or filesystem image file.

------

### B. Command to List All Loop Devices Currently in Use

```
losetup -a
```

OR

```
losetup --list
```

### C. Command to Detach a Mounted Loop Device

```
sudo losetup -d <loop_device>
```

#### Example:

```
sudo losetup -d /dev/loop0
```

Question 6

```
cat /sys/module/loop/parameters/max_loop
```

Question 7

Open:

```
sudo nano /etc/default/grub
```

Find:

```
GRUB_CMDLINE_LINUX=""
```

Modify it to include:

```
GRUB_CMDLINE_LINUX="loop.max_loop=64"
```

You can set any value you want (32, 64, 256).

Question 8 - Question 9

## Step 1: Attach the Disk Image as a Loop Device

Use partition scanning so Linux exposes the partitions:

```
sudo losetup -f --partscan --show sd.img
```

Example output:

```
/dev/loop34
```

Now the partitions appear as:

- `/dev/loop34p1`
- `/dev/loop34p2`

------

## Step 2: Format the First Partition as FAT16 or FAT32

### FAT16:

```
sudo mkfs.vfat -F 16 -n boot /dev/loop34p1
```

### FAT32:

```
sudo mkfs.vfat -F 32 -n boot /dev/loop34p1
```

This partition is typically used for boot loaders.

## Step 3 Format the Second Partition as EXT4

```
sudo mkfs.ext4 -L rootfs /dev/loop34p2
```

This partition is used as the main Linux filesystem.

------

## Step 4: Mount the Filesystems

Create mount points:

```
mkdir boot rootfs
```

Mount both partitions:

```
sudo mount /dev/loop34p1 boot/
sudo mount /dev/loop34p2 rootfs/
```

## Step 5: Unmount the Filesystems

After finishing:

```
sudo umount boot
sudo umount rootfs
```

------

## Step 6: Detach the Loop Device

Finally, detach the loop device:

```
sudo losetup -d /dev/loop34
```

This cleanly disconnects the loop mapping.

Question 10

# mount and umount Commands in Linux

## mount

`mount` connects a filesystem to the Linux directory tree so that you can read or write it normally.

When you run:

```
sudo mount /dev/loop34p1 boot/
```

You are telling the kernel:

- this block device contains a filesystem
- attach it inside the folder `boot/`
- make it accessible for users and processes

After mounting:

- files inside the partition appear in the mount directory
- all reads/writes go directly to the filesystem on the device

## umount

`umount` detaches the filesystem from the directory tree:

```
sudo umount boot
```

What it actually does:

- writes all cached data to the filesystem
- closes open files
- finalizes metadata
- removes the filesystem from the system namespace

You **must** unmount before:

- detaching loop devices
- removing USB drives
- using the image in QEMU
- modifying the disk image file directly

If you don’t unmount:

- filesystem corruption
- dirty journal
- incomplete writes

modifying the disk image file directly

If you don’t unmount:

filesystem corruption

dirty journal

incomplete writes

“device busy” errors

Question 11

# Block Device vs Character Device

Linux treats all hardware as either:

## Block Device

A block device transfers **data in fixed-size blocks** (usually 512 bytes or 4 KB).

Examples:

- Hard drives
- SSDs
- SD cards
- Loop devices
- NVMe storage
- RAM disks

Characteristics:

- Supports random access (jump to any block)
- Can host filesystems (EXT4, FAT, etc.)
- Uses buffering and caching
- Appears under `/dev` as: `/dev/sda`, `/dev/mmcblk0`, `/dev/loop32`

Block devices are used for **storage**.

## Character Device

A character device transfers **data one character (byte) at a time**.

Examples:

- UART serial ports
- Keyboards
- Mice
- Sound cards
- `/dev/tty`, `/dev/zero`, `/dev/null`

Characteristics:

- No random access
- Stream-based
- No filesystem can be created on it
- Data flows in sequence

Character devices are used for **streams** (input/output).

