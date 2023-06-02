#ifndef NES_MAPPER_AXROM_TYPE_H
#define NES_MAPPER_AXROM_TYPE_H

#include "mapper.h"

typedef struct
{
  mapper_t* (*create) (cartridge_t*, void (*mirroring_cb) (void));
  mapper_t* (*destroy) (mapper_t*);
} mapperAxROM_namespace_t;

#endif

// NES Emulation					June 02, 2023
//
//			Academic Purpose
//
// source: mapperAxROM.h
// author: @misael-diaz
//
// Synopsis:
// MapperAxROM header file.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
