	# qemu -kernel starts at 0x1000. the instructions
        # there seem to be provided by qemu, as if it
        # were a ROM. the code at 0x1000 jumps to
        # 0x8000000, the _start function here,
        # in machine mode. each CPU starts here.
.section .data
ram_disk: .incbin "fs.img"
.globl ram_disk
.section .data
.globl stack0
.section .text
.globl start
.section .text
.globl _entry
_entry:
	# set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li a0, 1024*4
        li t5, 0x10010000
        li t6, 65
        sw t6, 0(t5)
	csrr a1, mhartid
    # loop forever if this is the management core (will cause problems later; better not to count on it)
    beqz a1, junk
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
	# jump to start() in start.c
        call start
junk:
        j junk
