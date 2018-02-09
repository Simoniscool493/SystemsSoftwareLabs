#include <stdio.h>
#include <stdlib.h>

long factorial(int);

int main(int argv,char* args[])
{
   long f1;
   f1 = factorial(atoi(args[1]));
   printf("Fact is %d\n", f1);
}

long factorial(int f) {
   if(f == 0) {
      return 1;
   } else {
      return f*factorial(f-1);
   }
}
