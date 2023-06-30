#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"


static byte_t read (const void* vcpu, address_t const address)
{
  const cpu_t* cpu = vcpu;
  const bus_t* bus = cpu -> bus;
  byte_t const data = bus -> read(bus, address);
  return data;
}


static void write (void* vcpu, address_t const address, byte_t const data)
{
  cpu_t* cpu = vcpu;
  bus_t* bus = cpu -> bus;
  bus -> write(bus, address, data);
}


static void ConnectBus (cpu_t* cpu, const device_t* devCPU)
{
  bus_t* bus = devCPU -> vbus;
  cpu -> bus = bus;
}


static cpu_t* create (device_t* devCPU)
{
  cpu_t* cpu = malloc( sizeof(cpu_t) );
  if (cpu == NULL)
  {
    printf("CPU::CPU() failed to allocate CPU!\n");
    return cpu;
  }

  cpu -> read = read;
  cpu -> write = write;

  ConnectBus(cpu, devCPU);

  return cpu;
}


static cpu_t* destroy (cpu_t* cpu)
{
  if (cpu == NULL)
  {
    return cpu;
  }

  free(cpu);
  cpu = NULL;
  return cpu;
}


cpu_namespace_t const cpu = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: cpu.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the CPU object.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
