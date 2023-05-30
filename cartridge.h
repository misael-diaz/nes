#ifndef NES_CARTRIDGE_TYPE_H
#define NES_CARTRIDGE_TYPE_H

#include <stdlib.h>
#include <stdbool.h>
#include "byte.h"

typedef struct
{
  byte* header;
  byte* m_PRG_ROM;
  byte* m_CHR_ROM;
  size_t num_banks;
  size_t num_vbanks;
  byte banks;
  byte vbanks;
  byte m_nameTableMirroring;
  byte m_mapperNumber;
  bool m_extendedRAM;
} cartridge_t;

cartridge_t* create();
cartridge_t* destroy(cartridge_t*);
void loadFromFile(cartridge_t*);

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
