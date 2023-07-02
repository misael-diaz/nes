/*
	olc6502 - An emulation of the 6502/2A03 processor
	"Thanks Dad for believing computers were gonna be a big deal..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Background
	~~~~~~~~~~
	I love this microprocessor. It was at the heart of two of my favourite
	machines, the BBC Micro, and the Nintendo Entertainment System, as well
	as countless others in that era. I learnt to program on the Model B, and
	I learnt to love games on the NES, so in many ways, this processor is
	why I am the way I am today.

	In February 2019, I decided to undertake a selfish personal project and
	build a NES emulator. Ive always wanted to, and as such I've avoided
	looking at source code for such things. This made making this a real
	personal challenge. I know its been done countless times, and very likely
	in far more clever and accurate ways than mine, but I'm proud of this.

	Datasheet: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf

	Files: olc6502.h, olc6502.cpp

	Relevant Video: https://www.youtube.com/watch?v=8XmxKPJDGU0

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/

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
