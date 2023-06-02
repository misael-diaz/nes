#include <stdio.h>
#include "mapperAxROM.h"


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
  byte_t* m_characterRAM;
  void (*m_mirroringCallBack) (void);
  size_t m_size_ROM;
  size_t m_size_VROM;
  size_t m_size_characterRAM;
  uint32_t m_bank_PRG;
  nameTableMirroring_t m_mirroring;
} mapperAxROM_t;


extern mapper_namespace_t const mapper;


static byte_t readPRG (const void* v_core, const address_t addr)
{
  const mapper_t* core = v_core;
  const data_t* data = core -> data;
  const mapperKind_t kind = data -> m_kind;
  const cartridge_t* cart = data -> m_cartridge;
  const mapperAxROM_t* map = data -> next;
  const byte_t* PRG = cart -> getROM(cart);
  const uint32_t bank_PRG = map -> m_bank_PRG;
  // NOTE: mixed type arithmetic (from snes), address is uint16_t and bank-PRG is uint32_t
  const uint32_t idx = ( (bank_PRG * 0x8000) + (addr & 0x7fff) );
  const byte_t byte = PRG[idx];
  printf("Mapper::Mapper: %d read %u from PRG at address 0x%x\n", kind, byte, idx);
  return byte;
}


static byte_t readCHR (const void* v_core, const address_t addr)
{
  const mapper_t* core = v_core;
  const data_t* data = core -> data;
  const mapperKind_t kind = data -> m_kind;
  const mapperAxROM_t* map = data -> next;
  const byte_t* CHR = map -> m_characterRAM;

  if (CHR == NULL)
  {
    printf("Mapper::Mapper: %d does not use Character RAM\n", kind);
    return 0;
  }

  size_t const size_characterRAM = map -> m_size_characterRAM;
  if (addr >= size_characterRAM)
  {
    const char log [] = "Mapper::Mapper: %d attempted to read CHR "
			"at invalid address 0x%x\n";
    printf(log, kind, addr);
    return 0;
  }

  byte_t const byte = CHR[addr];
  printf("Mapper::Mapper: %d read %u CHR at address 0x%x\n", kind, byte, addr);
  return byte;
}


static void writePRG (void* v_core, const address_t addr, const byte_t byte)
{
  mapper_t* core = v_core;
  data_t* data = core -> data;
  const mapperKind_t kind = data -> m_kind;
  mapperAxROM_t* map = data -> next;
  if (addr >= 0x8000)
  {
    map -> m_bank_PRG = (byte & 0x07);
    map -> m_mirroring = ( (byte & 0x10)? OneScreenHigher : OneScreenLower );
    map -> m_mirroringCallBack();
    const uint32_t bank = map -> m_bank_PRG;
    const char log[] = "Mapper::Mapper: %d wrote %u to bank-PRG\n";
    printf(log, kind, bank);
  }
}


static void writeCHR (void* v_core, const address_t addr, const byte_t byte)
{
  mapper_t* core = v_core;
  data_t* data = core -> data;
  const mapperKind_t kind = data -> m_kind;
  mapperAxROM_t* map = data -> next;
  byte_t* CHR = map -> m_characterRAM;

  if (CHR == NULL)
  {
    printf("Mapper::Mapper: %d does not use Character RAM\n", kind);
    return;
  }

  size_t const size_characterRAM = map -> m_size_characterRAM;
  if (addr >= size_characterRAM)
  {
    return;
  }

  CHR[addr] = byte;
  printf("Mapper::Mapper: %d wrote %u to CHR at address 0x%x\n", kind, byte, addr);
}


static nameTableMirroring_t getNameTableMirroring (const void* v_core)
{
  const mapper_t* core = v_core;
  const data_t* data = core -> data;
  const mapperAxROM_t* map = data -> next;
  nameTableMirroring_t ntm = map -> m_mirroring;
  return ntm;
}


static mapper_t* create (cartridge_t* cart, void (*mirroring_cb) (void))
{
  // constructs core (or super class):
  mapperKind_t kind = AxROM;
  mapper_t* core = mapper.create(cart, kind);
  if (core == NULL)
  {
    return core;
  }

  // constructs derived AxRMO mapper (or extending class):
  mapperAxROM_t* map = malloc( sizeof(mapperAxROM_t) );
  if (map == NULL)
  {
    core = mapper.destroy(core);
    return core;
  }

  // binds core to mapper AxROM:
  data_t* data = core -> data;
  data -> next = map;

  // overrides methods:
  core -> readPRG = readPRG;
  core -> readCHR = readCHR;
  core -> writePRG = writePRG;
  core -> writeCHR = writeCHR;
  core -> getNameTableMirroring = getNameTableMirroring;

  // memory allocations:
  size_t size_ROM = cart -> getSizeROM(cart);
  if (size_ROM >= 0x8000)
  {
    printf("Using PRG-ROM OK\n");
  }

  size_t size_characterRAM = 0;
  size_t size_VROM = cart -> getSizeVROM(cart);
  if (size_VROM != 0)
  {
    size_characterRAM = 0;
    map -> m_characterRAM = NULL;
  }
  else
  {
    size_characterRAM = 0x2000;
    map -> m_characterRAM = (byte_t*) malloc( size_characterRAM * sizeof(byte_t) );
    for (size_t i = 0; i != size_characterRAM; ++i)
    {
      map -> m_characterRAM[i] = ( (byte_t) 0x00 );
    }
    printf("Uses Character RAM OK\n");
  }

  if ( (size_VROM == 0) && (map -> m_characterRAM == NULL) )
  {
    core = mapper.destroy(core);
    map -> m_core = NULL;
    free(map);
    map = NULL;
    printf("failed to allocate Character RAM for mapper AxROM\n");
    return core;
  }

  // initializes mapper AxROM data:
  map -> m_core = core;
  map -> m_bank_PRG = ( (uint32_t) 0 );
  map -> m_size_ROM = size_ROM;
  map -> m_size_VROM = size_VROM;
  map -> m_size_characterRAM = size_characterRAM;
  map -> m_mirroring = OneScreenLower;

  map -> m_mirroringCallBack = mirroring_cb;

  return core;
}


static mapper_t* destroy (mapper_t* core)
{
  if (core == NULL)
  {
    return core;
  }

  data_t* data = core -> data;
  mapperAxROM_t* map = data -> next;
  core = mapper.destroy(core);
  map -> m_core = NULL;

  if (map -> m_characterRAM != NULL)
  {
    free(map -> m_characterRAM);
    map -> m_characterRAM = NULL;
  }

  map -> m_mirroringCallBack = NULL;

  free(map);
  map = NULL;
  return core;
}


mapperAxROM_namespace_t const mapperAxROM = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 02, 2023
//
//			Academic Purpose
//
// source: mapperAxROM.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the mapper AxROM object.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
