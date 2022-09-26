#include "kernel/types.h"
#include "user.h"
   
int main(int argc,char* argv[]) {
    int ptoc[2];//parent to child
    int ctop[2];//child to parent
    pipe(ptoc);
    pipe(ctop);
    char buffer[10];
    if (fork() == 0) {
        //child
        close(ptoc[1]);//关闭parent to child写端
        read(ptoc[0],&buffer,sizeof(buffer));
        close(ptoc[0]);//读取完成，关闭parent to child读端
        printf("%d: received %s\n",getpid(),buffer);
        close(ctop[0]);//关闭child to parent读端
        write(ctop[1],"pong",sizeof("pong"));
        close(ctop[1]);//写入完成，关闭child to parent写端
        exit(0);
    }
    //parent
    close(ptoc[0]);//关闭parent to child读端
    write(ptoc[1],"ping",sizeof("ping"));
    close(ptoc[1]);//读取完成，关闭parent to child写端
    wait(0);
    close(ctop[1]);//关闭child to parent写端
    read(ctop[0],&buffer,sizeof(buffer));
    close(ctop[0]);//写入完成，关闭child to parent读端
    printf("%d: received %s\n",getpid(),buffer);
    exit(0);
}