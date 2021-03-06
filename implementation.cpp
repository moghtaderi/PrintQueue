#include "headerfile.h"
#include <iostream>

//*********************************************************************
//printJob class functions
printJob::printJob(){

}

printJob::printJob(int pgcnt, int pjid) {
	pageCount = pgcnt;
	jobID = pjid;
}

int printJob::getPageCount(void) {
	return pageCount;
}

int printJob::printAtSpeed(int wholePages, int& totalPagesPrinted, double printCost, double &totalInkCost){

	int finalPageCount = pageCount;

	totalPagesPrinted += wholePages;
	pageCount -= wholePages;

	if (pageCount < 0) {
		 pageCount = 0;
	}

	finalPageCount -= pageCount;
	totalInkCost += finalPageCount * printCost;

	return pageCount;
}

int printJob::getJobID(void){
	return jobID;
}

//*********************************************************************
//printer class functions
printer::printer() {
	static int pid;
	printerID = ++pid;
	printerBusy = false;
	completedJobs = 0;
	partialPages = 0.0;
	printerOnline = true;
	totalInkCost = 0.0;
	jamTimeLeft = 0;
}

void printer::setJob(printJob &newJob) {
	currentPrintJob = &newJob;
	setPrinterBusy();
}

void printer::setPrintSpeed(double ps){
	printSpeed = ps;
}

void printer::setPrintCost(double pc){
	printCost = pc;
}

double printer::getPrintCost(void){
	return totalInkCost;
}


int printer::getPagesPrinted(void){
	return totalInkCost/printCost;
}

void printer::progressOneMinute(std::ostream& outStream, int& totalPagesPrinted, int jamTime, double jamPrecentage, int maintenanceTime) {

	int remainingPages, wholePages;
	bool readyToGoOffline;

	if(printerOnline) {

		if (printerBusy) {

			if ((rand()%1000)/1000.0 <= jamPrecentage)
				jamTimeLeft = jamTime;

			//check if not jammed
			if(jamTimeLeft == 0){

				partialPages += printSpeed;
				wholePages = (int)partialPages;
				remainingPages = currentPrintJob->printAtSpeed(wholePages, totalPagesPrinted, printCost, totalInkCost);

				partialPages -= wholePages;
				maintenanceThresholdLeft -= wholePages;

				if(maintenanceThresholdLeft <= 0)
					readyToGoOffline = true;

				if (remainingPages != 0) {
					outStream << "      Printer " << printerID << " has " << remainingPages << " remaining pages \n       [cumulative ink cost: \x1b[32m$" << std::fixed << std::setprecision(2) << totalInkCost << "\x1b[0m]\n";
				}else{
					outStream << "      Printer " << printerID << " has finished printing! \n       [cumulative ink cost: \x1b[32m$" << std::fixed << std::setprecision(2) << totalInkCost << "\x1b[0m]\n";
					completedJobs++;
					setPrinterFree();

					if (readyToGoOffline){
						setPrinterOffline(maintenanceTime);
						readyToGoOffline = false;
					}

				}

			}else{
					outStream << "      \x1b[31m  *** Printer " << printerID << " is JAMMED! ***\n       Fix time remaining: " << jamTimeLeft << " minutes\x1b[0m\n";
			}
		}

		jamTimeLeft--;
		if (jamTimeLeft < 0) {
			jamTimeLeft = 0;
		}

	}else{
		outStream << "      \x1b[33m ### Printer " << printerID << " is OFFLINE! ###\n         Back online in: " << maintenanceTimeLeft << " minutes\x1b[0m\n";
		maintenanceTimeLeft--;
		if (maintenanceTimeLeft <= 0) {
			maintenanceTimeLeft = 0;
			// done with maintenance... ready for another X pages!
			maintenanceThresholdLeft = maintenanceThreshold;
			setPrinterOnline();
		}
	}


}

void printer::setMaintenanceThreshold(int mt){
	maintenanceThreshold = mt;
	maintenanceThresholdLeft = maintenanceThreshold;
}

bool printer::isFree(void){
	return !printerBusy;
}

int printer::getPrinterID(void){
	return printerID;
}

int printer::getCompletedJobs(void){
	return completedJobs;
}

void printer::setPrinterOnline(void) {
	printerOnline = true;
}

void printer::setPrinterOffline(int maintenanceTime) {
	printerOnline = false;
	maintenanceTimeLeft = maintenanceTime;
}

//PRIVATE MEMBER FUNCTIONS=============
void printer::setPrinterBusy(void) {
	printerBusy = true;
}

void printer::setPrinterFree(void) {
	printerBusy = false;
}
//=====================================

//*********************************************************************
//printerList class functions
printerList::printerList(int printerCount) {
	numberOfPrinters = printerCount;
	printers = new printer[numberOfPrinters];
}

void printerList::setPrintingSpeed(double* PSA){
	for (int i = 0; i < numberOfPrinters; i++) {
		printers[i].setPrintSpeed(PSA[i]);
	}
}

void printerList::setPrintingCost(double* PCA){
	for (int i = 0; i < numberOfPrinters; i++) {
		printers[i].setPrintCost(PCA[i]);
	}
}

void printerList::setPrintingMaintenanceThreshold(int MT){
	for (int i = 0; i < numberOfPrinters; i++) {
		printers[i].setMaintenanceThreshold(MT);
	}
}

// void printerList::setPrinterJamRate(int min){
// 	for(int i = 0;i < numberOfPrinters; i++){
// 		printers[i].setJamTimeLeft(min);
// 	}
// }

void printerList::progressOneMinute(std::ostream& outStream, int& totalPagesPrinted, double* PSA, int jamTime, double jamPrecentage, int maintenanceTime) {
	int wholePages = 0;
	outStream << "    =========== Printer Status ===========\n";
	for (int i = 0; i < numberOfPrinters; i++) {

		// printerSpeedArray[i] += PSA[i];    // add printer speeds i.e. 0.7+0.7 = 1.4 - 1 = 0.4  ... 0.4+0.7 = 1.1 - 1 = 0.1 ...
		// wholePages = (int)printerSpeedArray[i];
		// printerSpeedArray[i] -= wholePages;
		printers[i].progressOneMinute(outStream, totalPagesPrinted, jamTime, jamPrecentage, maintenanceTime);
	}
}

int printerList::getFreePrinterCount(void) {
	int cnt = 0;

	for (int i = 0; i < numberOfPrinters; i++) {
		if (printers[i].isFree())
			cnt++;
	}

	return cnt;
}

void printerList::assignNewJob(printJob &npj, std::ostream& outStream) {
	int i;

	do {
		i = rand()%numberOfPrinters;
	} while(!printers[i].isFree());

	printers[i].setJob(npj);
	outStream << " - got assigned to printer " << printers[i].getPrinterID() << "\n";
}

void printerList::listFreePrinters(std::ostream& outStream) {
	for (int i = 0; i < numberOfPrinters; i++) {
		if (printers[i].isFree())
			outStream << printers[i].getPrinterID() << " ";
	}
}

void printerList::completionReport(std::ostream& outStream) {
	double totalCost = 0.00;
	int totalPagsPrinted = 0;
	for (int i = 0; i < numberOfPrinters; i++) {
		outStream << "    Printer " << printers[i].getPrinterID() << " successfully completed " << printers[i].getCompletedJobs() << " print jobs!\n";
		outStream << "      These jobs used \x1b[32m$" << printers[i].getPrintCost() << "\x1b[0m ink and "<< printers[i].getPagesPrinted() << " pieces of paper\n";
		totalCost += printers[i].getPrintCost();
		totalPagsPrinted += printers[i].getPagesPrinted();
	}
	outStream << "\n" << "    Total Cost: \x1b[32m$" << totalCost << "\033[0m\n";
	outStream << "    Total Pages Printed: " << totalPagsPrinted << " pages \n";
}

int printerList::getCompletedJobsCount(void){
	int tcj = 0;

	for (int i = 0; i < numberOfPrinters; i++)
		tcj += printers[i].getCompletedJobs();

	return tcj;
}


//*********************************************************************
//printJobWaitingQueue class functions
printJobWaitingQueue::printJobWaitingQueue(int size) {

}

bool printJobWaitingQueue::isEmpty(void) {
	return empty();
}

// printJobWaitingQueue::printJobWaitingQueue(printJobWaitingQueue* otherQueue){
// 	while(!otherQueue->isEmpty()){
// 		std::cout << otherQueue->front().getPageCount() << " ";
// 		otherQueue->pop();
// 	}
// }

// void printJobWaitingQueue::listQueueContent(std::ostream& outStream){
// 	printJobWaitingQueue temp = printJobWaitingQueue(this);
// 	while(!temp.empty()){
// 		outStream << temp.front().getJobID() << " ";
// 		temp.pop();
// 	}
// }

//*********************************************************************
//printScheduler class functions
printScheduler::printScheduler(int* priorityQueueCutOffs, int priorityCount){
	queueArray = new printJobWaitingQueue[priorityCount];
}

void printScheduler::scheduleNewPrintJob(printJob* npj, std::ostream& outStream, int cutoffIndex) {
	int pageCount = npj->getPageCount();

	queueArray[cutoffIndex].push(npj);
	outStream << "    \x1b[36mNew " << pageCount << " page job with ID " << npj->getJobID();
	outStream << " - got assigned to priority queue level: " << cutoffIndex+1 << "\x1b[0m\n";
}

void printScheduler::processJobs(int attempts, printerList& plist, std::ostream& outStream, int priorityCount) {
	printJob* newJob;
	//bool checkQueue = true;
	bool assignedOneJob;
	int priorityLevel = 0;

	// Get the job based on priority
	while (attempts != 0){

		assignedOneJob = false;
		while(priorityLevel != priorityCount && !assignedOneJob){
			if(!queueArray[priorityLevel].isEmpty()){
				newJob = queueArray[priorityLevel].front();
				queueArray[priorityLevel].pop();
				outStream << "    \x1b[35mJob " << newJob->getJobID() << " Taken from priority queue level " << priorityLevel+1;
				plist.assignNewJob(*newJob, outStream);
				outStream << "\x1b[0m";
				assignedOneJob = true;
			} else {
				priorityLevel++;
			}
		}

		attempts--;

	}
}

void printScheduler::calculateWaitingTimes(int*& waitTimesArray, int priorityCount) {

	for (int i = 0; i < priorityCount; i++) {
		waitTimesArray[i]+=queueArray[i].size();
	}
}

int printScheduler::getLeftoverJobCount(int priorityCount) {

	int total = 0;
	for (int i = 0; i < priorityCount; i++) {
		total += queueArray[i].size();
	}

	return total;
}

int printScheduler::getJobCountForQueueIndex(int index) {
	return queueArray[index].size();
}
