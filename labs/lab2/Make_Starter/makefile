myprog : main.o areaOfRectangle.o areaOfSquare.o areaOfCircle.o area_of_triangle.o
	gcc -o areaProg main.c areaOfRectangle.o areaOfSquare.o areaOfCircle.o area_of_triangle.o -lm

main.o : main.c areaOfRectangle.h areaOfSquare.h areaOfCircle.h area_of_triangle.h
	gcc -c main.c

areaOfRectangle.o : areaOfRectangle.c
	gcc -c areaOfRectangle.c

areaOfSquare.o : areaOfSquare.c areaOfRectangle.h
	gcc -c areaOfSquare.c

areaOfCircle.o : areaOfCircle.c
	gcc -c areaOfCircle.c

area_of_triangle.o : area_of_triangle.c
	gcc -c area_of_triangle.c -lm

clean:
	rm areaProg main.o areaOfRectangle.o areaOfSquare.o areaOfCircle.o area_of_triangle.o
