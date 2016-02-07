#include "headerfile.h"
#include <iostream>

using namespace std;

printJob::printJob(int pgcnt) {
	this->pageCount = pgcnt;
}

printer::printer() {

}

void printer::setPrinterBusy(void) {
	this->printerBusy = true;
}

void printer::setPrinterFree(void) {
	this->printerBusy = false;
}

printerList::printerList() {

}

printJobWaitingQueue::printJobWaitingQueue(int size) {

}