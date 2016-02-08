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

private:
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

private:
	int numberOfPrinters;
	printer* printers;
};


//***********************************************************************************
class printJobWaitingQueue : public std::queue<printJob> {
public:
	printJobWaitingQueue(int size = 100);
	// will use the queue destructor
	bool isEmpty();
private:
	
};

//***********************************************************************************
class printScheduler {
public:
	printScheduler();
	void scheduleNewPrintJob(printJob npj);
	void processJobs(int jobCount, printerList& plist, std::ostream& outStream);

private:
	printJobWaitingQueue highPriority;
	printJobWaitingQueue mediumPriority;
	printJobWaitingQueue lowPriority;
};