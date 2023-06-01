#ifndef NES_MAPPER_TYPE_H
#define NES_MAPPER_TYPE_H

#include "address.h"
#include "cartridge.h"

typedef enum 	// Mapper::Type
{
  NROM  = 0,
  SxROM = 1,
  UxROM = 2,
  CNROM = 3,
  MMC3  = 4,
  AxROM = 7,
  ColorDreams = 11,
  GxROM = 66,
} mapperKind_t;

typedef struct	// Mapper
{
  // protected:
  void* data;
  // public:
  byte_t (*readPRG) (const address_t);
  byte_t (*readCHR) (const address_t);
  void (*writePRG) (const address_t, const byte_t);
  void (*writeCHR) (const address_t, const byte_t);
  nameTableMirroring_t (*getNameTableMirroring) (const void*);
  bool (*hasExtendedRAM) (const void*);
  void (*scanlineIRQ) (void);
} mapper_t;

typedef struct
{
  mapper_t* (*create) (cartridge_t*, const mapperKind_t);
  mapper_t* (*destroy) (mapper_t*);
  /*
  // TODO: add Mapper::createMapper:
  mapper_t* (*generate) (const mapperKind_t,
			 const cartridge_t*,
			 void (*interrupt_cb) (void),
			 void (*mirroring_cb) (void));
  */
} mapper_namespace_t;

#endif

// NES Emulation					June 01, 2023
//
//			Academic Purpose
//
// source: mapper.h
// author: @misael-diaz
//
// Synopsis:
// Mapper header file.
// Defines the mapper type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
