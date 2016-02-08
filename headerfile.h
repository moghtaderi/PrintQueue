#include <iostream>
#include <queue>

//***********************************************************************************
class printJob {
public:
	printJob();
	printJob(int pgcnt, int pjid);

	int getPageCount(void);
	int printAtSpeed(int printerSpeed);
	int getJobID(void);

private:
	int pageCount;
	int jobID;
};


//***********************************************************************************
class printer {	
public:	
	printer();
	void progressOneMinute(void);
	bool isFree(void);
	void setJob(printJob newJob);
	void setPrintSpeed(int ps);
	int getPrinterID(void);

private:
	int printerID;
	int printSpeed;
	printJob currentPrintJob;
	bool printerBusy;

	void setPrinterBusy(void);
	void setPrinterFree(void);
};


//***********************************************************************************
class printerList {
public:
	printerList(int printerCount);
	void progressOneMinute(void);
	int getFreePrinterCount(void);
	void assignNewJob(printJob npj);
	void setPrintingSpeed(int printSpeed);
	void listFreePrinters(std::ostream& outStream);

private:
	int numberOfPrinters;
	printer* printers;
};


//***********************************************************************************
class printJobWaitingQueue : public std::queue<printJob> {
public:
	printJobWaitingQueue(int size = 100);
	// printJobWaitingQueue(printJobWaitingQueue* otherQueue); // copy constructor

	bool isEmpty();
	// void listQueueContent(std::ostream& outStream);
	
private:
	
};

//***********************************************************************************
class printScheduler {
public:
	printScheduler();
	void scheduleNewPrintJob(printJob npj, std::ostream& outStream);
	void processJobs(int attempts, printerList& plist, std::ostream& outStream);

private:
	printJobWaitingQueue highPriority;
	printJobWaitingQueue mediumPriority;
	printJobWaitingQueue lowPriority;
};
