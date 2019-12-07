# 6.828 Final Project Report
*Miles Dai (milesdai), Shahul Alam (shahul)*

## Summary
In this project, we adapt xv6 to run on the HiFive Unleashed board made by SiFive.

## Emulation
We used QEMU to emulate the real SiFive board. This required pulling the upstream version of QEMU which contained the most recent updates for the HiFive Unleashed. We rebuilt QEMU, configuring it for the ```riscv64-softmmu``` target.

We tested to ensure that the emulation was working successfully by booting up a RISC-V Fedora Linux image. Once this booted successfully, we knew we could continue on to modifying xv6.

We modified the xv6 Makefile to point to the compiled upstream QEMU version and use the sifive_u emulator instead of the virtual device.

## Management Core
The HiFive Unleashed contains 5 cores. One of these cores is an E51 Monitor core while the the other four are U54 cores. Because the monitor core does not support a number of operations including supervisor mode and floating point operations, we do not want to boot xv6 on this core. In ```entry.S```, we add some assembly to check ```mhartid``` and set it to loop forever in an empty loop if the core has a hart ID of 0 which corresponds to the management core.

It was also at this time that we discovered that our GDB was unable to attach to any of the other cores besides hart 0. We were unable to fix this issue which limited our debugging tools to print statements.

## UART Transmit
Transmitting through the UART allows for printing to the screen. We first changed the location of the UART memory map in ```memlayout.h``` to the location specified in the manual for the board for UART0.

We found that by directly writing to the location of the UART transmit buffer, we were able to get printout on the screen.

## Disk
The original version of xv6 running on the virtual device used the virtio disk, an emulated hard drive. We used a ramdisk solution, adding assembly in ```entry.S``` to load ```fs.img``` into the kernel binary. This let us use simple driver code in ```virtio_disk_rw``` to index into the ramdisk to retrieve data.

## UART Receive
At this point, all initialization steps were successful, and the code was able to able to progress to the point that `user/sh.c` was executed; however, while the `$` command line prompt displayed, the console was not accepting keyboard input. We discovered that the UART was not successfully raising interrupts to signal inputs to xv6. This was fixed by first initializing the correct registers in the UART to enable interrupts. Then, we found that many of the macros used in ```plic.c``` were not correct for the sifive_u board. In particular, they assume that all harts have a machine and supervisor mode. However, in the HiFive Unleashed, the monitor core does not have supervisor-mode interrupts. This caused all the offsets in the macros to be incorrect. Once this was fixed, we were able to receive user input from the keyboard.

## Testing
We used usertests as the benchmark to determine whether our code was minimally functional. After we made the changes necessary to start up `sh.c`, we were able to pass all `usertests` except for two: `forkforkfork()` and `openiputtest()`. The root cause of this issue again turned out to be the sifive_u board’s management core. In the original xv6 code, the core with `cpuid() == 0` was used to count timer ticks in `trap.c`. Unmodified, this code obviously would not work for us, since we set the management core to spin forever during initialization, and since the two aforementioned tests call `sleep()`, this bug manifested by causing these tests to hang forever. We remedied this by assigning the core with `cpuid() == 1` the responsibility of incrementing the `ticks` variable. After this minor change, all `usertests` passed.

## Challenges
One of the biggest challenges we faced was the lack of a proper debugger which made following the path of the interrupts difficult. In particular, we had a very subtle bug in which one of the macros we were using to read a bit from the UART caused the UART to dequeue the value from the rx_fifo which caused the second read afterwards to return the incorrect value. This made it impossible for `uartintr()` to detect when the fifo was empty which causes the `uartgetc` function to continue forever in one of the harts.
