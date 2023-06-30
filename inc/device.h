#ifndef NES_DEVICE_TYPE_H
#define NES_DEVICE_TYPE_H

typedef struct
{
  void* vbus;
  void (*ConnectBus) (void*, void*);
} device_t;

typedef struct
{
  device_t* (*create) ();
  device_t* (*destroy) (device_t*);
} device_namespace_t;

#endif

// NES Emulation					June 30, 2023
//
//			Academic Purpose
//
// source: device.h
// author: @misael-diaz
//
// Synopsis:
// Device header file.
// Defines the device type.
// Ports SimpleNES (reference [0]) to clang for learning purposes.
//
// Copyright (c) 2023 Misael Diaz-Maldonado
// This file is released under the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// References:
// [0] https://github.com/amhndu/SimpleNES
