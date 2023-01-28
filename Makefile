rwildcard = $(foreach item,$(wildcard $1/*),$(call rwildcard,$(item),$2)$(filter $2,$(item)))

# Ajust as needed
BUILD := build
export LIB = $(notdir $(OUT))
SOURCES := src/Engine
INCLUDES := include/Engine
TEXTURES := Assets

ifneq ($(BUILD),$(notdir $(CURDIR)))

export CFLAGS = -g -Wall -ansi -pedantic
export LDFLAGS = -g

export PROJECT := Koneko
export OUT := $(CURDIR)/$(PROJECT)
export DEPSDIR := $(CURDIR)/$(BUILD)

SRC := $(call rwildcard,$(SOURCES),%.c)
SDL_BKD := $(call rwildcard,src/Backends/SDL,%.c)
GX_BKD := $(call rwildcard,src/Backends/GX,%.c)
NT_BKD := $(call rwildcard,src/Backends/NT,%.c)
UNIX_BKD := $(call rwildcard,src/Backends/UNIX,%.c)
LIBXML_BKD := $(call rwildcard,src/Backends/LIBXML,%.c)
MXML_BKD := $(call rwildcard,src/Backends/MXML,%.c)

SRC_DIR := $(dir $(SRC))
SDL_DIR := $(dir $(SDL_BKD))
GX_DIR := $(dir $(GX_BKD))
NT_DIR := $(dir $(NT_BKD))
UNIX_DIR := $(dir $(UNIX_BKD))
LIBXML_DIR := $(dir $(LIBXML_BKD))
MXML_DIR := $(dir $(MXML_BKD))

export SRC_FILES := $(notdir $(SRC))
export OBJ := $(SRC_FILES:%.c=%.o)

export SDL_FILES := $(notdir $(SDL_BKD))
export SDL_OBJ := $(SDL_FILES:%.c=%.o)

export GX_FILES := $(notdir $(GX_BKD))
export GX_OBJ := $(GX_FILES:%.c=%.o)

export NT_FILES := $(notdir $(NT_BKD))
export NT_OBJ := $(NT_FILES:%.c=%.o)

export UNIX_FILES := $(notdir $(UNIX_BKD))
export UNIX_OBJ := $(UNIX_FILES:%.c=%.o)

export LIBXML_FILES := $(notdir $(LIBXML_BKD))
export LIBXML_OBJ := $(LIBXML_FILES:%.c=%.o)

export MXML_FILES := $(notdir $(MXML_BKD))
export MXML_OBJ := $(MXML_FILES:%.c=%.o)

# Path make will search through to find target file candidates
export VPATH := $(foreach dir,$(SRC_DIR),$(CURDIR)/$(dir)) $(CURDIR)/$(TEXTURES) $(CURDIR)/$(BUILD)
export SDL_PATH := $(foreach dir,$(SDL_DIR),$(CURDIR)/$(dir))
export GX_PATH := $(foreach dir,$(GX_DIR),$(CURDIR)/$(dir))
export NT_PATH := $(foreach dir,$(NT_DIR),$(CURDIR)/$(dir))
export UNIX_PATH := $(foreach dir,$(UNIX_DIR),$(CURDIR)/$(dir))
export LIBXML_PATH := $(foreach dir,$(LIBXML_DIR),$(CURDIR)/$(dir))
export MXML_PATH := $(foreach dir,$(MXML_DIR),$(CURDIR)/$(dir))

export HEADERS := $(foreach hFile,$(call rwildcard,$(INCLUDES),%.h),$(CURDIR)/$(hFile))
export HEADERS += $(foreach hFile,$(call rwildcard,include/Backends,%.h),$(CURDIR)/$(hFile))

export INCLUDE := -I$(CURDIR)/$(BUILD) $(foreach hFile,$(HEADERS),-I$(dir $(hFile)))

.PHONY: $(BUILD) $(LIB) clean

linux: $(BUILD) $(LIB)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/NIXMakefile.mk

gamecube: $(BUILD) $(LIB)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/GCMakefile.mk

$(BUILD) $(LIB):
	@[ -d $@ ] || mkdir -p $@

clean:
	@echo clean ...
	@rm -rf $(BUILD) $(LIB) $(OUT) $(OUT).dol $(OUT).elf lib$(PROJECT).a

rebuild: clean linux

endif