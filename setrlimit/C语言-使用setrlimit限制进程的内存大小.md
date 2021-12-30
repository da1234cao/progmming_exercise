[toc]

## 摘要

搬运《unix高级环境编程》7.11 函数getrlimit和setrlimit。

使用示例程序，演示`setlimit`限制进程自身的内存大小。

---

## 函数getrlimit和setrlimit

相关参考：《unix高级环境编程》7.11 、[getrlimit(3p) — Linux manual page](https://man7.org/linux/man-pages/man3/getrlimit.3p.html) 、[setrlimit(3p) — Linux manual page](https://man7.org/linux/man-pages/man3/setrlimit.3p.html) 、[GETRLIMIT - Linux手册页](https://www.igiftidea.com/article/11090496896.html)

每个进程都有一组资源限制，其中一些可以用`getrlimit`和`setrlimit`函数查询和更改。

```c
#include <sys/resource.h>
int getrlimit(int resource, struct rlimit *rlptr);
int setrlimit(int resource, const struct rlimit *rlptr);
```

每个资源都有一个关联的软限制和硬限制，如rlimit结构所定义：

```c
struct rlimit {
    rlim_t rlim_cur;  /* Soft limit */
    rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
};
```

**软限制是内核为相应资源强制执行的值。硬限制是软限制的上限：无特权的进程只能将其软限制设置为介于0到硬限制之间的值，并(不可逆地)降低其硬限制**。特权进程(在Linux下：在初始用户名称空间中具有CAP_SYS_RESOURCE功能的进程)可以对两个限制值进行任意更改。

值RLIM_INFINITY表示对资源没有限制(在getrlimit()返回的结构和传递给setrlimit()的结构中)。

resource参数必须是以下之一：

**RLIMIT_AS**
这是进程的虚拟内存(地址空间)的最大大小。该限制以字节为单位指定，并向下舍入为系统页面大小。此限制影响对brk(2)，mmap(2)和mremap(2)的调用，这些调用在超出此限制时会失败，并显示错误ENOMEM。另外，自动堆栈扩展失败(如果没有通过sigaltstack(2)提供备用堆栈，则会生成SIGSEGV，该进程将终止该进程)。

**RLIMIT_CORE**
这是进程可能转储的核心文件的最大大小(请参阅core(5))。如果为0，则不会创建核心转储文件。当非零时，较大的转储将被截断为此大小。

**RLIMIT_CPU**
这是对进程可以消耗的CPU时间量的限制(以秒为单位)。当进程达到软限制时，将发送一个SIGXCPU信号。此信号的默认操作是终止进程。但是，可以捕获信号，并且处理程序可以将控制权返回给主程序。如果该进程继续消耗CPU时间，它将每秒发送一次SI​​GXCPU，直到达到硬限制为止，然后发送SIGKILL。 (后一点描述了Linux的行为。实现方式在处理如何达到达到软限制后仍继续消耗CPU时间的进程方面有所不同。需要捕获此信号的可移植应用程序应在首次收到SIGXCPU时执行有序终止。)

**RLIMIT_DATA**
这是进程的数据段(已初始化数据，未初始化数据和堆)的最大大小。该限制以字节为单位指定，并向下舍入为系统页面大小。此限制影响对brk(2)，sbrk(2)和(自Linux 4.7起)mmap(2)的调用，这些调用在遇到此资源的软限制时失败，并显示错误ENOMEM。

**RLIMIT_FSIZE**
这是该进程可能创建的文件的最大大小(以字节为单位)。尝试将文件扩展到此限制之外会导致SIGXFSZ信号的传递。默认情况下，该信号终止进程，但是进程可以捕获该信号，在这种情况下，相关的系统调用(例如write(2)，truncate(2))失败，并显示错误EFBIG。

**RLIMIT_MEMLOCK**
这是可以锁定到RAM中的最大内存字节数。该限制实际上四舍五入为系统页面大小的最接近倍数。此限制影响mlock(2)，mlockall(2)和mmap(2)MAP_LOCKED操作。从Linux 2.6.9开始，它还会影响shmctl(2)SHM_LOCK操作，在此操作上设置共享内存段中的总字节数的最大值(请参见shmget(2))，该最大字节数可能会被调用进程的实际用户ID锁定。 shmctl(2)SHM_LOCK锁与mlock(2)，mlockall(2)和mmap(2)MAP_LOCKED建立的每个进程的内存锁分开解决；进程可以在这两个类别的每个类别中将字节锁定到此限制。

等等...

---

## 示例程序

完整代码见本目录。

演示过程：限制当前进程的内存使用。逐渐增加内存使用：当触碰到软限制时候，将软限制的值提高的硬限制；当触碰到硬限制时，程序退出。其中，打印整个内存增长过程。

```c
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
```

程序输出如下：

```shell
 ✗ ./mem_limit 
the process's old soft virtual memory limit: 0xfffffffffff MB
the process's old hard virtual memory limit: 0xfffffffffff MB
the process's new soft virtual memory limit: 0x1000 kB = 0x4 MB
the process's new hard virtual memory limit: 0x2000 kB = 0x8 MB

now mem used : 0x9c4 kB
now mem used : 0xac8 kB
now mem used : 0xbcc kB
now mem used : 0xcd0 kB
now mem used : 0xdd4 kB
now mem used : 0xed8 kB
now mem used : 0xfdc kB
Out of memory: Cannot allocate memory

update soft limit to hard limit
the process's new soft virtual memory limit: 0x2000 kB = 0x8 MB
the process's new hard virtual memory limit: 0x2000 kB = 0x8 MB

now mem used : 0xfdc kB
now mem used : 0x10e0 kB
now mem used : 0x11e4 kB
now mem used : 0x12e8 kB
now mem used : 0x13ec kB
now mem used : 0x14f0 kB
now mem used : 0x15f4 kB
now mem used : 0x16f8 kB
now mem used : 0x17fc kB
now mem used : 0x1900 kB
now mem used : 0x1a04 kB
now mem used : 0x1b08 kB
now mem used : 0x1c0c kB
now mem used : 0x1d10 kB
now mem used : 0x1e14 kB
now mem used : 0x1f18 kB
Out of memory: Cannot allocate memory
```

---

## 其他

1. Linux ulimit命令用于控制shell程序的资源。

    ```shell
    ➜  ~ ulimit -a
    -t: cpu time (seconds)              unlimited
    -f: file size (blocks)              unlimited
    -d: data seg size (kbytes)          unlimited
    -s: stack size (kbytes)             8192
    -c: core file size (blocks)         0
    -m: resident set size (kbytes)      unlimited
    -u: processes                       47229
    -n: file descriptors                1024
    -l: locked-in-memory size (kbytes)  65536
    -v: address space (kbytes)          unlimited
    -x: file locks                      unlimited
    -i: pending signals                 47229
    -q: bytes in POSIX msg queues       819200
    -e: max nice                        0
    -r: max rt priority                 0
    -N 15:                              unlimited
    ```

    ulimit的源码并不在[coreutils](https://github.com/coreutils/coreutils/tree/master/src)中。ulimit是一个内置命令。它的源码在[ulimit.def](https://github.com/bminor/bash/blob/master/builtins/ulimit.def)。

    ```shell
    ✗ type ulimit 
    ulimit is a shell builtin
    ```
2. [RLIMIT_DATA和 RLIMIT_AS的选择](https://github.com/dojiong/Lo-runner/issues/10)

3. 获取当前进程虚拟内存的代码实现。

    ```c
    #ifndef GET_MEM_H
    #define GET_MEM_H

    // 代码来源：https://blog.csdn.net/whatday/article/details/104147981

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <assert.h>
    #include <string.h>
    
    int get_memory_by_pid(__pid_t p){
        FILE *fd;
        char name[32], line_buff[256] = {0}, file[64] = {0};
        int i, vmrss = 0;
    
        sprintf(file, "/proc/%d/status", p);
        fd = fopen(file, "r");
        if(fd==NULL){
            fprintf(stderr,"open %s fail\n",file);
            exit(1);
        }
    
        // 读取VmSize这一行的数据
        for (i = 0; i < 40; i++){
            if (fgets(line_buff, sizeof(line_buff), fd) == NULL){
                break;
            }
            if (strstr(line_buff, "VmSize") != NULL){
                sscanf(line_buff, "%s %d", name, &vmrss);
                break;
            }
        }
    
        fclose(fd);
        return vmrss;
    }

    /**
     *      "VmPeak"------------------------------------虚拟内存使用量的峰值，取mm->total_vm和mm->hiwater_vm的大值。
            "VmSize"------------------------------------当前虚拟内存的实际使用量。
            "VmLck"-------------------------------------PG_mlocked属性的页面总量，常被mlock()置位。
            "VmPin"-------------------------------------不可被移动的Pined Memory内存大小。
            "VmHWM"-------------------------------------HWM是High Water Mark的意思，表示rss的峰值。
            "VmRSS"-------------------------------------应用程序实际占用的物理内存大小，这里和VmSize有区别。VmRss要小于等于VmSize。
            "RssAnon"-----------------------------------匿名RSS内存大小。
            "RssFile"-----------------------------------文件RSS内存大小。
            "RssShmem"----------------------------------共享内存RSS内存大小。
            "VmData"------------------------------------程序数据段的所占虚拟内存大小，存放了初始化了的数据。
            "VmStk"-------------------------------------进程在用户态的栈大小。
            "VmExe"-------------------------------------进程主程序代码段内存使用量，即text段大小。
            "VmLib"-------------------------------------进程共享库内存使用量。
            "VmPTE"-------------------------------------进程页表项Page Table Entries内存使用量。
            "VmPMD"-------------------------------------进程PMD内存使用量。
            "VmSwap",-----------------------------------进程swap使用量。
    */

    #endif
    ```

