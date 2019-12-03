//
// low-level driver routines for 16550a UART.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

// the UART control registers.
// some have different meanings for
// read vs write.
// http://byterunner.com/16550.html
#define TXDATA 0 // transmit data register
#define RXDATA 4 // receive data register
#define TXCTRL 8
#define RXCTRL 12
#define IE 16
#define IP 20
#define DIV 24
#define THR 0 // transmit holding register (for output bytes)
#define RHR 0 // receive holding register (for input bytes)
#define THR 0 // transmit holding register (for output bytes)
#define IER 1 // interrupt enable register
#define FCR 2 // FIFO control register
#define ISR 2 // interrupt status register
#define LCR 3 // line control register
#define LSR 5 // line status register

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

#define GetBit(reg, bit) (ReadReg(reg) & (1 << bit))
#define SetBit(reg, bit) (WriteReg(reg, ReadReg(reg) |= (1 << bit))
#define ClearBit(reg, bit) (WriteReg(reg, ReadReg(reg) &= ~(1 << bit))
#define FlipBit(reg, bit) (WriteReg(reg, ReadReg(reg) ^= (1 << bit))

void
uartinit(void)
{
  WriteReg(RXCTRL, 0x1);
  WriteReg(IE, 0x3);
  return;
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, 0x80);

  // LSB for baud rate of 38.4K.
  WriteReg(0, 0x03);

  // MSB for baud rate of 38.4K.
  WriteReg(1, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, 0x03);

  // reset and enable FIFOs.
  WriteReg(FCR, 0x07);

  // enable receive interrupts.
  WriteReg(IER, 0x01);
}

// write one output character to the UART.
void
uartputc(int c)
{
  // wait for Transmit Holding Empty to be set in LSR.
  //while((ReadReg(LSR) & (1 << 5)) == 0)
  while(GetBit(TXDATA, 31) == 1)
    ;
  WriteReg(TXDATA, (uint32) c);
}

// read one input character from the UART.
// return -1 if none is waiting.
int
uartgetc(void)
{
  //if(ReadReg(LSR) & 0x01){
  if(GetBit(RXDATA, 31) == 0){
    // input data is ready.
    //return ReadReg(RHR);
    return (int) ReadReg(RXDATA);
  } else {
    return -1;
  }
}

// trap.c calls here when the uart interrupts.
void
uartintr(void)
{
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }
}
