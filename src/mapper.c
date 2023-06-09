#include <stdio.h>
#include "mapper.h"


// mapper protected data typedef:
typedef struct
{
  cartridge_t* m_cartridge;
  mapperKind_t m_kind;
  void* next;
} data_t;


static byte_t readPRG (const void* v_mapper, const address_t addr)
{
  const mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  mapperKind_t const kind = data -> m_kind;
  printf("Mapper::Mapper: %d reading PRG address %x ... done\n", kind, addr);
  return 0;
}


static byte_t readCHR (const void* v_mapper, const address_t addr)
{
  const mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  mapperKind_t const kind = data -> m_kind;
  printf("Mapper::Mapper: %d reading CHR address %x ... done\n", kind, addr);
  return 0;
}


static void writePRG (void* v_mapper, const address_t addr, const byte_t byte)
{
  mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  mapperKind_t const kind = data -> m_kind;
  printf("Mapper::Mapper: %d writing %x PRG address %x ... done\n", kind, byte, addr);
}


static void writeCHR (void* v_mapper, const address_t addr, const byte_t byte)
{
  mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  mapperKind_t const kind = data -> m_kind;
  printf("Mapper::Mapper: %d writing %x CHR address %x ... done\n", kind, byte, addr);
}


static nameTableMirroring_t getNameTableMirroring (const void* v_mapper)
{
  const mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  const cartridge_t* m_cartridge = data -> m_cartridge;
  nameTableMirroring_t const ntm = m_cartridge -> getNameTableMirroring(m_cartridge);
  return ntm;
}


static bool hasExtendedRAM (const void* v_mapper)
{
  const mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  const cartridge_t* m_cartridge = data -> m_cartridge;
  bool const ret = m_cartridge -> hasExtendedRAM(m_cartridge);
  return ret;
}


static void scanlineIRQ (void* v_mapper)
{
  mapper_t* mapper = v_mapper;
  const data_t* data = mapper -> data;
  mapperKind_t const kind = data -> m_kind;
  printf("Mapper::Mapper: %d Mapper::scalineIRQ\n", kind);
}


static mapper_t* create (cartridge_t* cart, const mapperKind_t kind)
{
  mapper_t* mapper = malloc( sizeof(mapper_t) );
  if (mapper == NULL)
  {
    return mapper;
  }

  mapper -> data = (data_t*) malloc( sizeof(data_t) );
  if (mapper -> data == NULL)
  {
    free(mapper);
    mapper = NULL;
    printf("failed to allocate memory for the mapper data!\n");
    return mapper;
  }

  data_t* data = mapper -> data;
  data -> m_cartridge = cart;
  data -> m_kind = kind;

  mapper -> readPRG = readPRG;
  mapper -> readCHR = readCHR;
  mapper -> writePRG = writePRG;
  mapper -> writeCHR = writeCHR;
  mapper -> getNameTableMirroring = getNameTableMirroring;
  mapper -> hasExtendedRAM = hasExtendedRAM;
  mapper -> scanlineIRQ = scanlineIRQ;

  return mapper;
}


static mapper_t* destroy (mapper_t* mapper)
{
  if (mapper == NULL)
  {
    return mapper;
  }

  if (mapper -> data == NULL)
  {
    free(mapper);
    mapper = NULL;
    return mapper;
  }

  data_t* data = mapper -> data;
  data -> m_cartridge = NULL;

  free(mapper -> data);
  mapper -> data = NULL;
  data = NULL;

  free(mapper);
  mapper = NULL;
  return mapper;
}


mapper_namespace_t const mapper = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 01, 2023
//
//			Academic Purpose
//
// source: mapper.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the mapper object.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
