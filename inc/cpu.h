#ifndef NES_CPU_TYPE_H
#define NES_CPU_TYPE_H

#include "bus.h"
#include "device.h"
#include "address.h"
#include "byte.h"

typedef struct	// CPU
{
  // public:
  bus_t* bus;						// Main Bus
  const device_t* dev;					// CPU base type (for Bus Connect)
  // Registers:
  address_t pc;						// Program Counter PC
  address_t abs;					// Absolute Address
  address_t rel;					// Relative Address
  byte_t a;						// Accumulator Register
  byte_t x;						// X Register
  byte_t y;						// Y Register
  byte_t fetched;
  // Bus Connectivity:
  byte_t (*read) (const void*, const address_t);
  void (*write) (void*, const address_t, const byte_t);
  // Addressing Modes:
  byte_t (*IMP) (void*);				// Implied
  byte_t (*IMM) (void*);				// Immediate
  byte_t (*ZP0) (void*);				// Zero Page
  byte_t (*ZPX) (void*);				// Zero Page + X Register offset
  byte_t (*ZPY) (void*);				// Zero Page + Y Register offset
  byte_t (*ABS) (void*);				// Absolute
  byte_t (*ABX) (void*);				// Absolute  + X Register offset
  byte_t (*ABY) (void*);				// Absolute  + Y Register offset
  byte_t (*IND) (void*);				// Indirect
  byte_t (*INX) (void*);				// Indirect  + X Register offset
  byte_t (*INY) (void*);				// Indirect  + Y Register offset
  byte_t (*REL) (void*);				// Relative
} cpu_t;

typedef struct
{
  cpu_t* (*create) (const device_t*);
  cpu_t* (*destroy) (cpu_t*);
} cpu_namespace_t;

#endif

// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: cpu.h
// author: @misael-diaz
//
// Synopsis:
// CPU header file.
// Defines the CPU type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
