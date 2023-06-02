// NES Emulation					May 29, 2023
//
//			Academic Purpose
//
// source: main.c
// author: @misael-diaz
//
// Synopsis:
// Tests reading NES ROM.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
// [1] https://www.howtogeek.com/428987/whats-the-difference-between-ntsc-and-pal/

#include <stdio.h>

#include "cartridge.h"
#include "mapper.h"

#define SUCCESS ( (int) 0x00000000 )
#define FAILURE ( (int) 0xffffffff )

extern cartridge_namespace_t const cartridge;
extern mapper_namespace_t const mapper;

int main ()
{
  cartridge_t* c = cartridge.create();
  c -> loadFromFile(c);

  if (c == NULL)
  {
    return FAILURE;
  }

  printf("ROM size: %lu \n", c -> getSizeROM(c));
  if (c -> getROM(c) == NULL)
  {
    printf("PRG-ROM NOT OK\n");
  }

  printf("VROM size: %lu \n", c -> getSizeVROM(c));
  if (c -> getVROM(c) == NULL)
  {
    printf("OK\n");
  }

  mapperKind_t k = NROM;
  mapper_t* map = mapper.create(c, k);

  map -> readPRG(map, 0);
  map -> readCHR(map, 0);
  map -> writePRG(map, 0, 0);
  map -> writeCHR(map, 0, 0);
  nameTableMirroring_t ntm = map -> getNameTableMirroring(map);
  printf("Mapper::Mapper: %d name table mirroring: %d\n", k, ntm);
  printf("Mapper::Mapper: %d has extended RAM: %d\n", k, map -> hasExtendedRAM(map));
  map -> scanlineIRQ(map);

  map = mapper.destroy(map);
  c = cartridge.destroy(c);
  return SUCCESS;
}


// COMMENTS:
// I am doing this for fun and because I want to test my ability to port C++ to C code.
// By doing this I am aiming to learn more about these languages.
//
// A gamer passionate about programming should strive to do console emulation of one
// of the most epic consoles at some point in their career. Now it is my time.
