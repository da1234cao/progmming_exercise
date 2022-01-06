#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

// What are the meanings of the columns of the symbol table displayed by readelf?:
// https://stackoverflow.com/questions/3065535/what-are-the-meanings-of-the-columns-of-the-symbol-table-displayed-by-readelf
struct elf_sym {
    int sym_num;
    off_t offset;
    int size;
    char type[10];
    char bind[10];
    char visibility[10];
    int sec_num;
    char name[30];
};

off_t get_elf_func_offset_by_readelf(const char *path, const char *func){
    char cmomand[100];
    FILE *fp;
    sprintf(cmomand,"readelf --symbols %s | grep %s",path,func);
    if((fp = popen(cmomand,"r")) == NULL){
        fprintf(stderr,"popen fail...");
        exit(1);
    }

    char *line=NULL;
    size_t line_sz = 0;
    off_t result=-1;
    while(getline(&line,&line_sz,fp) >= 0){
        struct elf_sym sym;
        if(sscanf(line, "%d: %16lx %d %s %s %s %d %20s",&sym.sym_num,&sym.offset,&sym.size,sym.type,sym.bind,sym.visibility,&sym.sec_num,sym.name) < 1)
            continue;
        if(strcmp(sym.name,func) == 0){
            result = sym.offset;
            break;
        }
    }

    pclose(fp);
    return result;
}

int main(void){
    char *path = "/bin/bash";
    char *func = "readline";
    off_t result = get_elf_func_offset_by_readelf(path,func);
    printf("%ld\n",result);
}