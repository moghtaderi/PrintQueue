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

void getSimulationParameters(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, char &userPrintSpeed, int *printerSpeedArray, streambuf* coutBuffer, char &userSeed, int &seedValue, bool userInputFromFile, bool userOutputToFile);
void getSeedRef(char &userSeed, int &seedValue, bool userInputFromFile);
void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs, int maxPages, int seedValue, char userPrintSpeed, int *printerSpeedArray);
void outputSimulationSummary(printerList* printers, int high, int med, int low, printScheduler* scheduler, int totalPagesPrinted, int tick);
void getPrintSpeed(char& userPrintSpeed, int& printerSpeed, int& printerCount, int *printerSpeedArray, bool userInputFromFile);
void setupIO(char& userInput, char& userOutput, ifstream &inStream, ofstream& outStream, streambuf*& cinBuffer, streambuf*& coutBuffer, bool& userInputFromFile, bool& userOutputToFile);


int main(int argc, char const *argv[]) {

	//variable declarations
	int printerCount = 3;
	int printerSpeed = 10;
	int numPrintJobs = 100;
	int numFinishedJobs = 0;
	int maxPages = 50;
	char userSeed;
	int seedValue;
	char userInput = 'Y';
	char userOutput = 'Y';
	string fileName;
	ofstream outfile;
	ifstream infile;
	streambuf* coutBuffer;
	streambuf* cinBuffer;
	int hwt = 0;
	int mwt = 0;
	int lwt = 0;
	int totalPagesPrinted = 0;
	bool userInputFromFile = false;
	bool userOutputToFile = false;

	char userPrintSpeed;
	int* printerSpeedArray = NULL;

	setupIO(userInput, userOutput, infile, outfile, cinBuffer, coutBuffer, userInputFromFile, userOutputToFile);

	getSimulationParameters(printerCount, printerSpeed, numPrintJobs, maxPages, userPrintSpeed, printerSpeedArray, coutBuffer, userSeed, seedValue, userInputFromFile, userOutputToFile);

	srand(seedValue);      // seed based on user input or time if no input provided

	outputSimulationSettings(printerCount, printerSpeed, numPrintJobs, maxPages, seedValue, userPrintSpeed, printerSpeedArray);

	// Setup Scheduler and Printers
	printScheduler scheduler = printScheduler();
	printerList printers = printerList(printerCount);
	printers.setPrintingSpeed(printerSpeed);

	int tick = 1, temp, pageCount, jobID = 1, numCreatedJobs = 0;
	int remainder = RAND_MAX % maxPages;

	while (numFinishedJobs != numPrintJobs){

		// new print job is scheduled in the queue
   	do{
   		temp = rand();
   	}while(temp >= RAND_MAX - remainder);

   	pageCount = 1 + temp % maxPages;

		printJob *newJob = new printJob(pageCount, jobID++);

		cout << endl << "Minute " << tick << ":" << endl;

		if(numCreatedJobs != numPrintJobs){
			scheduler.scheduleNewPrintJob(newJob, cout);
			numCreatedJobs++;
		}

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

		numFinishedJobs = printers.getCompletedJobsCount();
		tick++;
	}

	outputSimulationSummary(&printers, hwt, mwt, lwt, &scheduler, totalPagesPrinted, tick);

	cout << endl;
	cout.rdbuf(coutBuffer);                 // reset cout buffer
	outfile.close();                        // close the output file buffer

	return 0;
}
/*
	getSimulationParameters
	Pre-Condition: the variables must be declared in the callers scope.
	post-Condition: all of the variables will be set based on the user input.
*/
void getSimulationParameters(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages, char &userPrintSpeed, int* printerSpeedArray, streambuf* coutBuffer, char &userSeed, int &seedValue, bool userInputFromFile, bool userOutputToFile){

	char userDefaults;
	streambuf* currentBuffer = cout.rdbuf();

	if (userOutputToFile)
		cout.rdbuf(coutBuffer);                 // reset cout buffer

	if (!userInputFromFile) {
		cout << "Number of printers: " << printerCount << endl;
		cout << "Number of simulated print jobs: " << numPrintJobs << endl;
		cout << "Maximum page count per job: " << maxPages << endl;
		cout << "Printing speed in pages per minute: " << printerSpeed << endl;
		cout << endl << "Would you like to use the above default values [Y/N]: ";
	}
	cin >> userDefaults;

	if (userDefaults == 'n' || userDefaults == 'N') {
		if (!userInputFromFile)
			cout << endl << "Total number of available printers ----- ";
		cin >> printerCount;

		if (!userInputFromFile)
			cout << "Total number of simulated print jobs --- ";
		cin >> numPrintJobs;

		if (!userInputFromFile)
			cout << "Maximum page count per print job ------- ";
		cin >> maxPages;

		cout << endl;
	}

	getPrintSpeed(userPrintSpeed, printerSpeed, printerCount, printerSpeedArray, userInputFromFile);
	getSeedRef(userSeed, seedValue, userInputFromFile);

	cout.rdbuf(currentBuffer);
}

/*
	getPrintSpeed
	Pre-Condition: the printer speed(s) are declared in the callers scope
	Post-Condition: sets the printer speed(s) based on the user input
*/
void getPrintSpeed(char& userPrintSpeed, int& printerSpeed, int& printerCount, int *printerSpeedArray, bool userInputFromFile){

	printerSpeedArray = new int[printerCount];
	if (!userInputFromFile)
		cout << "Do all the printers have a speed of 10 pages per minute [Y/N]: ";
	cin >> userPrintSpeed;

	if (userPrintSpeed == 'n' || userPrintSpeed == 'N') {
		if (!userInputFromFile)
			cout << endl << "Please enter the print speed for each printer: " << endl;
		for (int i=0; i < printerCount; i++) {
			if (!userInputFromFile)
				cout << "   Printer " << i+1 << ": ";
			cin >> printerSpeedArray[i];
		}
		cout << endl;
	} else {
		for (int i = 0; i < printerCount; i++) {
			printerSpeedArray[i] = printerSpeed;
		}
	}
}

/*
	getSeedRef
	Pre-Condition: the seed variable is declared in the callers scope.
	post-Condition: sets the seed variable based on the user input.
*/
void getSeedRef(char &userSeed, int &seedValue, bool userInputFromFile){

	if (!userInputFromFile)
		cout << "Would you like to use the dault seed value [Y/N]: ";
	cin >> userSeed;

	if (userSeed == 'n' || userSeed == 'N') {
		if (!userInputFromFile)
			cout << endl << "Please enter your desired seed value: ";
		cin >> seedValue;
		cout << endl;
	}else{
		seedValue = time(NULL);
	}
}

void setupIO(char& userInput, char& userOutput, ifstream &inStream, ofstream& outStream, streambuf*& cinBuffer, streambuf*& coutBuffer, bool& userInputFromFile, bool& userOutputToFile) {

	string fileName;

	cout << endl << "••••••••• Welcome to the printer simulation! •••••••••" << endl << endl;

	cout << "Would you like to use input from the keyboard  [Y/N]: ";
	cin >> userInput;

	if (userInput == 'y' || userInput == 'Y') {
		userInputFromFile = false;
		// NOTE: no change to std:cin and no input file created
	}else{
		userInputFromFile = true;

		cout << endl << "Please enter the name of your input file: ";
		cin >> fileName;                    // get input file name
		cout << endl;

    	inStream.open(fileName.c_str());     // open file for write

		if(inStream.fail()) {
			cerr << "ERROR: could not open file <" << fileName << "> with read access." << endl;
			cerr << "       Please make sure a file with that name exists in this directory." << endl << endl;
			exit(1);
		}
		// NOTE: AFTER THIS FUNCTION, FOR THE REST OF THE PROGRAM STD:CIN WILL BE READING FROM TO INFILE
	}

	cout << "Would you like the output on the screen  [Y/N]: ";
	cin >> userOutput;

	if (userOutput == 'y' || userOutput == 'Y') {
		userOutputToFile = false;
		// NOTE: no change to std:cout and no output file created
	}else{
		userOutputToFile = true;

		cout << endl << "Please enter the name of your output file: ";
		cin >> fileName;                    // get output file name
		cout << endl;

		outStream.open(fileName.c_str());     // open file for write

		if(outStream.fail()) {
			cerr << "ERROR: could not create file <" << fileName << "> with write access." << endl;
			exit(1);
		}
		// NOTE: AFTER THIS FUNCTION, FOR THE REST OF THE PROGRAM STD:COUT WILL BE REDIRECTED TO OUTFILE
	}

	if (userInputFromFile) {
		cinBuffer = cin.rdbuf();          // keep a backup of cin buffer
		cin.rdbuf(inStream.rdbuf());      // redirect cin to input file
	}

	if (userOutputToFile) {
		coutBuffer = cout.rdbuf();          // keep a backup of cout buffer
		cout.rdbuf(outStream.rdbuf());      // redirect cout to output file
	}
}

void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs, int maxPages, int seedValue, char userPrintSpeed, int *printerSpeedArray){

	cout << "\n••••••••••••••••• BEGIN SIMULATION •••••••••••••••••\n";
	cout << "   Printers Available: " << printerCount << "\n";
	if (userPrintSpeed == 'y' || userPrintSpeed == 'Y') {
		cout << "   Printing Speed: " << printerSpeed << "\n";
	}else{
		for (int i=0; i<printerCount; i++)
			cout << "	Printing Speed for Printer " << i+1 << ": " << printerSpeedArray[i] << "\n";
	}
	cout << "   Number of print jobs: " << numPrintJobs << "\n";
	cout << "   Maximum possible page count: " << maxPages << "\n";
	cout << "   Seed Value: " << seedValue << "\n";
}

void outputSimulationSummary(printerList* printers, int high, int med, int low, printScheduler* scheduler, int totalPagesPrinted, int tick){
	cout << "\n••••••••••••••••• SIMULATION SUMMARY •••••••••••••••••\n\n";
	printers->completionReport(cout);
	cout << "\n";

	cout << "Total simulation time: " << tick << endl;
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
