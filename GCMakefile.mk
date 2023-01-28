ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

export INCLUDE += -I$(LIBOGC_INC) -I$(DEVKITPRO)/portlibs/ppc/include

export CFLAGS += $(MACHDEP) $(INCLUDE)

ARCHIVE := $(dir $(OUT))/lib$(notdir $(OUT)).a

.PHONY: package copy
# Main target
package: $(dir $(OUT))/lib$(PROJECT).a copy
	@echo "gamecube build ready!"

$(dir $(OUT))/lib$(PROJECT).a: $(OBJ) $(GX_OBJ) $(UNIX_OBJ) $(MXML_OBJ)

%.tpl.o: %.tpl
	$(bin2o)

copy: $(HEADERS)
	@cp -u $^ $(ARCHIVE) $(dir $(OUT))/$(LIB)/

-include $(DEPSDIR)/*.d