#ifndef NES_MAPPER_CNROM_TYPE_H
#define NES_MAPPER_CNROM_TYPE_H

#include "mapper.h"

typedef struct
{
  mapper_t* (*create) (cartridge_t*);
  mapper_t* (*destroy) (mapper_t*);
} mapperCNROM_namespace_t;

#endif

// NES Emulation					June 02, 2023
//
//			Academic Purpose
//
// source: mapperCNROM.h
// author: @misael-diaz
//
// Synopsis:
// MapperCNROM header file.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
