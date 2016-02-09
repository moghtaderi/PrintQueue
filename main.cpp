#include "headerfile.h"
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

int randomFromRangeWithSeed(int min, int max){
	int n = max - min + 1;
   int remainder = RAND_MAX % n;
   int x;
   do{
   	x = rand();
   }while (x >= RAND_MAX - remainder);
   return min + x % n;
}

void UserInput(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, int &simulationTime);
void getSeedRef(char &userSeed, int &seedValue);
void seperateOutput(char userOutput,string fileName,ofstream &outfile, streambuf*& coutBuffer);


int main(int argc, char const *argv[]) {

	//variable declarations
	int printerCount;
	int printerSpeed;
	int numPrintJobs;
	int maxPages;
	int simulationTime;
	char userSeed;
	int seedValue;
	char userOutput = 'Y';
	string fileName;
	ofstream outfile;
	streambuf* coutBuffer;

	UserInput(printerCount, printerSpeed, numPrintJobs, maxPages, simulationTime);
	getSeedRef(userSeed, seedValue);	
	
	srand(seedValue);      // seed based on user input or time if no input provided

	seperateOutput(userOutput, fileName, outfile, coutBuffer);


	printScheduler scheduler = printScheduler();
	printerList printers = printerList(printerCount);
	printers.setPrintingSpeed(printerSpeed);

	// Main time loop!
	int tick, temp, pageCount, jobID = 1;
	int remainder = RAND_MAX % maxPages;

	for (tick = 1; tick <= simulationTime; tick++) {

		// all printers progress for 1 minutes
		//printers.progressOneMinute();

		// new print job is scheduled in the queue
	   	do{
	   		temp = rand();
	   	}while(temp >= RAND_MAX - remainder);

	   	pageCount = 1 + temp % maxPages;

		printJob *newJob = new printJob(pageCount, jobID++);

//		cout << endl << "tick " << tick << ": NEW " << pageCount << " PAGE JOB QUEUED!" << endl;

		scheduler.scheduleNewPrintJob(newJob, cout);

		// if there are free printers, assign the highest priority job to it!
		int freePrinterCount = printers.getFreePrinterCount();

//		cout << "      THERE ARE " << freePrinterCount << " FREE PRINTERS ****  "; 
//		printers.listFreePrinters(cout);

		scheduler.processJobs(freePrinterCount, printers, cout);

		// process the last tick for all the printers
		printers.progressOneMinute();

	}


	// Collect data

	cout << printerCount << "-" << printerSpeed << "-" << numPrintJobs << "-" << maxPages << "-" << seedValue << endl;

	cout.rdbuf(coutBuffer);                 // reset cout buffer
	outfile.close();                        // close the output file buffer

	return 0;
}
/* 
	UserInput
	Pre-Condition: the variables must be declared in the callers scope.

	post-Condition: all of the variables will be set based on the user input.
*/
void UserInput(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, int &simulationTime){

	cout << endl << "+++ Welcome to the printer simulation! +++" << endl << endl;

	cout << "Total number of available printers ----- ";
	cin >> printerCount;
	if (printerCount < 1 || printerCount > 3){
		cout << "   ERROR: Invalid input! Parameter defaulted to 1" << endl;
		printerCount = 1;
	}
	cout << "Printing speed in pages per minute ----- ";
	cin >> printerSpeed;
	cout << "Total number of simulated print jobs --- ";
	cin >> numPrintJobs;
	cout << "Maximum page count per print job ------- ";
	cin >> maxPages;
	cout << "Total simulation time in minutes ------- ";
	cin >> simulationTime;


}

/*
	getSeedRef
	Pre-Condition: the seed variable is declared in the callers scope.

	post-Condition: sets the seed variable based on the user input.
*/
void getSeedRef(char &userSeed, int &seedValue){
	cout << "Would you like to provide the seed value [Y/N]: ";
	cin >> userSeed;

	if (userSeed == 'y' || userSeed == 'Y') {
		cout << "Please enter your desired seed value: ";
		cin >> seedValue;
	}else{
		seedValue = time(NULL);
	}
}

/*
	separateOutput
	Pre-Condition: the variables are decrared in the callers scope

	Post-Condition: outputs the information to either the screen or a file specified by the user
*/
void seperateOutput(char userOutput, string fileName, ofstream &outfile, streambuf*& coutBuffer){
	
	bool fileIO;

	cout << "Would you like the output on the screen  [Y/N]: ";
	cin >> userOutput;
	if (userOutput == 'y' || userOutput == 'Y') {
		fileIO = false;
		// NOTE: no change to std:cout and no output file created
	}else{
		cout << "Please enter the name of your output file: ";
		cin >> fileName;                    // get output file name

    	outfile.open(fileName.c_str());     // open file for write
   	
	   	if(outfile.fail()) {
	      	cerr << "ERROR: could not create file <" << fileName << ">" << endl;
	      	exit(1);
	   	}

	   	fileIO = true;
	   	coutBuffer = cout.rdbuf();          // keep a backup of cout buffer
	   	cout.rdbuf(outfile.rdbuf());        // redirect cout to output file
	   	// NOTE: FOR THE REST OF THE PROGRAM STD:COUT WILL BE REDIRECTED TO OUTFILE
	}
}




