#include <stdio.h>
#include <stdlib.h>
#include "device.h"


static void ConnectBus (void* vdev, void* vbus)
{
  device_t* dev = vdev;
  dev -> vbus = vbus;
}


static device_t* create ()
{
  device_t* dev = malloc( sizeof(device_t) );
  if (dev == NULL)
  {
    printf("dev::dev() failed to allocate dev!\n");
    return dev;
  }

  dev -> ConnectBus = ConnectBus;

  return dev;
}


static device_t* destroy (device_t* dev)
{
  if (dev == NULL)
  {
    return dev;
  }

  free(dev);
  dev = NULL;
  return dev;
}


device_namespace_t const device = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: dev.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the device type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
