#include "kernel/types.h"
#include "user.h"
#include "stddef.h"

void primes(int pd[])
{
    int front,next,i;
    int p[2];
    if (read(pd[0],&front,sizeof(int)) && front)
    {
        printf("prime %d\n",front);
        pipe(p);
        if (fork() == 0)
        {
            while (read(pd[0], &next, sizeof(int))&& next){
            if (next % front != 0)
                write(p[1], &next, sizeof(int));  
            }
            i = 0;
            write(p[1], &i, sizeof(int));
            close(p[1]);
        }
        else {
            wait(NULL);
            close(pd[0]);
            close(pd[1]);
            pd[0] = p[0];
            pd[1] = p[1];
            primes(pd);
        }
    }
    close(pd[0]);
    close(pd[1]);
    close(p[0]);
    close(p[1]);
}

int main(int argc,char* argv[]) {
    int fp[2],i;
    pipe(fp);
    if (fork() == 0){
        // close(fp[0]);
        for (i = 2;i < 36; i++){
            write(fp[1],&i,sizeof(int));
        }
        i = 0;
        write(fp[1], &i, sizeof(int));
        close(fp[1]);
    }
    else{
        wait(NULL);
        primes(fp);
    }
    exit(0);
}