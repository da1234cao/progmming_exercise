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