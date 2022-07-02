rwildcard = $(foreach item,$(wildcard $1/*),$(call rwildcard,$(item),$2)$(filter $2,$(item)))

# Ajust as needed
BUILD := build
export LIB = $(notdir $(OUT))
SOURCES := src
INCLUDES := include
TEXTURES := Assets

ifneq ($(BUILD),$(notdir $(CURDIR)))

export CFLAGS = -g -O2 -Wall
export LDFLAGS = -g

export PROJECT := Koneko
export OUT := $(CURDIR)/$(PROJECT)
export DEPSDIR := $(CURDIR)/$(BUILD)

SRC_PATH := $(call rwildcard,$(SOURCES),%.c)
SRC_DIR := $(dir $(SRC_PATH))
export SRC_FILES := $(notdir $(SRC_PATH))
export OBJ := $(SRC_FILES:%.c=%.o)

# Path make will search through to find target file candidates
export VPATH := $(foreach dir,$(SRC_DIR),$(CURDIR)/$(dir)) $(CURDIR)/$(TEXTURES) $(CURDIR)/$(BUILD)

export HEADERS := $(foreach hFile,$(call rwildcard,$(INCLUDES),%.h),$(CURDIR)/$(hFile))

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