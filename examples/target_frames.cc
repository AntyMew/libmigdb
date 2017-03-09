#include <stdio.h>
#include <string.h>

int v=2; struct { char *a; int q; } sup;

int FuncLev2(char *str)
{
 v=5;
 return strlen(str);
}

int FuncLev1(char *s, int i)
{
 i+=FuncLev2(s);
 return i;
}

int main(int argc, char *argv[])
{
 FuncLev1("Hi!",2500);
 printf("v=%d\n",v);
 return 0;
}
