ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

export CFLAGS := -g -O2 -Wall $(MACHDEP) $(INCLUDE)

ARCHIVE := $(dir $(OUT))/lib$(notdir $(OUT)).a

.PHONY: package copy
# Main target
package: $(dir $(OUT))/lib$(PROJECT).a copy
	@echo "gamecube build ready!"

$(dir $(OUT))/lib$(PROJECT).a: $(OBJ)

%.tpl.o: %.tpl
	$(bin2o)

copy: $(HEADERS)
	@cp -u $^ $(ARCHIVE) $(dir $(OUT))/$(LIB)/

-include $(DEPSDIR)/*.d