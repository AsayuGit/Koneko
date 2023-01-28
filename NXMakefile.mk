ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

include $(DEVKITPRO)/libnx/switch_rules

export INCLUDE += -I$(PORTLIBS)/include -I$(LIBNX)/include -I$(PORTLIBS)/include/libxml2

export CFLAGS += -D__SWITCH__ $(INCLUDE)

ARCHIVE := $(dir $(OUT))/lib$(notdir $(OUT)).a

export VPATH += $(SDL_PATH) $(UNIX_PATH) $(LIBXML_PATH)

.PHONY: package copy

# Main target
package: $(dir $(OUT))/lib$(PROJECT).a copy
	@echo "switch build ready!"

$(dir $(OUT))/lib$(PROJECT).a: $(OBJ) $(SDL_OBJ) $(UNIX_OBJ) $(LIBXML_OBJ)

copy: $(HEADERS)
	@cp -u $^ $(ARCHIVE) $(dir $(OUT))/$(LIB)/


debug:
	echo $(PORTLIBS)

-include $(DEPSDIR)/*.d