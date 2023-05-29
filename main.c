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
// References:
// [0] https://github.com/amhndu/SimpleNES

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char byte;

typedef struct {
  byte* m_PRG_ROM;
  byte* m_CHR_ROM;
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

  c -> m_PRG_ROM = NULL;
  c -> m_CHR_ROM = NULL;
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


void loadFromFile (cartridge_t* c)
{
  FILE* rom = fopen("ROM", "rb");
  if (rom == NULL)
  {
    printf("failed to read ROM\n");
    return;
  }

  // reads ROM header:

  size_t size = 0x10;
  byte header[size];
  size_t count = fread(header, sizeof(byte), size, rom);
  if (count != size)
  {
    printf("Invalid NES ROM\n");
    fclose(rom);
    return;
  }

  printf("header: %c %c %c %x\n", header[0], header[1], header[2], header[3]);

  byte banks = header[4];
  printf("16KB PRG-ROM Banks: %u \n", banks);
  if (!banks)
  {
    printf("16KB PRG-ROM Banks: %u \n", banks);
    printf("ROM does not have PRG-ROM Banks! Failed to load ROM\n");
    fclose(rom);
    return;
  }

  // reads ROM data:

  byte vbanks = header[5];
  printf("8KB CHR-ROM Banks: %u \n", vbanks);

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

  byte m_mapperNumber = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);
  c -> m_mapperNumber = m_mapperNumber;
  printf("Mapper Number: %u \n", m_mapperNumber);

  bool m_extendedRAM = (header[6] & 0x2)? true : false;
  c -> m_extendedRAM = m_extendedRAM;
  printf("Extended CPU RAM: %u \n", m_extendedRAM);

  if (header[6] & 0x4)
  {
    printf("Unsupported Trainer!\n");
    fclose(rom);
    return;
  }

  //if ( (header[0xa] & 0x3) == 0x2 || (header[0xa] & 0x1) ) // what you should test
  if ( (header[0xa] & 0x1) )
  {
    // NOTE: dunno why the ROM is not PAL compatible and how serious that really is
    printf("Unsupported PAL ROM!\n");
    fclose(rom);
    return;
  }
  else
  {
    printf("ROM is NTSC compatible\n");
  }

  size_t num_banks = (0x4000 * banks);
  byte b_PRG_ROM[num_banks];
  count = fread(b_PRG_ROM, sizeof(byte), num_banks, rom);
  if (num_banks != count)
  {
    printf("Failed to read PRG-ROM!\n");
    fclose(rom);
    return;
  }

  c -> m_PRG_ROM = (byte*) malloc( num_banks * sizeof(byte) );

  if (c -> m_PRG_ROM == NULL)
  {
    printf("failed to allocate memory for PRG-ROM!\n");
    fclose(rom);
    return;
  }

  byte* m_PRG_ROM = c -> m_PRG_ROM;
  for (size_t i = 0; i != num_banks; ++i)
  {
    m_PRG_ROM[i] = b_PRG_ROM[i];
  }

  if (vbanks)
  {
    size_t num_vbanks = (0x2000 * vbanks);
    byte b_CHR_ROM[num_vbanks];
    count = fread(b_CHR_ROM, sizeof(byte), num_vbanks, rom);
    if (num_vbanks != count)
    {
      printf("Failed to read CHR-ROM!\n");
      free(c -> m_PRG_ROM);
      c -> m_PRG_ROM = NULL;
      fclose(rom);
      return;
    }

    c -> m_CHR_ROM = (byte*) malloc( num_vbanks * sizeof(byte) );

    if (c -> m_CHR_ROM == NULL)
    {
      printf("failed to allocate memory for CHR-ROM!\n");
      free(c -> m_PRG_ROM);
      c -> m_PRG_ROM = NULL;
      fclose(rom);
      return;
    }

    byte* m_CHR_ROM = c -> m_CHR_ROM;
    for (size_t i = 0; i != num_vbanks; ++i)
    {
      m_CHR_ROM[i] = b_CHR_ROM[i];
    }
  }

  printf("ROM with CHR-RAM\n");

  fclose(rom);
}


// COMMENTS:
// I am doing this for fun and because I want to test my ability to port C++ to C code.
// By doing this I am aiming to learn more about these languages.
//
// A gamer passionate about programming should strive to do console emulation of one
// of the most epic consoles at some point in their career. Now it is my time.
