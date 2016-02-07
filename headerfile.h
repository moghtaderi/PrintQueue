#include <iostream>
#include <queue>

//***********************************************************************************
class printJob {
public:
	printJob(int pgcnt);

private:
	int pageCount;
};


//***********************************************************************************
class printer {
public:
	printer();
	void setPrinterBusy(void);
	void setPrinterFree(void);

private:
	printJob currentPrintJob;
	bool printerBusy;
};


//***********************************************************************************
class printerList {
public:
	printerList();

private:
	int numberOfPrinters;
	printer* printers;
};


//***********************************************************************************
class printJobWaitingQueue : public std::queue<printJob> {
public:
	printJobWaitingQueue(int size = 100);
	// will use the queue destructor
private:
	
};