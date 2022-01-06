APPS = get_elf_func_offset  get_elf_func_offset_by_readelf

.PHONY:all

all:$(APPS)

$(APPS): %: %.c
	gcc  $< -lelf -o $@

.PHONY:clean

clean:
	rm -rf $(APPS)