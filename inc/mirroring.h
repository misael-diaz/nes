#ifndef NES_NAME_TABLE_MIRRORING_TYPE_H
#define NES_NAME_TABLE_MIRRORING_TYPE_H

typedef enum
{
  Horizontal = 0,
  Vertical = 1,
  FourScreen = 8,
  OneScreenLower,
  OneScreenHigher,
} nameTableMirroring_t;

#endif

// NES Emulation					June 01, 2023
//
//			Academic Purpose
//
// source: mirroring.h
// author: @misael-diaz
//
// Synopsis:
// Defines the Name Table Mirroring type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
