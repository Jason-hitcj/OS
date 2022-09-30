#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *filename;

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  //排除末尾/0影响
  memmove(buf, p, strlen(p) + 1);
  return buf;
}

//仿照ls访问文件目录方式，递归寻找符合文件
void
find(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    //将文件名与路径名匹配，若相同则输出
    if (strcmp(fmtname(path), filename) == 0) {
                printf("%s\n", path);
            }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      //不要递归进入.和..
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      //使用递归允许find进入到子目录
      find(buf);
    }
    break;
  }
  close(fd);
}

int main(int argc,char* argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find path filename\n");
        exit(1);
    }
    filename = argv[2];
    find(argv[1]);
    exit(0);
}