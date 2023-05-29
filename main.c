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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char byte;

typedef struct {
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
} cartridge_t;

// from Mapper
enum nameTableMirroring
{
  Horizontal = 0,
  Vertical = 1,
  FourScreen = 8
};

cartridge_t* create();
cartridge_t* destroy(cartridge_t*);

void loadFromFile(cartridge_t* c);

int main ()
{
  cartridge_t* c = create();
  loadFromFile(c);

  if (c -> m_PRG_ROM == NULL)
  {
    printf("PRG-ROM NOT OK\n");
  }

  if (c -> m_CHR_ROM == NULL)
  {
    printf("OK\n");
  }

  c = destroy(c);
  return 0;
}


cartridge_t* create ()
{
  cartridge_t* c = malloc( sizeof(cartridge_t) );
  if (c == NULL)
  {
    return c;
  }

  c -> header = NULL;
  c -> m_PRG_ROM = NULL;
  c -> m_CHR_ROM = NULL;
  c -> num_banks = 0;
  c -> num_vbanks = 0;
  c -> banks = 0;
  c -> vbanks = 0;
  c -> m_nameTableMirroring = 0;
  c -> m_mapperNumber = 0;
  c -> m_extendedRAM = false;

  return c;
}


cartridge_t* destroy (cartridge_t* c)
{
  if (c == NULL)
  {
    return c;
  }

  if (c -> header != NULL)
  {
    free(c -> header);
    c -> header= NULL;
  }

  if (c -> m_PRG_ROM != NULL)
  {
    free(c -> m_PRG_ROM);
    c -> m_PRG_ROM = NULL;
  }

  if (c -> m_CHR_ROM != NULL)
  {
    free(c -> m_CHR_ROM);
    c -> m_CHR_ROM = NULL;
  }

  free(c);
  c = NULL;
  return c;
}



int load_H_ROM (FILE* rom, cartridge_t* c)	// loads Header of ROM into cartridge
{
  size_t size = 0x10;
  byte header[size];
  size_t count = fread(header, sizeof(byte), size, rom);
  if (count != size)
  {
    printf("Invalid NES ROM\n");
    fclose(rom);
    return 0xFFFFFFFF;
  }

  printf("header: %c %c %c %x\n", header[0], header[1], header[2], header[3]);

  c -> header = (byte*) malloc( size * sizeof(byte) );
  if (c -> header == NULL)
  {
    printf("failed to allocate memory for ROM header!\n");
    fclose(rom);
    return 0xFFFFFFFF;
  }

  byte* h = c -> header;
  for (size_t i = 0; i != size; ++i)
  {
    h[i] = header[i];
  }

  return 0;
}


int load_PRG_ROM (FILE* rom, cartridge_t* c)
{
  byte* header = c -> header;
  byte banks = header[4];
  printf("16KB PRG-ROM Banks: %u \n", banks);
  if (!banks)
  {
    printf("16KB PRG-ROM Banks: %u \n", banks);
    printf("ROM does not have PRG-ROM Banks! Failed to load ROM\n");
    free(c -> header);
    c -> header = NULL;
    header = NULL;
    fclose(rom);
    return 0xFFFFFFFF;
  }

  c -> banks = banks;

  size_t num_banks = (0x4000 * banks);
  byte b_PRG_ROM[num_banks];
  size_t count = fread(b_PRG_ROM, sizeof(byte), num_banks, rom);
  if (num_banks != count)
  {
    printf("Failed to read PRG-ROM!\n");
    free(c -> header);
    c -> header = NULL;
    header = NULL;
    fclose(rom);
    return 0xFFFFFFFF;
  }
  c -> num_banks = num_banks;

  c -> m_PRG_ROM = (byte*) malloc( num_banks * sizeof(byte) );

  if (c -> m_PRG_ROM == NULL)
  {
    printf("failed to allocate memory for PRG-ROM!\n");
    free(c -> header);
    c -> header = NULL;
    header = NULL;
    fclose(rom);
    return 0xFFFFFFFF;
  }

  byte* m_PRG_ROM = c -> m_PRG_ROM;
  for (size_t i = 0; i != num_banks; ++i)
  {
    m_PRG_ROM[i] = b_PRG_ROM[i];
  }

  return 0;
}


int load_CHR_ROM (FILE* rom, cartridge_t* c)
{
  byte* header = c -> header;
  byte vbanks = header[5];
  c -> vbanks = vbanks;
  printf("8KB CHR-ROM Banks: %u \n", vbanks);

  if (vbanks)
  {
    size_t num_vbanks = (0x2000 * vbanks);
    byte b_CHR_ROM[num_vbanks];
    size_t count = fread(b_CHR_ROM, sizeof(byte), num_vbanks, rom);
    if (num_vbanks != count)
    {
      printf("Failed to read CHR-ROM!\n");
      free(c -> header);
      c -> header = NULL;
      header = NULL;
      free(c -> m_PRG_ROM);
      c -> m_PRG_ROM = NULL;
      fclose(rom);
      return 0xFFFFFFFF;
    }
    c -> num_vbanks = num_vbanks;

    c -> m_CHR_ROM = (byte*) malloc( num_vbanks * sizeof(byte) );

    if (c -> m_CHR_ROM == NULL)
    {
      printf("failed to allocate memory for CHR-ROM!\n");
      free(c -> header);
      c -> header = NULL;
      header = NULL;
      free(c -> m_PRG_ROM);
      c -> m_PRG_ROM = NULL;
      fclose(rom);
      return 0xFFFFFFFF;
    }

    byte* m_CHR_ROM = c -> m_CHR_ROM;
    for (size_t i = 0; i != num_vbanks; ++i)
    {
      m_CHR_ROM[i] = b_CHR_ROM[i];
    }
  }

  printf("ROM with CHR-RAM\n");
  return 0;
}


void setTableMirroring (cartridge_t* c)
{
  byte* header = c -> header;
  if (header[6] & 0x8)
  {
    enum nameTableMirroring mirroring;
    mirroring = FourScreen;
    byte m_nameTableMirroring = mirroring;
    printf("Name Table Mirroring: %u \n", m_nameTableMirroring);
    c -> m_nameTableMirroring = m_nameTableMirroring;
  }
  else
  {
    byte m_nameTableMirroring = (header[6] & 0x1);
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
    c -> m_nameTableMirroring = m_nameTableMirroring;
  }
}


void setMapperNumber (cartridge_t* c)
{
  byte* header = c -> header;
  byte m_mapperNumber = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);
  c -> m_mapperNumber = m_mapperNumber;
  printf("Mapper Number: %u \n", m_mapperNumber);
}


void setExtendedRAM (cartridge_t* c)
{
  byte* header = c -> header;
  bool m_extendedRAM = (header[6] & 0x2)? true : false;
  c -> m_extendedRAM = m_extendedRAM;
  printf("Extended CPU RAM: %u \n", m_extendedRAM);
}


void info_colorSystem (cartridge_t* c)
{
  byte* header = c -> header;
  //if ( (header[0xa] & 0x3) == 0x2 || (header[0xa] & 0x1) ) // what you should test
  if ( (header[0xa] & 0x1) )
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


int hasTrainerSupport (FILE* rom, cartridge_t* c)
{
  byte* header = c -> header;
  if (header[6] & 0x4)
  {
    printf("Unsupported Trainer!\n");
    free(c -> header);
    c -> header = NULL;
    header = NULL;
    fclose(rom);
    return 0xFFFFFFFF;
  }

  return 0;
}


void loadFromFile (cartridge_t* c)
{
  FILE* rom = fopen("ROM", "rb");
  if (rom == NULL)
  {
    printf("failed to read ROM\n");
    return;
  }

  // reads ROM header:

  int stat;
  stat = load_H_ROM(rom, c);
  if (stat != 0)
  {
    return;
  }

  // reads ROM data:

  stat = hasTrainerSupport(rom, c);
  if (stat != 0)
  {
    return;
  }

  stat = load_PRG_ROM(rom, c);
  if (stat != 0)
  {
    return;
  }

  stat = load_CHR_ROM(rom, c);
  if (stat != 0)
  {
    return;
  }

  setTableMirroring(c);
  setMapperNumber(c);
  setExtendedRAM(c);
  info_colorSystem(c);

  fclose(rom);
}


// COMMENTS:
// I am doing this for fun and because I want to test my ability to port C++ to C code.
// By doing this I am aiming to learn more about these languages.
//
// A gamer passionate about programming should strive to do console emulation of one
// of the most epic consoles at some point in their career. Now it is my time.


// TODO:
// [ ] check how ROMs unsupporting the PAL color system affect the SimpleNES emulator and
//     see if you can come up with a possible solution. Digging around has lead me to
//     find that PAL is used in Europe and Asia, whereas NTSC is used in the USA (ref[1]).
