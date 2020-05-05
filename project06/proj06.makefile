proj06: proj06.student.o
	g++ -pthread proj06.student.o -o proj06

proj06.student.o: proj06.student.c
	g++ -Wall -c proj06.student.c
