#include <stdio.h>
#include <sys/resource.h>
#include <errno.h>
#include "get_mem.h"

#define MEM_LIM 8*1024*1024 // 最大虚拟内存为6MB

int main(void){
    struct rlimit old_limit;
    struct rlimit new_limit;

    /**
     * 打印当前的虚拟内存限制。
     * RLIM_INFINITY=0xffffffffffffffffuLL=18446744073709551615
    */
    getrlimit(RLIMIT_AS,&old_limit);
    printf("the process's old soft virtual memory limit: 0x%lx MB\n",old_limit.rlim_cur/1024/1024);
    printf("the process's old hard virtual memory limit: 0x%lx MB\n",old_limit.rlim_max/1024/1024);

    // 设置当前进程的虚拟内存限制为MEM_LIM
    new_limit.rlim_cur = MEM_LIM/2;
    new_limit.rlim_max = MEM_LIM;
    setrlimit(RLIMIT_AS,&new_limit);
    getrlimit(RLIMIT_AS,&new_limit);
    printf("the process's new soft virtual memory limit: 0x%lx kB = 0x%lx MB\n",new_limit.rlim_cur/1024,new_limit.rlim_cur/1024/1024);
    printf("the process's new hard virtual memory limit: 0x%lx kB = 0x%lx MB\n",new_limit.rlim_max/1024,new_limit.rlim_max/1024/1024);
    printf("\n");

    // 到软限制，跳出循环
    while(1){
        unsigned long virt_mem_sz = get_memory_by_pid(getpid());
        printf("now mem used : 0x%lx kB\n",virt_mem_sz);
        char *str = malloc(1024*16*16);
        if(errno == ENOMEM){
            perror("Out of memory");
            break;
        }
    }
    printf("\n");
    
    // 提高软限制的值为硬限制
    printf("update soft limit to hard limit\n");
    errno = 0;
    new_limit.rlim_cur = MEM_LIM;
    new_limit.rlim_max = MEM_LIM;
    setrlimit(RLIMIT_AS,&new_limit);
    getrlimit(RLIMIT_AS,&new_limit);
    printf("the process's new soft virtual memory limit: 0x%lx kB = 0x%lx MB\n",new_limit.rlim_cur/1024,new_limit.rlim_cur/1024/1024);
    printf("the process's new hard virtual memory limit: 0x%lx kB = 0x%lx MB\n",new_limit.rlim_max/1024,new_limit.rlim_max/1024/1024);
    printf("\n");

    // 到硬限制，跳出循环
    while(1){
        unsigned long virt_mem_sz = get_memory_by_pid(getpid());
        printf("now mem used : 0x%lx kB\n",virt_mem_sz);
        char *str = malloc(1024*16*16);
        if(errno == ENOMEM){
            perror("Out of memory");
            break;
        }
    }

    return 0;
}