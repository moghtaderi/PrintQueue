#include "headerfile.h"
#include <time.h>
#include <cstdlib>


using namespace std;

int main(int argc, char const *argv[]) {

	int printerCount;
	int printerSpeed;
	int numPrintJobs;
	int maxPages;
	char userSeed;
	int seedValue;

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


	cout << printerCount << "-" << printerSpeed << "-" << numPrintJobs << "-" << maxPages << "-" << seedValue << endl;

	return 0;
}