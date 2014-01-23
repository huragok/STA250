/* Class of histogram (frequency table) */

#ifndef HIST
#define HIST

#include <cstddef>
using namespace std;

#include "histEntry.h"

class hist
{
public:
	int numEntry, numLeft, numRight; // Number of histogram entry (category), sum of ArrDelay smaller than zero, greater or equal to zero

	/* The negative and the positive categories are stored in two linked list with different searching directions (decreasing and increasing)*/
	histEntry *ptrRight; // Head pointer to the right linked list
	histEntry *ptrLeft; // Head pointer to the left linked list

	hist(void); // Constructor
	~hist(void); // Destructor

	void AddEntry(int arrDelay); // Add a new arrDelay to the histogram
	double mean(void); // Evaluate the mean
	double stdev(void); // Evaluate the standard deviation
	int median(void); // Evaluate the median

};

#endif
