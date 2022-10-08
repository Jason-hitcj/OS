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
    
    struct Mem *tempa;
    tempa = (struct Mem *)malloc(sizeof(struct Mem));
    
    tempa->a = 10;
    // tempa->birthday[0]->year = 4;
    // tempa->birthday[1]->year = 4;
    // tempa->birthday[1]->month = 4;
    // tempa->birthday[1]->day = 4;
    if (tempa->birthday){
        printf("ok\n");
    }
    // printf("%d\n",sizeof(tempa->birthday)/sizeof(tempa->birthday[0]));
    // printf("%d %d \n",tempa->a,tempa->birthday[0]->year); 
    exit(0);
}