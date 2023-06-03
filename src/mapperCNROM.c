#include <stdio.h>
#include "mapperCNROM.h"


// we need the typedef of the protected data of the core mapper
typedef struct
{
  cartridge_t* m_cartridge;
  mapperKind_t m_kind;
  void* next;
} data_t;


typedef struct
{
  mapper_t* m_core;
  size_t m_size_ROM;
  size_t m_size_VROM;
  bool m_oneBank;
  address_t m_selectCHR;
  nameTableMirroring_t m_mirroring;
} mapperCNROM_t;


extern mapper_namespace_t const mapper;


static byte_t readPRG (const void* v_core, const address_t address)
{
  const mapper_t* core = v_core;
  const data_t* data = core -> data;
  mapperKind_t const kind = data -> m_kind;
  const cartridge_t* cart = data -> m_cartridge;
  const mapperCNROM_t* map = data -> next;
  const byte_t* PRG = cart -> getROM(cart);
  bool const mirrored = (map -> m_oneBank);

  address_t const offset = (address - 0x8000);
  address_t const addr = ( (mirrored)? (offset & 0x3fff) : (offset) );
  byte_t const byte = PRG[addr];
  printf("Mapper::Mapper: %d read %u from PRG at address 0x%x\n", kind, byte, addr);
  return byte;
}


static byte_t readCHR (const void* v_core, const address_t address)
{
  const mapper_t* core = v_core;
  const data_t* data = core -> data;
  mapperKind_t const kind = data -> m_kind;
  const cartridge_t* cart = data -> m_cartridge;
  const mapperCNROM_t* map = data -> next;
  const byte_t* VROM = cart -> getVROM(cart);

  if (VROM == NULL)
  {
    printf("Mapper::Mapper: %d does not have VROM\n", kind);
    return 0;
  }

  // sane check not in snes:
  address_t const selectCHR = map -> m_selectCHR;
  address_t const addr = ( address | (selectCHR << 13) );
  size_t const size_VROM = map -> m_size_VROM;
  if (addr >= size_VROM)
  {
    const char log [] = "Mapper::Mapper: %d attempted to read VROM "
			"at invalid address 0x%x\n";
    printf(log, kind, addr);
    return 0;
  }

  byte_t const byte = VROM[addr];
  printf("Mapper::Mapper: %d read %u VROM at address 0x%x\n", kind, byte, addr);
  return byte;
}


static void writePRG (void* v_core, const address_t addr, const byte_t byte)
{
  mapper_t* core = v_core;
  data_t* data = core -> data;
  mapperKind_t const kind = data -> m_kind;
  mapperCNROM_t* map = data -> next;

  byte_t const value = (byte & 0x03);
  map -> m_selectCHR = value;
  const char log [] = "Mapper::Mapper: %d wrote %u to select-CHR. "
		      "address (ignore unused): 0x%x\n";
  printf(log, kind, value, addr);
}


static void writeCHR (void* v_core, const address_t addr, const byte_t byte)
{
  mapper_t* core = v_core;
  data_t* data = core -> data;
  const mapperKind_t kind = data -> m_kind;

  const char log [] = "Mapper::Mapper: %d attempted to write %u to "
		      "read-only CHR at address 0x%x\n";
  printf(log, kind, byte, addr);
}


static mapper_t* create (cartridge_t* cart)
{
  // constructs core (or super class):
  mapperKind_t kind = CNROM;
  mapper_t* core = mapper.create(cart, kind);
  if (core == NULL)
  {
    return core;
  }

  // constructs derived CNROM mapper (or extending class):
  mapperCNROM_t* map = malloc( sizeof(mapperCNROM_t) );
  if (map == NULL)
  {
    core = mapper.destroy(core);
    return core;
  }

  // binds core to mapper CNROM:
  data_t* data = core -> data;
  data -> next = map;

  // overrides methods:
  core -> readPRG = readPRG;
  core -> readCHR = readCHR;
  core -> writePRG = writePRG;
  core -> writeCHR = writeCHR;

  // gets [V]ROM info:
  size_t size_ROM = cart -> getSizeROM(cart);
  size_t size_VROM = cart -> getSizeVROM(cart);
  bool oneBank = ( (size_ROM == 0x4000)? true : false );

  // initializes mapper CNROM data:
  map -> m_core = core;
  map -> m_oneBank = oneBank;
  map -> m_size_ROM = size_ROM;
  map -> m_size_VROM = size_VROM;
  map -> m_selectCHR = ( (address_t) 0x0000 );

  return core;
}


static mapper_t* destroy (mapper_t* core)
{
  if (core == NULL)
  {
    return core;
  }

  data_t* data = core -> data;
  mapperCNROM_t* map = data -> next;
  core = mapper.destroy(core);
  map -> m_core = NULL;

  free(map);
  map = NULL;
  return core;
}


mapperCNROM_namespace_t const mapperCNROM = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 02, 2023
//
//			Academic Purpose
//
// source: mapperCNROM.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the mapper CNROM object.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
