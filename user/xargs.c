#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]){
    
    if (argc < 2) {
        fprintf(2, "usage: xargs command\n");//检查参数数量是否正确
        exit(1);//异常退出
    }

    // 存放子进程 exec 的参数
    char * argvs[MAXARG];//使用kernel/param.h中定义的MAXARG来声明argv的长度

    for (int i = 1; i < argc; i++)  // 略去xargs，第一个参数必须是该命令本身
        argvs[i - 1] = argv[i];
    
    char buf[64]; // 存放从标准输入转化而来的参数
    char buf_char = 0;// 存放缓存区读入的字符

    while (1)
    {
        int buf_cnt = 0;    // buf尾指针
        int arg_begin = 0;  // 当前这个参数在buf中开始的位置
        int index = argc - 1;
        while (1)   // 读取一行
        {
            if (read(0, &buf_char, 1)== 0) // 如果标准输入中没有数据,跳出所有循环（这里直接用exit实现）
                exit(0);
            if (buf_char == '\n')
            {
                buf[buf_cnt++] = 0; //最后一个参数置为0
                argvs[index++] = &buf[arg_begin]; //将每个参数开始地址存入argvs
                arg_begin = buf_cnt; //下一个参数开始为上一个参数结束位置+1
                break; //读取结束，跳出循环
            } 
            else
            {
                buf[buf_cnt++] = buf_char; //将读入的字符存入数组
            }
        }
        argvs[index] = 0; //exec接收最后一个参数argvs[size-1]必须为0
        if (fork() == 0)
        {
            exec(argv[1], argvs);
        }
        else
        {
            wait(0);
        }
    }
    // 正常退出
    exit(0);
}


