#ifndef NES_CPU_BUS_TYPE_H
#define NES_CPU_BUS_TYPE_H

typedef struct
{
  void* vbus;
  void (*ConnectBus) (void*, void*);
} buscpu_t;

typedef struct
{
  buscpu_t* (*create) ();
  buscpu_t* (*destroy) (buscpu_t*);
} buscpu_namespace_t;

#endif

// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: buscpu.h
// author: @misael-diaz
//
// Synopsis:
// Bus CPU header file.
// Defines the Bus CPU type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
