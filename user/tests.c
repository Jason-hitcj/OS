// #include "kernel/types.h"
// #include "user.h"
// #include "stddef.h"
// int main(int argc,char* argv[]) {
//     int a[10];
//     int i,j;
//     i=0;
//     j=0;
//     for (i=0;i<5;i++){
//         a[j++]=i;
//         printf("%d %d %d\n",i,j,a[i]);
//     }
//     printf("Done\n");
//     exit(0);
// }


// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/param.h"

// #define MAXN 1024

// int main(int argc, char *argv[])
// {
//     if (argc < 2)
//     {
//         fprintf(2, "usage: xargs command\n");
//         exit(1);
//     }

//     char *_argv[MAXARG];    // 存放子进程exec的参数
//     for (int i = 1; i < argc; i++)  // 略去xargs，第一个参数必须是该命令本身
//         _argv[i - 1] = argv[i];
//     char buf[MAXN]; // 存放从标准输入转化而来的参数
//     char c = 0;
//     int stat = 1;   // 从标准输入read返回的状态

//     while (1) // 标准输入中还有数据
//     {
//         int buf_cnt = 0;    // buf尾指针
//         int arg_begin = 0;  // 当前这个参数在buf中开始的位置
//         int argv_cnt = argc - 1;
//         while (1)   // 读取一行
//         {
//             stat = read(0, &c, 1); //从标准输入中读取
//             if (stat == 0) // 标准输入中没有数据，exit
//                 exit(0);
//             if (c == ' ' || c == '\n')
//             {
//                 buf[buf_cnt++] = 0; //最后一个参数必需为0，否则会执行失败
//                 _argv[argv_cnt++] = &buf[arg_begin];
//                 arg_begin = buf_cnt; 
//                 if (c == '\n')
//                     break;
//             } 
//             else
//             {
//                 buf[buf_cnt++] = c;
//             }
//         }

//         _argv[argv_cnt] = 0; //结束标志
//         if (fork() == 0)
//         {
//             exec(_argv[0], _argv);
//         }
//         else
//         {
//             wait(0);
//         }
//     }
//     exit(0);
// }
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(){
    int child_pid;
    if((child_pid=fork())==0){
    while(1);
        printf("forbidden zone\n");
        exit(0);
    }
    else{
        printf(“received a signal\n”)
        while(getc(stdin)){
            kill(child_pid,SIGKILL);
            printf(“received a signal\n”); 
            wait(0);
            exit(0);
        }
    }
 }