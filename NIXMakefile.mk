CC := gcc
LD := gcc
AR := ar

LIBS = $$(sdl2-config --cflags) $$(xml2-config --cflags)

ARCHIVE := $(dir $(OUT))/lib$(notdir $(OUT)).a

.PHONY: archive copy

$(OUT): archive copy

%.o: %.c
	@$(CC) $(CFLAGS) -c $< $(INCLUDE) $(LIBS) -o $@

archive: $(OBJ)
	@$(AR) rcs $(ARCHIVE) $^

copy: $(HEADERS)
	@cp -u $^ $(ARCHIVE) $(dir $(OUT))/$(LIB)/