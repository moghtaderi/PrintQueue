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

void UserInput(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, int &simulationTime, char &userPrintSpeed, int *printerArray);
void getSeedRef(char &userSeed, int &seedValue);
void setupOutput(char userOutput,string fileName, ofstream &outStream, streambuf*& coutBuffer);
void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs, int maxPages, int seedValue, char userPrintSpeed, int *printerArray);
void outputSimulationSummary(printerList* printers, int high, int med, int low, printScheduler* scheduler, int totalPagesPrinted);
void getPrintSpeed(char& userPrintSpeed, int& printerSpeed, int& printerCount, int *printerArray);

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
	int hwt = 0;
	int mwt = 0;
	int lwt = 0;
	int totalPagesPrinted = 0;

	char userPrintSpeed;
	int *printerArray = NULL;

	UserInput(printerCount, printerSpeed, numPrintJobs, maxPages, simulationTime, userPrintSpeed, printerArray);

	getSeedRef(userSeed, seedValue);

	srand(seedValue);      // seed based on user input or time if no input provided

	// Decide and setup the desired user output format (screen/file)
	setupOutput(userOutput, fileName, outfile, coutBuffer);

	outputSimulationSettings(printerCount, printerSpeed, numPrintJobs, maxPages, seedValue, userPrintSpeed, printerArray);

	// Setup Scheduler and Printers
	printScheduler scheduler = printScheduler();
	printerList printers = printerList(printerCount);
	printers.setPrintingSpeed(printerSpeed);

	int tick, temp, pageCount, jobID = 1;
	int remainder = RAND_MAX % maxPages;

	for (tick = 1; tick <= simulationTime; tick++) {

		// new print job is scheduled in the queue
   	do{
   		temp = rand();
   	}while(temp >= RAND_MAX - remainder);

   	pageCount = 1 + temp % maxPages;

		printJob *newJob = new printJob(pageCount, jobID++);

		cout << endl << "Minute " << tick << ":" << endl;

		scheduler.scheduleNewPrintJob(newJob, cout);

		// if there are free printers, assign the highest priority job to it!
		int freePrinterCount = printers.getFreePrinterCount();

		if (freePrinterCount > 0) {
			cout << "    There are " << freePrinterCount << " available printers: ";
			printers.listFreePrinters(cout);
			cout << endl;
		}else{
			cout << "    All printers are currently busy!" << endl;
		}

		// process as many jobs as there are free printers
		scheduler.processJobs(freePrinterCount, printers, cout);

		// let all the printers progress for one minute
		printers.progressOneMinute(cout, totalPagesPrinted);

		scheduler.calculateWaitingTimes(hwt, mwt, lwt);
	}

	outputSimulationSummary(&printers, hwt, mwt, lwt, &scheduler, totalPagesPrinted);

	cout << endl;
	cout.rdbuf(coutBuffer);                 // reset cout buffer
	cout << endl;
	outfile.close();                        // close the output file buffer

	return 0;
}
/*
	UserInput
	Pre-Condition: the variables must be declared in the callers scope.
	post-Condition: all of the variables will be set based on the user input.
*/
void UserInput(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, int &simulationTime, char &userPrintSpeed, int *printerArray){

	cout << endl << "••••••••• Welcome to the printer simulation! •••••••••" << endl << endl;

	cout << "Total number of available printers ----- ";
	cin >> printerCount;

	printerArray = new int[printerCount];
	getPrintSpeed(userPrintSpeed, printerSpeed, printerCount, printerArray);
	
	cout << "Total number of simulated print jobs --- ";
	cin >> numPrintJobs;
	cout << "Maximum page count per print job ------- ";
	cin >> maxPages;
	cout << "Total simulation time in minutes ------- ";
	cin >> simulationTime;

}

/*
	getPrintSpeed
	Pre-Condition: the printer speed(s) are declared in the callers scope
	Post-Condition: sets the printer speed(s) based on the user input
*/
void getPrintSpeed(char& userPrintSpeed, int& printerSpeed, int& printerCount, int *printerArray){

	cout << "Do the printers print at the same speed [Y/N]: ";
	cin >> userPrintSpeed;

	if (userPrintSpeed == 'y' || userPrintSpeed == 'Y') {
		cout << "Printing speed in pages per minute ----- ";
		cin >> printerSpeed;
	}else{
		cout << "Please enter the print speed for each of the printers: " << endl;
		for (int i=0; i<printerCount; i++) {
			cout << "Printer " << i+1 << ": ";
			cin >> printerArray[i];
		}
	}
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
void setupOutput(char userOutput, string fileName, ofstream &outStream, streambuf*& coutBuffer){

	bool fileIO;

	cout << "Would you like the output on the screen  [Y/N]: ";
	cin >> userOutput;
	if (userOutput == 'y' || userOutput == 'Y') {
		fileIO = false;
		// NOTE: no change to std:cout and no output file created
	}else{
		cout << "Please enter the name of your output file: ";
		cin >> fileName;                    // get output file name

    	outStream.open(fileName.c_str());     // open file for write

	   	if(outStream.fail()) {
	      	cerr << "ERROR: could not create file <" << fileName << ">" << endl;
	      	exit(1);
	   	}

	   	fileIO = true;
	   	coutBuffer = cout.rdbuf();          // keep a backup of cout buffer
	   	cout.rdbuf(outStream.rdbuf());      // redirect cout to output file
	   	// NOTE: FOR THE REST OF THE PROGRAM STD:COUT WILL BE REDIRECTED TO OUTFILE
	}
}

void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs, int maxPages, int seedValue, char userPrintSpeed, int *printerArray){

	cout << "\n••••••••••••••••• BEGIN SIMULATION •••••••••••••••••\n";
	cout << "   Printers Available: " << printerCount << "\n";
	if (userPrintSpeed == 'y' || userPrintSpeed == 'Y') {
		cout << "   Printing Speed: " << printerSpeed << "\n";
	}else{
		for (int i=0; i<printerCount; i++)
			cout << "	Printing Speed for Printer " << i+1 << ": " << printerArray[i] << "\n"; 
	}
	cout << "   Number of print jobs: " << numPrintJobs << "\n";
	cout << "   Maximum possible page count: " << maxPages << "\n";
	cout << "   Seed Value: " << seedValue << "\n";
}

void outputSimulationSummary(printerList* printers, int high, int med, int low, printScheduler* scheduler, int totalPagesPrinted){
	cout << "\n••••••••••••••••• SIMULATION SUMMARY •••••••••••••••••\n\n";
	printers->completionReport(cout);
	cout << "\n";

	int freePrinterCount = printers->getFreePrinterCount();
	if (freePrinterCount != 0) {
		cout << "    Printer(s) ";
		printers->listFreePrinters(cout);
		cout << "were free at the end of simulation!\n";
	} else {
		cout << "    All printers were busy at the end of simulation!\n";
	}

	cout << "    Total pages printed: " << totalPagesPrinted << " pages\n";
	cout << "    Total waiting time in the queue: " << high+med+low << " minutes\n";
	cout << "        in HIGH PRIOROTY queue: " << high << " minutes\n";
	cout << "        in MEDIUM PRIOROTY queue: " << med << " minutes\n";
	cout << "        in LOW PRIOROTY queue: " << low << " minutes\n";
	cout << "    Jobs still waiting in the queue: " << scheduler->getLeftoverJobCount() << "\n";

	cout << "\n••••••••••••••••••••••••••••••••••••••••••••••••••••••\n";
}
