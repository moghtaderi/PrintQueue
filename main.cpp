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

int main(int argc, char const *argv[]) {

	int printerCount;
	int printerSpeed;
	int numPrintJobs;
	int maxPages;
	int simulationTime;
	char userSeed;
	int seedValue;
	char userOutput;
	string fileName;
	bool fileIO;
	ofstream outfile;
	streambuf* coutBuffer;

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

	cout << "Would you like to provide the seed value [Y/N]: ";
	cin >> userSeed;

	if (userSeed == 'y' || userSeed == 'Y') {
		cout << "Please enter your desired seed value: ";
		cin >> seedValue;
	}else{
		seedValue = time(NULL);
	}
	
	srand(seedValue);      // seed based on user input or time if no input provided

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


	// Main time loop!
	for (int tick = 1; tick <= simulationTime; tick++) {

		// all printers progress for 1 minutes

		// new print job is scheduled in the queue
   	int remainder = RAND_MAX % maxPages;
   	int x;
   	do{
   		x = rand();
   	}while (x >= RAND_MAX - remainder);
   	int pageCount = 1 + x % maxPages;
		cout << pageCount << endl;

		
		// if there are free printers, assign the highest priority job to it!

	}

	// process the last tick for all the printers

	cout << printerCount << "-" << printerSpeed << "-" << numPrintJobs << "-" << maxPages << "-" << seedValue << endl;

	printJobWaitingQueue queue = printJobWaitingQueue();

	cout.rdbuf(coutBuffer);                 // reset cout buffer
	outfile.close();                        // close the output file buffer

	return 0;
}