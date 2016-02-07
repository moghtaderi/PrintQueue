#include "headerfile.h"
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char const *argv[]) {

	int printerCount;
	int printerSpeed;
	int numPrintJobs;
	int maxPages;
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
	cout << "Printing speed in pages per minute ----- ";
	cin >> printerSpeed;
	cout << "Total number of simulated print jobs --- ";
	cin >> numPrintJobs;
	cout << "Maximum page count per print job ------- ";
	cin >> maxPages;

	cout << "Would you like to provide the seed value [Y/N]: ";
	cin >> userSeed;

	if (userSeed == 'y' || userSeed == 'Y') {
		cout << "Please enter your desired seed value: ";
		cin >> seedValue;
	}else{
		seedValue = time(NULL);
	}
	srand(seedValue);

	cout << "Would you like the output on the screen [Y/N]: ";
	cin >> userOutput;
	if (userOutput == 'y' || userOutput == 'Y') {
		fileIO = false;
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
   	// NOTE: the rest of the file can use cout but it will be redirected to outfile
	}

	cout << printerCount << "-" << printerSpeed << "-" << numPrintJobs << "-" << maxPages << "-" << seedValue << endl;

	cout.rdbuf(coutBuffer);                 // reset cout buffer
	outfile.close();                        // close the output file buffer

	return 0;
}