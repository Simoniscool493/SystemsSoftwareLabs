#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[])
{
	int number = atoi(argv[1]);
	char numerals[50];
	int i = 0;

	while(number>0)
	{
		if(number-1000>=0)
		{
			number-=1000;
			numerals[i] = 'M';
		}
		else if(number-500>=0)
		{
			number-=500;
			numerals[i] = 'D';
		}
		else if(number-100>=0)
		{
			number-=100;
			numerals[i] = 'C';
		}
		else if(number-50>=0)
		{
			number-=50;
			numerals[i] = 'L';
		}
		else if(number-10>=0)
		{
			number-=10;
			numerals[i] = 'X';
		}
		else if(number-5>=0)
		{
			number-=5;
			numerals[i] = 'V';
		}
		else if(number-1>=0)
		{
			number-=1;
			numerals[i] = 'I';
		}

		i++;
	}

	for(int j=0;j<i;j++)
	{	
		printf("%c",numerals[j]);
	}

	printf("\n");
	
}


