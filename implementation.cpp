#include "headerfile.h"
#include <iostream>

//*********************************************************************
printJob::printJob(){

}

printJob::printJob(int pgcnt, int pjid) {
	this->pageCount = pgcnt;
	this->jobID = pjid;
}

int printJob::getPageCount(void) {
	return this->pageCount;
}

int printJob::printAtSpeed(int printerSpeed){
	this->pageCount -= printerSpeed;
	if (this->pageCount < 0) {
		this-> pageCount = 0;
	}
	return this->pageCount;
}

int printJob::getJobID(void){
	return this->jobID;
}

//*********************************************************************
printer::printer() {

}

void printer::setJob(printJob newJob) {
	this->currentPrintJob = newJob;
	this->setPrinterBusy();
}

void printer::progressOneMinute(void) {
	int remainingPages;

	if (printerBusy) {
		remainingPages = currentPrintJob.printAtSpeed(this->printSpeed);
		if (!remainingPages) {
			this->setPrinterFree();
		}
	}
}

bool printer::isFree(void){
	return !(this->printerBusy);
}

//PRIVATE MEMBER FUNCTIONS=============
void printer::setPrinterBusy(void) {
	this->printerBusy = true;
}

void printer::setPrinterFree(void) {
	this->printerBusy = false;
}
//=====================================

//*********************************************************************
printerList::printerList(int printerCount) {
	numberOfPrinters = printerCount;
	printers = new printer[numberOfPrinters];
}

void printerList::progressOneMinute(void) {
	for (int i = 0; i < numberOfPrinters; i++) {
		printers[i].progressOneMinute();
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

void printerList::assignNewJob(printJob npj){
	for (int i = 0; i < numberOfPrinters; i++) {
		if (printers[i].isFree())
			printers[i].setJob(npj);
	}
}


//*********************************************************************
printJobWaitingQueue::printJobWaitingQueue(int size) {

}

bool printJobWaitingQueue::isEmpty(void) {
	return this->empty();
}

//*********************************************************************
printScheduler::printScheduler(){

}

void printScheduler::scheduleNewPrintJob(printJob npj) {
	int pageCount = npj.getPageCount();
	
	if(pageCount < 10){
		highPriority.push(npj);
	}else if(pageCount < 20){
		mediumPriority.push(npj);
	}else{
		lowPriority.push(npj);
	}
}

void printScheduler::processJobs(int jobCount, printerList& plist, std::ostream& outStream) {
	printJob newJob;

	// Get the job based on priority
	while (jobCount != 0){
		if(!highPriority.isEmpty()){
			newJob = highPriority.front();
			highPriority.pop();
			outStream << "    JOB " << newJob.getJobID() << " TAKEN FROM HIGH PRIOROTY QUEUE!\n";
		}else if(!mediumPriority.isEmpty()){
			newJob = mediumPriority.front();
			mediumPriority.pop();
			outStream << "    JOB " << newJob.getJobID() << " TAKEN FROM MEDIUM PRIOROTY QUEUE!\n";
		}else if(!lowPriority.isEmpty()){
			newJob = lowPriority.front();
			lowPriority.pop();
			outStream << "    JOB " << newJob.getJobID() << " TAKEN FROM LOW PRIOROTY QUEUE!\n";
		}else{
			// no more jobs left but free printers available
			outStream << "    NO MORE JOBS IN THE QUEUE FOR THE FREE PRINTER(S)!\n";
		}

		jobCount--;
	}

	plist.assignNewJob(newJob);
}