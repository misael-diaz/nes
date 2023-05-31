#include <stdio.h>
#include "cartridge.h"


#define NES_FAILURE_STATE ( (int) 0xffffffff )
#define NES_SUCCESS_STATE ( (int) 0x00000000 )


// cartridge private data typedef:
typedef struct
{
  byte* header;
  byte* m_PRG_ROM;
  byte* m_CHR_ROM;
  size_t num_banks;
  size_t num_vbanks;
  byte banks;
  byte vbanks;
  byte m_nameTableMirroring;
  byte m_mapperNumber;
  bool m_extendedRAM;
} data_t;


static void util_copy (size_t size, byte* restrict dst, const byte* restrict src)
{
  for (size_t i = 0; i != size; ++i)
  {
    dst[i] = src[i];
  }
}


static int load_H_ROM (FILE* rom, cartridge_t* c)	// loads ROM Header into cartridge
{
  size_t size = 0x10;
  byte header[size];
  size_t count = fread(header, sizeof(byte), size, rom);
  if (count != size)
  {
    printf("Invalid NES ROM\n");
    fclose(rom);
    return NES_FAILURE_STATE;
  }

  printf("header: %c %c %c %x\n", header[0], header[1], header[2], header[3]);

  data_t* d = c -> data;
  d -> header = (byte*) malloc( size * sizeof(byte) );
  if (d -> header == NULL)
  {
    printf("failed to allocate memory for ROM header!\n");
    fclose(rom);
    return NES_FAILURE_STATE;
  }

  byte* h = d -> header;
  util_copy(size, h, header);

  return NES_SUCCESS_STATE;
}


static int load_PRG_ROM (FILE* rom, cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte banks = header[4];
  printf("16KB PRG-ROM Banks: %u \n", banks);
  if (!banks)
  {
    printf("16KB PRG-ROM Banks: %u \n", banks);
    printf("ROM does not have PRG-ROM Banks! Failed to load ROM\n");
    free(d -> header);
    d -> header = NULL;
    header = NULL;
    free(c -> data);
    c -> data = NULL;
    d = NULL;
    fclose(rom);
    return NES_FAILURE_STATE;
  }

  d -> banks = banks;

  size_t num_banks = (0x4000 * banks);
  byte b_PRG_ROM[num_banks];
  size_t count = fread(b_PRG_ROM, sizeof(byte), num_banks, rom);
  if (num_banks != count)
  {
    printf("Failed to read PRG-ROM!\n");
    free(d -> header);
    d -> header = NULL;
    header = NULL;
    free(c -> data);
    c -> data = NULL;
    d = NULL;
    fclose(rom);
    return NES_FAILURE_STATE;
  }
  d -> num_banks = num_banks;

  d -> m_PRG_ROM = (byte*) malloc( num_banks * sizeof(byte) );

  if (d -> m_PRG_ROM == NULL)
  {
    printf("failed to allocate memory for PRG-ROM!\n");
    free(d -> header);
    d -> header = NULL;
    header = NULL;
    free(c -> data);
    c -> data = NULL;
    d = NULL;
    fclose(rom);
    return NES_FAILURE_STATE;
  }

  byte* m_PRG_ROM = d -> m_PRG_ROM;
  util_copy(num_banks, m_PRG_ROM, b_PRG_ROM);

  return NES_SUCCESS_STATE;
}


static int load_CHR_ROM (FILE* rom, cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte vbanks = header[5];
  d -> vbanks = vbanks;
  printf("8KB CHR-ROM Banks: %u \n", vbanks);

  if (vbanks)
  {
    size_t num_vbanks = (0x2000 * vbanks);
    byte b_CHR_ROM[num_vbanks];
    size_t count = fread(b_CHR_ROM, sizeof(byte), num_vbanks, rom);
    if (num_vbanks != count)
    {
      printf("Failed to read CHR-ROM!\n");
      free(d -> header);
      d -> header = NULL;
      header = NULL;
      free(d -> m_PRG_ROM);
      d -> m_PRG_ROM = NULL;
      free(c -> data);
      c -> data = NULL;
      d = NULL;
      fclose(rom);
      return NES_FAILURE_STATE;
    }
    d -> num_vbanks = num_vbanks;

    d -> m_CHR_ROM = (byte*) malloc( num_vbanks * sizeof(byte) );

    if (d -> m_CHR_ROM == NULL)
    {
      printf("failed to allocate memory for CHR-ROM!\n");
      free(d -> header);
      d -> header = NULL;
      header = NULL;
      free(d -> m_PRG_ROM);
      d -> m_PRG_ROM = NULL;
      free(c -> data);
      c -> data = NULL;
      d = NULL;
      fclose(rom);
      return NES_FAILURE_STATE;
    }

    byte* m_CHR_ROM = d -> m_CHR_ROM;
    util_copy(num_vbanks, m_CHR_ROM, b_CHR_ROM);
  }

  printf("ROM with CHR-RAM\n");
  return NES_SUCCESS_STATE;
}


static void setTableMirroring (cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte const isFourScreenMirroringBitSet = (header[6] & 0x08);
  if (isFourScreenMirroringBitSet)
  {
    enum nameTableMirroring // from Mapper, placed here temporarily
    {
      Horizontal = 0,
      Vertical = 1,
      FourScreen = 8
    };
    enum nameTableMirroring mirroring;
    mirroring = FourScreen;
    byte m_nameTableMirroring = mirroring;
    printf("Name Table Mirroring: %u \n", m_nameTableMirroring);
    d -> m_nameTableMirroring = m_nameTableMirroring;
  }
  else
  {
    byte const isTableMirroringBitSet = (header[6] & 0x01);
    byte m_nameTableMirroring = isTableMirroringBitSet;
    switch (m_nameTableMirroring)
    {
      case 0:
	printf("Name Table Mirroring: Horizontal\n");
	break;
      case 1:
	printf("Name Table Mirroring: Vertical\n");
	break;
      default:
	printf("Name Table Mirroring: Unknown\n");
	break;
    }
    d -> m_nameTableMirroring = m_nameTableMirroring;
  }
}


static void setMapperNumber (cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte m_mapperNumber = ( ( (header[6] >> 4) & 0x0f ) | (header[7] & 0xf0) );
  d -> m_mapperNumber = m_mapperNumber;
  printf("Mapper Number: %u \n", m_mapperNumber);
}


static void setExtendedRAM (cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte const isExtendedRAMBitSet = (header[6] & 0x02);
  bool m_extendedRAM = (isExtendedRAMBitSet)? true : false;
  d -> m_extendedRAM = m_extendedRAM;
  printf("Extended CPU RAM: %u \n", m_extendedRAM);
}


static void info_colorSystem (cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte const isColorSystemBitSet = (header[0x0a] & 0x01);
  //if ( (header[0xa] & 0x3) == 0x2 || (header[0xa] & 0x1) ) // what you should test
  if (isColorSystemBitSet)
  {
    // NOTE: dunno why the ROM is not PAL compatible and how serious that really is
    // Perhaps it is okay if it supports one or the other but this will have to be
    // resolved later.
    printf("Unsupported PAL ROM!\n");
  }
  else
  {
    printf("ROM is NTSC compatible\n");
  }
}


static int hasTrainerSupport (FILE* rom, cartridge_t* c)
{
  data_t* d = c -> data;
  byte* header = d -> header;
  byte const isTrainerBitSet = (header[6] & 0x04);
  if (isTrainerBitSet)
  {
    printf("Unsupported Trainer!\n");
    free(d -> header);
    d -> header = NULL;
    header = NULL;
    free(c -> data);
    c -> data = NULL;
    d = NULL;
    fclose(rom);
    return NES_FAILURE_STATE;
  }

  return NES_SUCCESS_STATE;
}


static void loadFromFile (void* v_cartridge)
{
  cartridge_t* c = v_cartridge;
  FILE* rom = fopen("ROM", "rb");
  if (rom == NULL)
  {
    printf("failed to read ROM\n");
    return;
  }

  // reads ROM header:

  int stat;
  stat = load_H_ROM(rom, c);
  if (stat == NES_FAILURE_STATE)
  {
    return;
  }

  // reads ROM data:

  stat = hasTrainerSupport(rom, c);
  if (stat == NES_FAILURE_STATE)
  {
    return;
  }

  stat = load_PRG_ROM(rom, c);
  if (stat == NES_FAILURE_STATE)
  {
    return;
  }

  stat = load_CHR_ROM(rom, c);
  if (stat == NES_FAILURE_STATE)
  {
    return;
  }

  setTableMirroring(c);
  setMapperNumber(c);
  setExtendedRAM(c);
  info_colorSystem(c);

  fclose(rom);
}


static cartridge_t* create ()
{
  cartridge_t* c = malloc( sizeof(cartridge_t) );
  if (c == NULL)
  {
    return c;
  }

  c -> data = (data_t*) malloc( sizeof(data_t) );
  if (c -> data == NULL)
  {
    free(c);
    c = NULL;
    printf("failed to allocate memory for the cartridge data!\n");
    return c;
  }

  data_t* d = c -> data;
  d -> header = NULL;
  d -> m_PRG_ROM = NULL;
  d -> m_CHR_ROM = NULL;
  d -> num_banks = 0;
  d -> num_vbanks = 0;
  d -> banks = 0;
  d -> vbanks = 0;
  d -> m_nameTableMirroring = 0;
  d -> m_mapperNumber = 0;
  d -> m_extendedRAM = false;

  c -> loadFromFile = loadFromFile;

  return c;
}


static cartridge_t* destroy (cartridge_t* c)
{
  if (c == NULL)
  {
    return c;
  }

  if (c -> data == NULL)
  {
    free(c);
    c = NULL;
    return c;
  }

  data_t* d = c -> data;
  if (d -> header != NULL)
  {
    free(d -> header);
    d -> header= NULL;
  }

  if (d -> m_PRG_ROM != NULL)
  {
    free(d -> m_PRG_ROM);
    d -> m_PRG_ROM = NULL;
  }

  if (d -> m_CHR_ROM != NULL)
  {
    free(d -> m_CHR_ROM);
    d -> m_CHR_ROM = NULL;
  }

  free(c -> data);
  c -> data = NULL;
  d = NULL;

  free(c);
  c = NULL;
  return c;
}


cartridge_namespace const cartridge = {
  .create = create,
  .destroy = destroy
};


// NES Emulation					May 29, 2023
//
//			Academic Purpose
//
// source: cartridge.c
// author: @misael-diaz
//
// Synopsis:
// Implements the methods of the cartridge object.
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


// TODO:
// [ ] check how ROMs unsupporting the PAL color system affect the SimpleNES emulator and
//     see if you can come up with a possible solution. Digging around has lead me to
//     find that PAL is used in Europe and Asia, whereas NTSC is used in the USA (ref[1]).
