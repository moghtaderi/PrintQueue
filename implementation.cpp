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

int printJob::printAtSpeed(int wholePages, int& totalPagesPrinted){

	totalPagesPrinted += wholePages;
	pageCount -= wholePages;
	if (pageCount < 0) {
		 pageCount = 0;
	}
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

void printer::progressOneMinute(std::ostream& outStream, int& totalPagesPrinted) {
	int remainingPages, wholePages;

	if (printerBusy) {

		partialPages += printSpeed;
		wholePages = (int)partialPages;
		remainingPages = currentPrintJob->printAtSpeed(wholePages, totalPagesPrinted);
		partialPages -= wholePages;

		if (remainingPages != 0) {
			outStream << "      Printer " << printerID << " has " << remainingPages << " remaining pages\n";
		}else{
			outStream << "      Printer " << printerID << " has finished printing!\n";
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
	bool assignmentCompleted = false;
	for (int i = 0; i < numberOfPrinters && !assignmentCompleted; i++) {
		if (printers[i].isFree()){
			printers[i].setJob(npj);
			outStream << ": got assigned to printer " << printers[i].getPrinterID() << "\n";
			assignmentCompleted = true;
		}
	}
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

void printScheduler::scheduleNewPrintJob(printJob* npj, std::ostream& outStream) {
	int pageCount = npj->getPageCount();
	if(pageCount < 10){

		highPriority.push(npj);
		outStream << "    New " << pageCount << " page job with ID:" << npj->getJobID() << " got assigned to HIGH PRIOROTY queue!\n";
	}else if(pageCount < 20){

		mediumPriority.push(npj);
		outStream << "    New " << pageCount << " page job with ID " << npj->getJobID() << " got assigned to MEDIUM PRIOROTY queue!\n";
	}else{

		lowPriority.push(npj);

		outStream << "    New " << pageCount << " page job with ID:" << npj->getJobID() << " got assigned to LOW PRIOROTY queue!\n";
	}
}

void printScheduler::processJobs(int attempts, printerList& plist, std::ostream& outStream) {
	printJob* newJob;
	bool checkQueue = true;

	// Get the job based on priority
	while (attempts != 0){
		if(!highPriority.isEmpty()){
			newJob = highPriority.front();
			highPriority.pop();
			outStream << "    Job " << newJob->getJobID() << " Taken from HIGH PRIOROTY queue";
			plist.assignNewJob(*newJob, outStream);

		}else if(!mediumPriority.isEmpty()){
			newJob = mediumPriority.front();
			mediumPriority.pop();
			outStream << "    Job " << newJob->getJobID() << " Taken from MEDIUM PRIOROTY queue";
			plist.assignNewJob(*newJob, outStream);

		}else if(!lowPriority.isEmpty()){
			newJob = lowPriority.front();
			lowPriority.pop();
			outStream << "    Job " << newJob->getJobID() << " Taken from LOW PRIOROTY queue";
			plist.assignNewJob(*newJob, outStream);

		}else if(checkQueue) {
			// no more jobs left but free printers that are available
			outStream << "    No more jobs available in the scheduler for free printers: ";
			plist.listFreePrinters(outStream);
			outStream << "\n";
			checkQueue = false;
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
