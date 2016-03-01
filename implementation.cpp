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

void printer::progressOneMinute(std::ostream& outStream, int& totalPagesPrinted) {
	int remainingPages, wholePages;

	if (printerBusy) {

		partialPages += printSpeed;
		wholePages = (int)partialPages;
		
//		std::cerr << "partial pages: " << partialPages << " whole pages: " << wholePages << "\n";

		remainingPages = currentPrintJob->printAtSpeed(wholePages, totalPagesPrinted, printCost, totalInkCost);
		partialPages -= wholePages;

//		std::cerr << printerID << " : whole pages: "<< wholePages << " total cost: " << totalInkCost << "\n";

		if (remainingPages != 0) {
			outStream << "      Printer " << printerID << " has " << remainingPages << " remaining pages. \n       [cumulative ink cost: $" << std::fixed << std::setprecision(2) << totalInkCost << "]\n";
		}else{
			outStream << "      Printer " << printerID << " has finished printing! \n       [cumulative ink cost: $" << std::fixed << std::setprecision(2) << totalInkCost << "]\n";
			completedJobs++;
			setPrinterFree();
		}
	}
}

void printer::setMaintenanceThreshold(int mt){
	maintenanceThreshold = mt;
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

void printer::setPrinterOffline(void) {
	printerOnline = false;
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

void printerList::progressOneMinute(std::ostream& outStream, int& totalPagesPrinted, double* PSA) {
	int wholePages = 0;
	outStream << "    =========== Printer Status ===========\n";
	for (int i = 0; i < numberOfPrinters; i++) {

		// printerSpeedArray[i] += PSA[i];    // add printer speeds i.e. 0.7+0.7 = 1.4 - 1 = 0.4  ... 0.4+0.7 = 1.1 - 1 = 0.1 ...
		// wholePages = (int)printerSpeedArray[i];
		// printerSpeedArray[i] -= wholePages;
		printers[i].progressOneMinute(outStream, totalPagesPrinted);
	}
	outStream << "    ======================================\n";
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
	outStream << ": got assigned to printer " << printers[i].getPrinterID() << "\n";
}

void printerList::listFreePrinters(std::ostream& outStream) {
	for (int i = 0; i < numberOfPrinters; i++) {
		if (printers[i].isFree())
			outStream << printers[i].getPrinterID() << " ";
	}
}

void printerList::completionReport(std::ostream& outStream) {
	for (int i = 0; i < numberOfPrinters; i++) {
		outStream << "    Printer " << printers[i].getPrinterID() << " successfully completed " << printers[i].getCompletedJobs() << " print jobs!\n";
	}
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
	outStream << "    New " << pageCount << " page job with ID: " << npj->getJobID();
	outStream << " got assigned to priority queue level: " << cutoffIndex << "\n";
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
				outStream << "    Job " << newJob->getJobID() << " Taken from priority queue level " << priorityLevel;
				plist.assignNewJob(*newJob, outStream);
				assignedOneJob = true;
			} else {
				priorityLevel++;
			}
		}

		attempts--;
	}
}

void printScheduler::calculateWaitingTimes(int& high, int& med, int& low) {
	high += highPriority.size();
	med += mediumPriority.size();
	low += lowPriority.size();
}

int printScheduler::getLeftoverJobCount(void) {
	return highPriority.size()+mediumPriority.size()+lowPriority.size();
}
