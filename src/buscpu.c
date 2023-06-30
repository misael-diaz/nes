#include <stdio.h>
#include <stdlib.h>
#include "buscpu.h"


static void ConnectBus (void* vbusCPU, void* vbus)
{
  buscpu_t* busCPU = vbusCPU;
  busCPU -> vbus = vbus;
}


static buscpu_t* create ()
{
  buscpu_t* busCPU = malloc( sizeof(buscpu_t) );
  if (busCPU == NULL)
  {
    printf("busCPU::busCPU() failed to allocate busCPU!\n");
    return busCPU;
  }

  busCPU -> ConnectBus = ConnectBus;

  return busCPU;
}


static buscpu_t* destroy (buscpu_t* busCPU)
{
  if (busCPU == NULL)
  {
    return busCPU;
  }

  free(busCPU);
  busCPU = NULL;
  return busCPU;
}


buscpu_namespace_t const buscpu = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: buscpu.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the bus of the CPU.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
