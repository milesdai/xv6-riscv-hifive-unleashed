# 6.858 Final Project Log
## Building QEMU
The most accurate emulation of the HiFive Unleashed board comes from the latest version of the repository. Thus we need to build it ourselves instead of relying on an existing release.

Follow instructions here: https://wiki.qemu.org/Hosts/Mac

* SiFive site: https://www.sifive.com/blog/risc-v-qemu-part-2-the-risc-v-qemu-port-is-upstream
* Migrated wiki: https://github.com/riscv/riscv-qemu/wiki
* Booting Fedora onto a HiFive Unleashed board: https://wiki.qemu.org/Documentation/Platforms/RISCV


1. Clone the repo:
```bash
git clone https://github.com/qemu/qemu.git
```

2. Configure and build. Check the README to ensure all prereqs are met.
```bash
cd qemu
./configure --target-list=riscv64-softmmu
# -j option will make compilation much faster
make -j
```

# The Above didn't seem to work

Will try the following commands from: https://github.com/riscv/riscv-qemu/wiki

```bash
# The old git repo has been archived and is moved 
# git clone --recursive https://github.com/riscv/riscv-qemu.git
git clone --recursive https://git.qemu.org/git/qemu.git
cd riscv-qemu
./configure \
    --target-list=riscv64-softmmu,riscv32-softmmu
make -j$(nproc)
make install
```
