#!/usr/bin/make
#
# NES Emulation			Academic Purpose		May 30, 2023
#
# source: Makefile
# author: @misael-diaz
#
# Synopsis:
# Defines the Makefile for building the application with GNU make.
#
# Copyright (c) 2023 Misael Diaz-Maldonado
#
# This file is released under the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#

include make-inc

all: $(TESTS)

$(MAIN_BIN): $(OBJECTS)
	$(CC) $(CCOPT) $(OBJECTS) -o $(MAIN_BIN) $(LIBS)

$(CARTRIDGE_OBJ): $(CARTRIDGE_SRC)
	$(CC) $(CCOPT) $(INC) -c $(CARTRIDGE_SRC) -o $(CARTRIDGE_OBJ)

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CCOPT) $(INC) -c $(MAIN_SRC) -o $(MAIN_OBJ)

clean:
	/bin/rm -rf *.o $(TESTS)
