#compiles the .o files and checks for possible warnings.
Simulation: main.o implementation.o 
	g++ -Wall -g -o Simulation main.o implementation.o

#compiles the simulation
main.o: main.cpp implementation.cpp headerfile.h
	g++ -Wall -g -c main.cpp

#compiles the PrintJob
implementation.o: headerfile.h

clean:
	rm Simulation main.o implementation.o