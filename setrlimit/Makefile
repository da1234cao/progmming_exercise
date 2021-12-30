.PHONY:all
.PHONY:clean

all:mem_limit

mem_limit:mem_limit.c get_mem.h
	gcc -o $@ $^

clean:
	rm -rf mem_limit