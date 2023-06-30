#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bus.h"


static byte_t read (const void* vbus, address_t const address)
{
  const bus_t* bus = vbus;
  const byte_t* ram = bus -> ram;

  // NOTE: ignore GCC -Wtype-limits warning regarding that the condition is always true
  bool const isValidAddressableRange = (address >= 0x0000 && address <= 0xFFFF);
  if (isValidAddressableRange)
  {
    byte_t const data = ram[address];
    return data;
  }
  else
  {
    return 0x00;
  }
}


static void write (void* vbus, address_t const address, byte_t const data)
{
  bus_t* bus = vbus;
  byte_t* ram = bus -> ram;

  bool const isValidAddressableRange = (address >= 0x0000 && address <= 0xFFFF);
  if (isValidAddressableRange)
  {
    ram[address] = data;
  }
}


static bus_t* create (device_t* cpu)
{
  bus_t* bus = malloc( sizeof(bus_t) );
  if (bus == NULL)
  {
    printf("Bus::Bus() failed to allocate bus!\n");
    return bus;
  }

  size_t const ram_size = 64 * 1024;	// 64 KB (Fake) RAM
  bus -> ram = (byte_t*) malloc( ram_size * sizeof(byte_t) );
  if (bus -> ram == NULL)
  {
    free(bus);
    bus = NULL;
    printf("Bus::Bus() failed to allocate (Fake) RAM!\n");
    return bus;
  }

  byte_t* ram = bus -> ram;
  for (size_t i = 0; i != ram_size; ++i)
  {
    ram[i] = 0x00;
  }

  bus -> read = read;
  bus -> write = write;

  cpu -> ConnectBus(cpu, bus);

  return bus;
}


static bus_t* destroy (bus_t* bus)
{
  if (bus == NULL)
  {
    return bus;
  }

  free(bus -> ram);
  bus -> ram = NULL;

  free(bus);
  bus = NULL;
  return bus;
}


bus_namespace_t const bus = {
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
