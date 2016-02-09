#include <iostream>
#include <queue>

//***********************************************************************************
/*
printJob class:
*/
class printJob {
public:
	//printJob constructor
	printJob();
	printJob(int pgcnt, int pjid);

	//getPageCount
	//Function to return the number of pages
	//Pre-condition: none
	//Post-condition: returns the number of pages of the current job
	int getPageCount(void);

	//printAtSpeed
	//Function that "prints" at the speed given by the user
	//Pre-condition: the print speed given by the user
	//Post-condition: returns the number of pages printed
	int printAtSpeed(int printerSpeed);

	//getJobID
	//Function that gets the job ID
	//Pre-condition: none
	//Post-condition: returns the job ID
	int getJobID(void);

private:
	int pageCount;
	int jobID;
};


//***********************************************************************************
/*
printer class:
*/
class printer {
public:
	//printer constructor
	printer();

	//progressOneMinute
	//Function to tell you how many pages are left
	//Pre-Condition: none
	//Post-Condition: increments with the number of pages left
	void progressOneMinute(std::ostream& outStream);

	//isFree
	//Function that returns the status of the printer
	//Pre-condition: none
	//Post-condition: returns the status of the printer
	bool isFree(void);

	//setJob
	//Function to give a printer a job
	//Pre-condition: printer's job
	//Post-condition: sets the printer to busy based on its new job
	void setJob(printJob &newJob);

	//setPrintSpeed
	//Function to set the print speed
	//Pre-condition: the printer speed given by the user
	//ps: print speed
	//Post-condition: sets the printer speed
	void setPrintSpeed(int ps);

	//getPrinterID
	//Function that returns the printer number
	//Pre-condition: none
	//Post-condition: returns the printer ID
	int getPrinterID(void);

	//getCompletedJobs
	//Function that returns the number of completed print jobs
	//Pre-condition: none
	//Post-condition: returns the completed job count
	int getCompletedJobs(void);

private:
	int printerID;
	int printSpeed;
	printJob *currentPrintJob;
	bool printerBusy;
	int completedJobs;

	//setPrinterBusy
	//Function to set the printer to busy
	//Pre-condition: none
	//Post-condition: sets the printer to busy
	void setPrinterBusy(void);

	//setPrinterFree
	//Function to set the printer to free
	//Pre-condition: none
	//Post-condition: sets the printer to free
	void setPrinterFree(void);
};


//***********************************************************************************
/*
printerList class:
*/
class printerList {
public:
	//printerList constructor
	printerList(int printerCount);

	//progressOneMinute
	//Function to increase the progress of each printer each minute
	//Pre-condition: none
	//Post-condition: increments the progress of each printer in the queue
	void progressOneMinute(std::ostream& outStream);

	//getFreePrinterCount
	//Function that returns the number of free printers
	//Pre-condition: none
	//Post-condition: Returns the number of free printers
	int getFreePrinterCount(void);

	//assignNewJob
	//Function that assigns a printer a job if it is free
	//npj: new print job
	//Pre-condition: new print job to be assigned
	//Post-condition: assigns a free printer a new print job
	void assignNewJob(printJob &npj, std::ostream& outStream);

	//setPrintingSpeed
	//Function to set the printer speed for all printers
	//Pre-condition: printer speed given by the user
	//Post-condition: sets the printing speed for all printers
	void setPrintingSpeed(int printSpeed);

	//listFreePrinters
	//Function to output a list of free printers
	//Pre-condition: outstream
	//Post-condition: outputs a list of free printers
	void listFreePrinters(std::ostream& outStream);

	//completionReport
	//Function that outputs the completed jobs for all printers
	//Pre-condition: none
	//Post-condition: returns the completed jobs for all printers
	void completionReport(std::ostream& outStream);

private:
	int numberOfPrinters;
	printer* printers;
};


//***********************************************************************************
/*
printJobWaitingQueue class
*/
class printJobWaitingQueue : public std::queue<printJob*> {
public:
	//printJobWaitingQueue constructor
	printJobWaitingQueue(int size = 100);
	// printJobWaitingQueue(printJobWaitingQueue* otherQueue); // copy constructor

	//isEmpty
	//Function that returns if the waiting queue is empty
	//Pre-condition: none
	//Post-condition: returns if the queue is empty
	bool isEmpty();

	// void listQueueContent(std::ostream& outStream);

private:

};

//***********************************************************************************
/*
printScheduler class
*/
class printScheduler {
public:
	//printScheduler constructor
	printScheduler();

	//scheduleNewPrintJob
	//Function that assigns the priority of the jobs
	//npj: new print job
	//Pre-condition: new print job, ostream
	//Post-condition: schedules the jobs' priorities
	void scheduleNewPrintJob(printJob* npj, std::ostream& outStream);

	//processJobs
	//Function to process the jobs based on priority
	//attempts: the number of free printers
	//Pre-condition: number of attempts to grab a free printer, list of printers, ostream
	//Post-condition: outputs the jobs taken from different priority queues
	void processJobs(int attempts, printerList& plist, std::ostream& outStream);

	void calculateWaitingTimes(int& high, int& med, int& low);
	int getLeftoverJobCount(void);

private:
	printJobWaitingQueue highPriority;
	printJobWaitingQueue mediumPriority;
	printJobWaitingQueue lowPriority;
};
