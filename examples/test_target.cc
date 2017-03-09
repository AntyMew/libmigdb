#include <stdio.h>

int v=3;

void PrintHi(int val)
{
 int x=val*3;

 printf("Hi! x=%d\n",x); v++;
}

int main(int argc, char *argv[])
{
 printf("arguments: %d\n",argc);
 for (int i=0; i<argc; i++)
     printf("%d: %s\n",i,argv[i]);
 printf("*stopped,reason=\"You fool! that isn't gdb ;-)\"\n"); v=5;
 printf("Press ENTER\n");
 getchar();
 PrintHi(argc);
 return 0;
}
