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

int printJob::printAtSpeed(int printerSpeed){

//	std::cout << "printeAtSpeed: " << printerSpeed << std::endl;

	pageCount -= printerSpeed;
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
}

void printer::setJob(printJob &newJob) {
	currentPrintJob = &newJob;
	setPrinterBusy();
//	std::cout << "printer number " << printerID << " set to BUSY!!*************************************" << std::endl;
}

void printer::setPrintSpeed(int ps){
	printSpeed = ps;
}

void printer::progressOneMinute(void) {
	int remainingPages;

//	std::cout << "printer " << printerID << " progressed for one minute!" << std::endl;

	if (printerBusy) {
		remainingPages = currentPrintJob->printAtSpeed(printSpeed);
//		std::cout << "***** PRINTER " << printerID << " HAS " << remainingPages << " REMAINING PAGES!!" << std::endl;
		if (remainingPages == 0) {
			//this->currentPrintJob.destructor
			setPrinterFree();
		}
	}
}

bool printer::isFree(void){
	return !printerBusy;
}

int printer::getPrinterID(void){
	return printerID;
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

void printerList::setPrintingSpeed(int printSpeed){
	for (int i = 0; i < numberOfPrinters; i++) {
		printers[i].setPrintSpeed(printSpeed);
	}
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

void printerList::assignNewJob(printJob &npj){
	bool assignmentCompleted = false;
	for (int i = 0; i < numberOfPrinters && !assignmentCompleted; i++) {
		if (printers[i].isFree()){
			printers[i].setJob(npj);
//			std::cout << "JOB " << npj.getJobID() << " GOT ASSIGNED TO PRINTER " << printers[i].getPrinterID() << std::endl;
			assignmentCompleted = true;
		}
	}
}

void printerList::listFreePrinters(std::ostream& outStream){
	for (int i = 0; i < numberOfPrinters; i++) {
		if (printers[i].isFree())
			outStream << printers[i].getPrinterID() << " ";
	}
	outStream << "\n";
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
printScheduler::printScheduler(){

}

void printScheduler::scheduleNewPrintJob(printJob* npj, std::ostream& outStream) {
	int pageCount = npj->getPageCount();
	if(pageCount < 10){

		highPriority.push(npj);
		outStream << "    JOB " << npj->getJobID() << " GOT ASSIGNED TO HIGH PRIOROTY QUEUE!\n";
	}else if(pageCount < 20){

		mediumPriority.push(npj);
		outStream << "    JOB " << npj->getJobID() << " GOT ASSIGNED TO MEDIUM PRIOROTY QUEUE!\n";
	}else{

		lowPriority.push(npj);

		outStream << "    JOB " << npj->getJobID() << " GOT ASSIGNED TO LOW PRIOROTY QUEUE!\n";
	}
}

void printScheduler::processJobs(int attempts, printerList& plist, std::ostream& outStream) {
	printJob* newJob;

	// Get the job based on priority
	while (attempts != 0){
		if(!highPriority.isEmpty()){
			newJob = highPriority.front();
			highPriority.pop();
			outStream << "    JOB " << newJob->getJobID() << " TAKEN FROM HIGH PRIOROTY QUEUE!\n";
			plist.assignNewJob(*newJob);

		}else if(!mediumPriority.isEmpty()){
			newJob = mediumPriority.front();
			mediumPriority.pop();
			outStream << "    JOB " << newJob->getJobID() << " TAKEN FROM MEDIUM PRIOROTY QUEUE!\n";
			plist.assignNewJob(*newJob);

		}else if(!lowPriority.isEmpty()){
			newJob = lowPriority.front();
			lowPriority.pop();
			outStream << "    JOB " << newJob->getJobID() << " TAKEN FROM LOW PRIOROTY QUEUE!\n";
			plist.assignNewJob(*newJob);

		}else{
			// no more jobs left but free printers available
			outStream << "    NO MORE JOBS IN THE QUEUE FOR PRINTERS: ";
			plist.listFreePrinters(outStream);
		}

		attempts--;
	}
}