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

$(DEV_OBJ): $(DEV_SRC)
	$(CC) $(CCOPT) $(INC) -c $(DEV_SRC) -o $(DEV_OBJ)

$(BUS_OBJ): $(DEV_OBJ) $(BUS_SRC)
	$(CC) $(CCOPT) $(INC) -c $(BUS_SRC) -o $(BUS_OBJ)

$(CPU_OBJ): $(DEV_OBJ) $(CPU_SRC)
	$(CC) $(CCOPT) $(INC) -c $(CPU_SRC) -o $(CPU_OBJ)

$(CARTRIDGE_OBJ): $(CARTRIDGE_SRC)
	$(CC) $(CCOPT) $(INC) -c $(CARTRIDGE_SRC) -o $(CARTRIDGE_OBJ)

$(MAPPER_OBJ): $(CARTRIDGE_OBJ) $(MAPPER_SRC)
	$(CC) $(CCOPT) $(INC) -c $(MAPPER_SRC) -o $(MAPPER_OBJ)

$(MAPPER_AXROM_OBJ): $(MAPPER_OBJ) $(MAPPER_AXROM_SRC)
	$(CC) $(CCOPT) $(INC) -c $(MAPPER_AXROM_SRC) -o $(MAPPER_AXROM_OBJ)

$(MAPPER_CNROM_OBJ): $(MAPPER_OBJ) $(MAPPER_CNROM_SRC)
	$(CC) $(CCOPT) $(INC) -c $(MAPPER_CNROM_SRC) -o $(MAPPER_CNROM_OBJ)

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CCOPT) $(INC) -c $(MAIN_SRC) -o $(MAIN_OBJ)

clean:
	/bin/rm -rf *.o $(TESTS)
