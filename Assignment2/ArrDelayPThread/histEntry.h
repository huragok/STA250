/* Class of histogram entry (category)*/

#ifndef HISTENTRY
#define HISTENTRY

#include <cstddef>
using namespace std;

class histEntry
{
public:
	int arrDelay; // Arrival delay
	int freq; // Frequency
	histEntry *ptrNext; // Pointer to the next histogram category

	histEntry(int delay); // Constructor, needed when adding a new entry to the histogram
};

#endif
