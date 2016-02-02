#compiles the .o files and checks for possible warnings.
Simulation: Simulation.o PrintJob.o Printer.o PrinterList.o PrintJobWaitingQueue.o
	g++ -Wall -g -o Simulation Simulation.o PrintJob.o Printer.o PrinterList.o PrintJobWaitingQueue.o

#compiles the simulation
Simulation.o: simulation.cpp PrintJob.h Printer.h PrinterList.h PrintJobWaitingQueue.h
	g++ -Wall -g -c Simulation.cpp

#compiles the PrintJob
PrintJob.o: PrintJob.h

#compiles the Printer
Printer.o: Printer.h

#compiles the PrinterList
PrinterList.o: PrinterList.h

#compiles the PrintJob
PrintJobWaitingQueue.o: PrintJobWaitingQueue.h