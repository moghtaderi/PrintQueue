#include "headerfile.h"

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

int Factorial(int n) {
	return (n == 1 || n == 0) ? 1 : Factorial(n - 1) * n;
}

void calculatePoissonDistribution(vector<double>& distribution, double avgNumPrintJobsPerMinute){

	double total = 0.0;
	int k = 0;
	double newValue = 0.0;

	while (total <= 0.95) {
		newValue = (pow(avgNumPrintJobsPerMinute, k)*exp(-1*avgNumPrintJobsPerMinute))/Factorial(k);
		k++;
		total+=newValue;
		distribution.push_back(total);
	}
}

int numNewJobsToQueue(double random, vector<double> distribution){

	int i = 0;
	for (vector<double>::iterator it = distribution.begin(); it != distribution.end(); it++){
		if(random <= *it){
			return i;
		}
		i++;
	}
	return i;
}

int newJobPriorityLevel(double random, int priorityCount){
	double total = 0.0;
	int i;
	for (i = 1; i < priorityCount; i++) {
		total += 1.0/(double)(2+i);
		if (random <= total) {
			return i-1;
		}
	}
	return i-1;
}


int newJobPageCount(int cutoffIndex, int* priorityQueueCutOffs, int priorityCount){

	int low, high;
	int rand_catch;

	if (cutoffIndex == 0){
			low = 0;
	} else {
			low = priorityQueueCutOffs[cutoffIndex-1]+1;
	}
		high = priorityQueueCutOffs[cutoffIndex];

	rand_catch = rand()%(high-low)+low;
	if(rand_catch == 0){
		return rand_catch+1;
	}else
		return rand_catch;

}

void getSimulationParameters(int &printerCount, int &printerSpeed, int &numPrintJobs,
                             int &maxPages, char &userPrintSpeed, double*& printerSpeedArray,
									  streambuf* coutBuffer, char &userSeed, int &seedValue,
									  bool userInputFromFile, bool userOutputToFile,
									  int& priorityCount, int*& priorityQueueCutOffs,
									  double& avgNumPrintJobsPerMinute, double& costPerPage,
									  char& userPrintCost, double*& printerCostArray,
									  int& maintenanceThreshold);

void getSeedRef(char &userSeed, int &seedValue, bool userInputFromFile);

void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs,
	                           int maxPages, int seedValue, char userPrintSpeed,
								double* printerSpeedArray, int priorityCount,
		                       int* priorityQueueCutOffs, double avgNumPrintJobsPerMinute, double costPerPage, 
		                       double* printerCostArray, int maintenanceThreshold);

void outputSimulationSummary(printerList* printers, int high, int med, int low,
	                        printScheduler* scheduler, int totalPagesPrinted, int tick);

void getPrintSpeed(char& userPrintSpeed, int printerSpeed, int& printerCount,
	                double*& printerSpeedArray, bool userInputFromFile);

void setupIO(char& userInput, char& userOutput, ifstream &inStream, ofstream& outStream,
	          streambuf*& cinBuffer, streambuf*& coutBuffer, bool& userInputFromFile,
				 bool& userOutputToFile);

void getPrintCost(char& userPrintCost, double costPerPage, int printerCount,
	               double*& printerCostArray, bool userInputFromFile);

void getPriorityQueueDetails(int& priorityCount, int*& priorityQueueCutOffs, int maxPages);


int main(int argc, char const *argv[]) {

	//variable declarations
	int printerCount = 3;
	int printerSpeed = 10;
	int numPrintJobs = 100;
	double avgNumPrintJobsPerMinute = 1.5;
	double costPerPage = 0.2;
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
	char userPrintCost;
	double* printerSpeedArray = NULL;
	double* printerCostArray = NULL;
	int tick = 1;
	int jobID = 1;
	int numCreatedJobs = 0;
	int priorityCount;
	int* priorityQueueCutOffs = NULL;
	int maintenanceThreshold = 10;
	vector<double> distribution;

	// choose between file vs. standard input and output paths
	setupIO(userInput, userOutput, infile, outfile, cinBuffer, coutBuffer, userInputFromFile, userOutputToFile);

	// Establish all user inputs for the simulation
	getSimulationParameters(printerCount, printerSpeed, numPrintJobs, maxPages,
		                     userPrintSpeed, printerSpeedArray, coutBuffer, userSeed,
									seedValue, userInputFromFile, userOutputToFile, priorityCount,
									priorityQueueCutOffs, avgNumPrintJobsPerMinute, costPerPage,
									userPrintCost, printerCostArray, maintenanceThreshold);

	// setup distribution vector
	calculatePoissonDistribution(distribution, avgNumPrintJobsPerMinute);

	// Set the randomization seed based on user request
	srand(seedValue);

	// Output the settings for the simulation to confirm
	outputSimulationSettings(printerCount, printerSpeed, numPrintJobs, maxPages,
		                      seedValue, userPrintSpeed, printerSpeedArray, priorityCount,
		                       priorityQueueCutOffs, avgNumPrintJobsPerMinute, costPerPage, 
		                       printerCostArray, maintenanceThreshold);

	// Setup Scheduler and Printers
	printScheduler scheduler = printScheduler(priorityQueueCutOffs, priorityCount);
	printerList printers = printerList(printerCount);
	printers.setPrintingSpeed(printerSpeedArray);
	printers.setPrintingCost(printerCostArray);
	printers.setPrintingMaintenanceThreshold(maintenanceThreshold);

	// Main loop
	while (numFinishedJobs != numPrintJobs){

		cout << endl << "Minute " << tick << ":" << endl;

		//find out how many new jobs to add based on our poisson distribution
		int numNewJobs = numNewJobsToQueue((rand()%RAND_MAX)/(double)RAND_MAX, distribution);

		for (int i = 0; i < numNewJobs; i++) {
			int cutoffIndex = newJobPriorityLevel((rand()%RAND_MAX)/(double)RAND_MAX, priorityCount);
			int pageCount = newJobPageCount(cutoffIndex, priorityQueueCutOffs, priorityCount);

			printJob* newJob = new printJob(pageCount, jobID++);

			if(numCreatedJobs != numPrintJobs){
				scheduler.scheduleNewPrintJob(newJob, cout, cutoffIndex);
				numCreatedJobs++;
			}
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
		scheduler.processJobs(freePrinterCount, printers, cout, priorityCount);

		// let all the printers progress for one minute
		printers.progressOneMinute(cout, totalPagesPrinted, printerSpeedArray);

		// keep track of all the wait times based on the queues
		scheduler.calculateWaitingTimes(hwt, mwt, lwt);

		// keep track of the jobs that are done!
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
void getSimulationParameters(int &printerCount, int &printerSpeed, int &numPrintJobs, int &maxPages,
	                          char &userPrintSpeed, double*& printerSpeedArray, streambuf* coutBuffer,
									  char &userSeed, int &seedValue, bool userInputFromFile,
									  bool userOutputToFile, int& priorityCount, int*& priorityQueueCutOffs,
									  double& avgNumPrintJobsPerMinute, double& costPerPage, char& userPrintCost,
									  double*& printerCostArray, int& maintenanceThreshold){

	char userDefaults;
	streambuf* currentBuffer = cout.rdbuf();

	if (userOutputToFile)
		cout.rdbuf(coutBuffer);                 // reset cout buffer

	if (!userInputFromFile) {
		cout << endl << "Number of printers: " << printerCount << endl;
		cout << "Number of simulated print jobs: " << numPrintJobs << endl;
		cout << "Maximum page count per job: " << maxPages << endl;
		cout << "Average number of print jobs per minute: " << avgNumPrintJobsPerMinute << endl;
		cout << "Printing speed in pages per minute: " << printerSpeed << endl;
		cout << "Cost per printed page (USD): " << costPerPage << endl;
		cout << "Number of printed pages before requiring maintenance: " << maintenanceThreshold << endl;
		cout << endl << "Would you like to use the above default values [Y/N]: ";
	}
	cin >> userDefaults;

	if (userDefaults == 'n' || userDefaults == 'N') {
		if (!userInputFromFile)
			cout << endl << "Total number of available printers ------- ";
		cin >> printerCount;

		if (!userInputFromFile)
			cout << "Total number of simulated print jobs ----- ";
		cin >> numPrintJobs;

		if (!userInputFromFile)
			cout << "Maximum page count per print job --------- ";
		cin >> maxPages;

		if (!userInputFromFile)
			cout << "Average number of print jobs per minute -- ";
		cin >> avgNumPrintJobsPerMinute;

		if (!userInputFromFile)
			cout << "Maintenance break printed page count ----- ";
		cin >> maintenanceThreshold;

		cout << endl;
	}

	getPrintSpeed(userPrintSpeed, printerSpeed, printerCount, printerSpeedArray, userInputFromFile);
	getPrintCost(userPrintCost, costPerPage, printerCount, printerCostArray, userInputFromFile);
	getPriorityQueueDetails(priorityCount, priorityQueueCutOffs, maxPages);
	getSeedRef(userSeed, seedValue, userInputFromFile);

	cout.rdbuf(currentBuffer);
}

void getPrintCost(char& userPrintCost, double costPerPage, int printerCount, double*& printerCostArray,
	               bool userInputFromFile) {

	printerCostArray = new double[printerCount];

	if (!userInputFromFile)
		cout << "Do all the printers have a cost per page of " << costPerPage << " USD [Y/N]: ";
	cin >> userPrintCost;

	double tmpPrinterCost;

	if (userPrintCost == 'n' || userPrintCost == 'N') {
		cout << endl << "Please enter the cost per printed page for each printer (decimals allowed): " << endl;
		for (int i=0; i < printerCount; i++) {
			cout << "   Printer " << i+1 << " cost per page: ";
			cin >> tmpPrinterCost;
			while (!(tmpPrinterCost > 0)) {
				cerr << "ERROR: printer must print a positive cost per printed page!" << endl;
				cout << "   Printer " << i+1 << " cost per page: ";
				cin >> tmpPrinterCost;
			}
			printerCostArray[i] = tmpPrinterCost;
		}

	} else {
		for (int i = 0; i < printerCount; i++) {
			printerCostArray[i] = costPerPage;
		}
	}
}


void getPriorityQueueDetails(int& priorityCount, int*& priorityQueueCutOffs, int maxPages) {
	cout << endl << "How many priority levels do you have: ";
	cin >> priorityCount;
	while (priorityCount < 1) {
		cerr << "   ERROR: must have at least one priority level!" << endl;
		cout << "How many priority levels do you have: ";
		cin >> priorityCount;
	}

	priorityQueueCutOffs = new int[priorityCount];
	int previous = 1;
	int next;

	for (int i = 1; i < priorityCount; i++) {
		cout << "Page cut-off for level " << i << ": ";
		cin >> next;
		while (!(next < maxPages) || !(previous < next)) {
			cerr << "   ERROR: you must enter a value that is greater than " << previous;
			cerr << " and less than " << maxPages << endl;
			cout << "Page cut-off for level " << i << ": ";
			cin >> next;
		}
		priorityQueueCutOffs[i-1] = next;
		previous = next;
	}
	priorityQueueCutOffs[priorityCount-1] = maxPages;
	cout << endl;
}

/*
	getPrintSpeed
	Pre-Condition: the printer speed(s) are declared in the callers scope
	Post-Condition: sets the printer speed(s) based on the user input
*/
void getPrintSpeed(char& userPrintSpeed, int printerSpeed, int& printerCount, double*& printerSpeedArray,
	                bool userInputFromFile){

	printerSpeedArray = new double[printerCount];
	if (!userInputFromFile)
		cout << "Do all the printers have a speed of " << printerSpeed << " pages per minute [Y/N]: ";
	cin >> userPrintSpeed;

	double tmpPrinterSpeed;

	if (userPrintSpeed == 'n' || userPrintSpeed == 'N') {
		cout << endl << "Please enter the print speed for each printer (decimals allowed): " << endl;
		for (int i=0; i < printerCount; i++) {
			cout << "   Printer " << i+1 << " speed: ";
			cin >> tmpPrinterSpeed;
			while (!(tmpPrinterSpeed > 0)) {
				cerr << "ERROR: printer must print a positive number of pages per minute!" << endl;
				cout << "   Printer " << i+1 << " speed: ";
				cin >> tmpPrinterSpeed;
			}
			printerSpeedArray[i] = tmpPrinterSpeed;
		}
		cout << endl;
	} else {
		for (int i = 0; i < printerCount; i++) {
			printerSpeedArray[i] = (double)printerSpeed;
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
		cout << "Would you like to use the default seed value [Y/N]: ";
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

void setupIO(char& userInput, char& userOutput, ifstream &inStream, ofstream& outStream,
	          streambuf*& cinBuffer, streambuf*& coutBuffer, bool& userInputFromFile,
				 bool& userOutputToFile) {

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

void outputSimulationSettings(int printerCount, int printerSpeed, int numPrintJobs, int maxPages, 
							  int seedValue, char userPrintSpeed, double* printerSpeedArray, int priorityCount,
		                       int* priorityQueueCutOffs, double avgNumPrintJobsPerMinute, double costPerPage, 
		                       double* printerCostArray, int maintenanceThreshold){

	cout << "\n••••••••••••••••• BEGIN SIMULATION •••••••••••••••••\n";
	cout << "   Printers Available: " << printerCount << "\n";
	if (userPrintSpeed == 'y' || userPrintSpeed == 'Y') {
		cout << "   Printing Speed: " << printerSpeed << "\n";
	}else{
		for (int i=0; i<printerCount; i++)
			cout << "   Printing Speed for Printer " << i+1 << ": " << printerSpeedArray[i] << "\n";
	}
	cout << "   Number of print jobs: " << numPrintJobs << "\n";
	cout << "   Maximum possible page count: " << maxPages << "\n";
	cout << "   Seed Value: " << seedValue << "\n";
	cout << "   Number of Priority levels: " << priorityCount << "\n";
	for(int i=0; i<priorityCount; i++){
		cout << "   Priority Queue " << i+1 << " Cutoff: " << priorityQueueCutOffs[i] << "\n";
	}
	cout << "   Average Number of Jobs per Minute: " << avgNumPrintJobsPerMinute << "\n";
	cout << "   Cost Per Page: " << costPerPage << "\n";
	for(int i=0; i<printerCount; i++){
		cout << "   Printer " << i+1 << " Cost :" << printerCostArray[i] << "\n";
	}
	cout << "   Printers require maintenance after: " << maintenanceThreshold << " Pages \n";
	

}

void outputSimulationSummary(printerList* printers, int high, int med, int low, printScheduler* scheduler,
	                          int totalPagesPrinted, int tick){

	cout << "\n••••••••••••••••• SIMULATION SUMMARY •••••••••••••••••\n\n";
	printers->completionReport(cout);
	cout << "\n";

	cout << "    Total simulation time: " << tick << " minutes\n";
	// int freePrinterCount = printers->getFreePrinterCount();
	// if (freePrinterCount != 0) {
	// 	cout << "    Printer(s) ";
	// 	printers->listFreePrinters(cout);
	// 	cout << "were free at the end of simulation!\n";
	// } else {
	// 	cout << "    All printers were busy at the end of simulation!\n";
	// }

	cout << "    Total pages printed: " << totalPagesPrinted << " pages\n";
	cout << "    Total waiting time in the queue: " << high+med+low << " minutes\n";
	cout << "        in HIGH PRIOROTY queue: " << high << " minutes\n";
	cout << "        in MEDIUM PRIOROTY queue: " << med << " minutes\n";
	cout << "        in LOW PRIOROTY queue: " << low << " minutes\n";
	// cout << "    Jobs still waiting in the queue: " << scheduler->getLeftoverJobCount() << "\n";

	cout << "\n••••••••••••••••••••••••••••••••••••••••••••••••••••••\n";
}
