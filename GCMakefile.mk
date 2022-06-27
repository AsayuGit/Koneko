ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

export CFLAGS := -g -O2 -Wall $(MACHDEP) $(INCLUDE)
export LDFLAGS := -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

LIBS := -logc -lm

# Main target
$(OUT).dol: $(OUT).elf
$(OUT).elf: $(OBJ)

%.tpl.o: %.tpl
	$(bin2o)

-include $(DEPSDIR)/*.d