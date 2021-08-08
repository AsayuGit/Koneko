#   Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
#   using SDL and libxml2. This engine is meant to allow game developpement
#   for Linux, Windows and the og Xbox.
#
#   Copyright (C) 2021 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; version 2 of the License.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Recursive Wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

# Remove duplicate function
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1))) 

# C Compilers
CC = gcc
WINCC = x86_64-w64-mingw32-gcc

# Archiver
ARC = ar

# Compile / Link Flags
CFLAGS += -c -Wall -std=c89 -pedantic-errors -g
LDFLAGS = $$(sdl2-config --libs) $$(xml2-config --libs) -lSDL2_image -lSDL2_mixer

# Main target and filename of the executable
OUT = Koneko

# Build Directory
BUILD_DIR = build

# List of all the .c source files to compile
SRC = $(call rwildcard,,*.c)

# List of all the .o object files to produce
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(SRC:%.c=%.o))
OBJ_DIR = $(call uniq, $(dir $(OBJ)))

# List of all includes directory
INCLUDES = $(call rwildcard,, include/*.h)
INCLUDES_DIR = $(patsubst %, -I %, $(call uniq, $(dir $(INCLUDES))))
LIBS = $$(sdl2-config --cflags) $$(xml2-config --cflags)

# Number of therads available 
CPUS = $(nproc)

multi:
	@$(MAKE) -j$(CPUS) --no-print-directory all
	
copy: $(INCLUDES)
	@mkdir -p $(OUT)
	@cp -u $^ lib$(OUT).a $(OUT)/

all: $(OBJ_DIR) $(OUT)
	@$(MAKE) copy

$(OBJ_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $< $(INCLUDES_DIR) $(LIBS) -o $@

$(OUT): $(OBJ)
	@echo "Archiving $@"
	@$(ARC) rcs lib$(OUT).a $^

clean:
	@echo "Cleaning Build"
	@rm -rf $(BUILD_DIR) $(OUT) lib$(OUT)*.a

rebuild: clean
	@$(MAKE) -j$(CPUS) --no-print-directory all
