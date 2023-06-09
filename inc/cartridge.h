#ifndef NES_CARTRIDGE_TYPE_H
#define NES_CARTRIDGE_TYPE_H

#include <stdlib.h>
#include <stdbool.h>

#include "byte.h"
#include "mirroring.h"

typedef struct
{
  // private:
  void* data;
  // public:
  void (*loadFromFile) (void*);
  byte_t* (*getROM) (const void*);
  byte_t* (*getVROM) (const void*);
  size_t (*getSizeROM) (const void*);
  size_t (*getSizeVROM) (const void*);
  byte_t (*getNameTableMirroring) (const void*);
  bool (*hasExtendedRAM) (const void*);
} cartridge_t;

typedef struct
{
  cartridge_t* (*create) (void);
  cartridge_t* (*destroy) (cartridge_t*);
} cartridge_namespace_t;

#endif

// NES Emulation					May 30, 2023
//
//			Academic Purpose
//
// source: cartridge.h
// author: @misael-diaz
//
// Synopsis:
// Cartridge header file.
// Defines the cartridge type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
