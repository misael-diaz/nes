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

extern cartridge_namespace_t const cartridge;

int main ()
{
  cartridge_t* c = cartridge.create();
  c -> loadFromFile(c);

  /*
  if (c -> m_PRG_ROM == NULL)
  {
    printf("PRG-ROM NOT OK\n");
  }

  if (c -> m_CHR_ROM == NULL)
  {
    printf("OK\n");
  }
  */

  c = cartridge.destroy(c);
  return 0;
}


// COMMENTS:
// I am doing this for fun and because I want to test my ability to port C++ to C code.
// By doing this I am aiming to learn more about these languages.
//
// A gamer passionate about programming should strive to do console emulation of one
// of the most epic consoles at some point in their career. Now it is my time.
