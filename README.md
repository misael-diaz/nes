# NES Emulation

**Academic Purpose**

I am doing this to learn about game console emulation and because it is fun!

This work is a port of [SimpleNES](https://github.com/amhndu/SimpleNES) to C.

## Compiling the NES Emulator with GNU Make

I am assuming that you are familiar with Linux and that you have some experience working
with the terminal application.

To compile the NES emulator open the terminal application and clone this repository:

```sh
git clone https://github.com/misael-diaz/nes.git
```

And compile with GNU make at the top level of the repository (if you see the LICENSE
file you are at the top level) by issuing the following command:

```sh
make
```

Possible output if all goes well:

```sh
make[1]: Entering directory '/home/username/.../git/nes/src'
gcc -g -Wall -Wextra -O0 -I../../nes/inc -c cartridge.c -o cartridge.o
gcc -g -Wall -Wextra -O0 -I../../nes/inc -c main.c -o main.o
gcc -g -Wall -Wextra -O0 cartridge.o main.o -o ../../nes/nes-emulator
make[1]: Leaving directory '/home/username/.../git/nes/src'
```

If you see the following files that means that means that you have successfully compiled
the emulator from source:

```
inc  LICENSE  Makefile  make-inc  nes-emulator  README.md  ROM  src
```

As you might have guessed, the nes-emulator is the NES Emulator binary and the ROM is a
NES ROM that you will have to provide if you wish to test the emulator.

## Using the NES Emulator

The NES Emulator expects the ROM to be located at the same level (as shown at the end of
the previous section) as the emulator for it to work.

To run the emulator input the following command on your terminal:

```sh
./nes-emulator
```

Possible output if all goes well:

```
header: N E S 1a
16KB PRG-ROM Banks: 8
8KB CHR-ROM Banks: 0
Name Table Mirroring: Vertical
Mapper Number: 1
Extended CPU RAM: 0
ROM is NTSC compatible
ROM size: 131072
VROM size: 0
OK
Using PRG-ROM OK
Uses Character RAM OK
mirroring callback from mapper
Mapper::Mapper: 7 wrote 1 to bank-PRG
Mapper::Mapper: 7 wrote 255 to CHR at address 0x1000
Mapper::Mapper: 7 read 76 from PRG at address 0x8000
Mapper::Mapper: 7 read 255 CHR at address 0x1000
Mapper::Mapper: 7 name table mirroring: 9
Mapper::Mapper: 7 has extended RAM: 0
Mapper::Mapper: 7 Mapper::scalineIRQ
```

If you see something similar that means that the emulator was able to load the ROM
and instantiate the AxROM mapper. If not, feel free to create an issue.

That is all that has been implemented so far. Not much, but exciting nevertheless!
