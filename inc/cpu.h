#ifndef NES_CPU_TYPE_H
#define NES_CPU_TYPE_H

#include "bus.h"
#include "device.h"
#include "address.h"
#include "byte.h"

typedef struct	// CPU
{
  // public:
  bus_t* bus;							// Main Bus
  device_t* device;						// base type of CPU
  byte_t (*read) (const void*, const address_t);
  void (*write) (void*, const address_t, const byte_t);
} cpu_t;

typedef struct
{
  cpu_t* (*create) (device_t*);
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
