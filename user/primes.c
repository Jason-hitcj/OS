#include "kernel/types.h"
#include "user.h"
#include "stddef.h"

void primes(int pd[])
{
    int front,next,i;//front:管道读入的第一个数；next：之后的数
    int p[2];
    if (read(pd[0],&front,sizeof(int)) && front)//当最后的数都输出，只剩0时停止
    {
        printf("prime %d\n",front);//第一个数一定为质数
        pipe(p);
        if (fork() == 0)
        {
            //筛选质数，当到0时停止
            while (read(pd[0], &next, sizeof(int))&& next){
            if (next % front != 0)
                write(p[1], &next, sizeof(int));  
            }
            i = 0;
            //将0写入管道末尾，方便判断管道读取完成
            write(p[1], &i, sizeof(int));
            close(p[1]);
        }
        else {
            //等待子线程写入完成
            wait(NULL);
            //先关闭父管道，再将新管道复制给父管道，完成每一次更新
            close(pd[0]);
            close(pd[1]);
            pd[0] = p[0];
            pd[1] = p[1];
            //递归调用primes，直到筛选完所有数
            primes(pd);
        }
    }
    //递归完成，关闭所有管道
    close(pd[0]);
    close(pd[1]);
    close(p[0]);
    close(p[1]);
}

int main(int argc,char* argv[]) {
    int fp[2],i;
    pipe(fp);
    if (fork() == 0){
        // 将2-35写入管道
        for (i = 2;i < 36; i++){
            write(fp[1],&i,sizeof(int));
        }
        i = 0;
        //将0写入管道末尾，用于判断是否读取完成，方便跳出循环
        write(fp[1], &i, sizeof(int));
        close(fp[1]);
    }
    else{
        wait(NULL);//等待子线程写入完成
        primes(fp);//调用primes筛选质数，传入管道
    }
    //正常退出
    exit(0);
}