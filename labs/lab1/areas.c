#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "circle.h"
#include "rectangle.h"
#include "triangle.h"

int main(int argc,char* argv[])
{
	char* shape = argv[1];
	float ans;

	if(strcmp("Circle",shape) == 0)
	{
		ans =  areaOfCircle(atoi(argv[2]));
	}
	else if(strcmp("Rectangle",shape) == 0)
	{
		ans =  areaOfRectangle(atoi(argv[2]),atoi(argv[3]));
	}
	else if(strcmp("Triangle",shape) == 0)
	{
		ans =  areaOfTriangle(atoi(argv[2]),atoi(argv[3]));
	}

	printf("%f\n",ans);
	return ans;
}
