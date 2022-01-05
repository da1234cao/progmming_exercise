.PHONY:all

all:get_elf_func_offset

get_elf_func_offset:get_elf_func_offset.c
	gcc  $^ -lelf -o $@

.PHONY:clean

clean:
	rm -rf get_elf_func_offset