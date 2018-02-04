#include <stdio.h>

long factorial(int);

int main()
{
   long f1;
   f1 = factorial(5);
   printf("Fact is %d\n", f1);
}

long factorial(int f) {
   if(f == 0) {
      return f;
   } else {
      return f*factorial(f-1);
   }
}