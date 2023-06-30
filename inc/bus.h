#ifndef NES_BUS_TYPE_H
#define NES_BUS_TYPE_H

#include "device.h"
#include "address.h"
#include "byte.h"

typedef struct	// Bus
{
  // public:
  byte_t* ram;						// fake RAM
  device_t* cpu;					// for the CPU Bus connection
  byte_t (*read) (const void*, const address_t);
  void (*write) (void*, const address_t, const byte_t);
} bus_t;

typedef struct
{
  bus_t* (*create) (device_t*);
  bus_t* (*destroy) (bus_t*);
} bus_namespace_t;

#endif

// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: bus.h
// author: @misael-diaz
//
// Synopsis:
// Bus header file.
// Defines the bus type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
