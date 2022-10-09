#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct Date
{
	int year;
	int month;
	int day;
};
struct Mem{
   int a;
   int b;
   struct Date *birthday[3];
};
int
main(int argc, char *argv[])
{
    int mm;
    struct Mem *tempa;
    tempa = (struct Mem *)malloc(sizeof(struct Mem));
    struct Date **day;
    // day = (struct Date *)malloc(sizeof(struct Date));
    day = tempa->birthday;
    // tempa->birthday[0]->year = 4;
    day[0]->year = 0;
    mm = day[0]->month;
    tempa->birthday[1]->year = 4;
    tempa->birthday[1]->month = 4;
    tempa->birthday[1]->day = 4;
    printf("ok  %d,\n",mm);
    // if (tempa->birthday[2]->year){
    //     printf("ok\n");
    // }
    for(int i=0; i<3; i++){
       if(!day[i])
          printf("%d\n",i);
    }
    // printf("%d\n",sizeof(tempa->birthday)/sizeof(tempa->birthday[0]));
    // printf("%d %d \n",tempa->a,tempa->birthday[0]->year); 
    exit(0);
}