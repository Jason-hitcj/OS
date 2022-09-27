#include "kernel/types.h"
#include "user.h"
#include "stddef.h"
int main(int argc,char* argv[]) {
    printf("hello\n");
    if (fork() == 0){
        printf("child\n");
    }
    else{
        wait(NULL);
        printf("parent\n");
    }
    printf("zz\n");
    exit(0);
}