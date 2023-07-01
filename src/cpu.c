#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"


static byte_t read (const void* vcpu, address_t const address)
{
  const cpu_t* cpu = vcpu;
  const bus_t* bus = cpu -> bus;
  byte_t const data = bus -> read(bus, address);
  return data;
}


static void write (void* vcpu, address_t const address, byte_t const data)
{
  cpu_t* cpu = vcpu;
  bus_t* bus = cpu -> bus;
  bus -> write(bus, address, data);
}


static void ConnectBus (cpu_t* cpu, const device_t* devCPU)
{
  bus_t* bus = devCPU -> vbus;
  cpu -> bus = bus;
}


static byte_t implied (void* vcpu)
{
  cpu_t* cpu = vcpu;
  byte_t fetched = cpu -> fetched;
  byte_t const accumulator = cpu -> a;

  fetched = accumulator;
  cpu -> fetched = fetched;
  return 0x00;
}


static byte_t immediate (void *vcpu)
{
  cpu_t* cpu = vcpu;
  address_t pc = cpu -> pc;

  address_t const abs = pc;
  ++pc;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t zeroPage (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t pc = cpu -> pc;

  address_t abs = cpu -> read(cpu, pc);
  ++pc;

  abs &= 0x00ff;
  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t zeroPageXRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const x = cpu -> x;
  address_t pc = cpu -> pc;

  address_t abs = cpu -> read(cpu, pc);
  ++pc;

  abs += x;
  abs &= 0x00ff;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t zeroPageYRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const y = cpu -> y;
  address_t pc = cpu -> pc;

  address_t abs = cpu -> read(cpu, pc);
  ++pc;

  abs += y;
  abs &= 0x00ff;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t absolute (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t pc = cpu -> pc;

  address_t const lo = cpu -> read(cpu, pc);
  ++pc;
  address_t const hi = cpu -> read(cpu, pc);
  ++pc;

  address_t const abs = ( (hi << 8) | lo );

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t absoluteXRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const x = cpu -> x;
  address_t pc = cpu -> pc;

  address_t const lo = cpu -> read(cpu, pc);
  ++pc;
  address_t const hi = cpu -> read(cpu, pc);
  ++pc;

  address_t abs = ( (hi << 8) | lo );
  abs += x;

  bool const inNextPage = ( (abs & 0xff00) != (hi << 8) );
  byte_t const ret = (inNextPage)? 1 : 0;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return ret;
}


static byte_t absoluteYRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const y = cpu -> y;
  address_t pc = cpu -> pc;

  address_t const lo = cpu -> read(cpu, pc);
  ++pc;
  address_t const hi = cpu -> read(cpu, pc);
  ++pc;

  address_t abs = ( (hi << 8) | lo );
  abs += y;

  bool const inNextPage = ( (abs & 0xff00) != (hi << 8) );
  byte_t const ret = (inNextPage)? 1 : 0;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return ret;
}


static byte_t indirect (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t abs = cpu -> abs;
  address_t pc = cpu -> pc;

  address_t const lo = cpu -> read(cpu, pc);
  ++pc;
  address_t const hi = cpu -> read(cpu, pc);
  ++pc;

  address_t const addr = ( (hi << 8) | lo );

  // caters hardware bug
  if (lo == 0x00ff)
  {
    address_t const lo = cpu -> read(cpu, addr);
    address_t const hi = cpu -> read(cpu, addr & 0xff00);
    abs = ( (hi << 8) | lo );
  }
  else
  {
    address_t const lo = cpu -> read(cpu, addr);
    address_t const hi = cpu -> read(cpu, addr + 1);
    abs = ( (hi << 8) | lo );
  }

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t indirectXRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const x = cpu -> x;
  address_t pc = cpu -> pc;

  address_t const addr = cpu -> read(cpu, pc);
  ++pc;

  address_t const lo = cpu -> read(cpu, (addr + x) & 0x00ff);
  address_t const hi = cpu -> read(cpu, (addr + x + 1) & 0x00ff);

  address_t const abs = ( (hi << 8) | lo );

  cpu -> abs = abs;
  cpu -> pc = pc;
  return 0x00;
}


static byte_t indirectYRegisterOffset (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t const y = cpu -> y;
  address_t pc = cpu -> pc;

  address_t const addr = cpu -> read(cpu, pc);
  ++pc;

  address_t const lo = cpu -> read(cpu, addr & 0x00ff);
  address_t const hi = cpu -> read(cpu, (addr + 1) & 0x00ff);

  address_t abs = ( (hi << 8) | lo );
  abs += y;

  bool const inNextPage = ( (abs & 0xff00) != (hi << 8) );
  byte_t const ret = (inNextPage)? 1 : 0;

  cpu -> abs = abs;
  cpu -> pc = pc;
  return ret;
}



static byte_t relative (void* vcpu)
{
  cpu_t* cpu = vcpu;
  address_t pc = cpu -> pc;

  address_t rel = cpu -> read(cpu, pc);
  ++pc;

  if (rel & 0x0080)
  {
    rel |= 0xff00;
  }

  cpu -> rel = rel;
  cpu -> pc = pc;
  return 0x00;
}


static cpu_t* create (const device_t* dev)
{
  cpu_t* cpu = malloc( sizeof(cpu_t) );
  if (cpu == NULL)
  {
    printf("CPU::CPU() failed to allocate CPU!\n");
    return cpu;
  }

  // bus connectivity:
  cpu -> read = read;
  cpu -> write = write;
  cpu -> dev = dev;

  // registers:
  cpu -> pc = 0x0000;
  cpu -> abs = 0x0000;
  cpu -> rel = 0x0000;
  cpu -> a = 0x00;
  cpu -> x = 0x00;
  cpu -> y = 0x00;

  cpu -> fetched = 0x00;

  // addressing modes:
  cpu -> IMP = implied;
  cpu -> IMM = immediate;
  cpu -> ZP0 = zeroPage;
  cpu -> ZPX = zeroPageXRegisterOffset;
  cpu -> ZPY = zeroPageYRegisterOffset;
  cpu -> ABS = absolute;
  cpu -> ABX = absoluteXRegisterOffset;
  cpu -> ABY = absoluteYRegisterOffset;
  cpu -> IND = indirect;
  cpu -> INX = indirectXRegisterOffset;
  cpu -> INY = indirectYRegisterOffset;
  cpu -> REL = relative;

  ConnectBus(cpu, dev);

  return cpu;
}


static cpu_t* destroy (cpu_t* cpu)
{
  if (cpu == NULL)
  {
    return cpu;
  }

  free(cpu);
  cpu = NULL;
  return cpu;
}


cpu_namespace_t const cpu = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: cpu.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the CPU object.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
