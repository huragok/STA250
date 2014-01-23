#include "hist.h"
#include <math.h>

hist::hist(void)
{
	numEntry = 0;
	numRight = 0;
	numLeft = 0;
	ptrRight = NULL;
	ptrLeft = NULL;
}

hist::~hist(void)
{
	histEntry *ptrCurrent = NULL, *ptrTemp = NULL;

	ptrCurrent = ptrLeft;
	while (ptrCurrent != NULL)
	{
		ptrTemp = ptrCurrent;
		delete ptrTemp;
		ptrCurrent = ptrCurrent -> ptrNext;
	}

	ptrCurrent = ptrRight;
	while (ptrCurrent != NULL)
	{
		ptrTemp = ptrCurrent;
		delete ptrTemp;
		ptrCurrent = ptrCurrent -> ptrNext;
	}
}

void hist::AddEntry(int d)
{
	histEntry *ptrCurrent = NULL;

	if (d >= 0)
	{
		numRight++;
		ptrCurrent = ptrRight;
		if (ptrCurrent == NULL)
		{
			ptrRight = new histEntry(d);
			numEntry++;
			return;
		}
		else
		{
			if (ptrRight -> arrDelay > d)
			{
				histEntry *ptrTemp = new histEntry(d);
				ptrTemp -> ptrNext = ptrRight;
				ptrRight = ptrTemp;
				numEntry++;
				return;
			}

			while ((ptrCurrent -> ptrNext) != NULL)
			{
				if ((ptrCurrent -> arrDelay) == d)
				{
					(ptrCurrent -> freq)++;
					return;
				}
				else if((ptrCurrent -> arrDelay) < d && ((ptrCurrent -> ptrNext) -> arrDelay) > d)
				{
					histEntry *ptrTemp = new histEntry(d);
					ptrTemp -> ptrNext = ptrCurrent -> ptrNext;
					ptrCurrent -> ptrNext = ptrTemp;
					numEntry++;
					return;
				}
				ptrCurrent = ptrCurrent -> ptrNext;
			}

			if ((ptrCurrent -> arrDelay) == d)
			{
				(ptrCurrent -> freq)++;
				return;
			}
			else
			{
				histEntry *ptrTemp = new histEntry(d);
				ptrCurrent -> ptrNext = ptrTemp;
				return;
			}
		}
	}
	else
	{
		numLeft++;
		ptrCurrent = ptrLeft;
		if (ptrCurrent == NULL)
		{
			ptrLeft = new histEntry(d);
			numEntry++;
			return;
		}
		else
		{
			if (ptrLeft -> arrDelay < d)
			{
				histEntry *ptrTemp = new histEntry(d);
				ptrTemp -> ptrNext = ptrLeft;
				ptrLeft = ptrTemp;
				numEntry++;
				return;
			}

			while ((ptrCurrent -> ptrNext) != NULL)
			{
				if ((ptrCurrent -> arrDelay) == d)
				{
					(ptrCurrent -> freq)++;
					return;
				}
				else if((ptrCurrent -> arrDelay) > d && ((ptrCurrent -> ptrNext) -> arrDelay) < d)
				{
					histEntry *ptrTemp = new histEntry(d);
					ptrTemp -> ptrNext = ptrCurrent -> ptrNext;
					ptrCurrent -> ptrNext = ptrTemp;
					numEntry++;
					return;
				}
				ptrCurrent = ptrCurrent -> ptrNext;
			}
			if ((ptrCurrent -> arrDelay) == d)
			{
				(ptrCurrent -> freq)++;
				return;
			}
			else
			{
				histEntry *ptrTemp = new histEntry(d);
				ptrCurrent -> ptrNext = ptrTemp;
				return;
			}
		}
	}
}

double hist::mean(void)
{
	double mean = 0;
	histEntry *ptrCurrent = NULL;
	double numTotal = (double)numLeft + (double)numRight;

	ptrCurrent = ptrRight;
	while (ptrCurrent != NULL)
	{
		mean += (double)(ptrCurrent -> arrDelay) * (double)(ptrCurrent -> freq) / numTotal;
		ptrCurrent = ptrCurrent -> ptrNext;
	}

	ptrCurrent = ptrLeft;
	while (ptrCurrent != NULL)
	{
		mean += (double)(ptrCurrent -> arrDelay) * (double)(ptrCurrent -> freq) / numTotal;
		ptrCurrent = ptrCurrent -> ptrNext;
	}
	return mean;
}

double hist::stdev(void)
{
	double meanSqr = 0;
	histEntry *ptrCurrent = NULL;
	double numTotal = (double)numLeft + (double)numRight;

	ptrCurrent = ptrRight;
	while (ptrCurrent != NULL)
	{
		meanSqr += pow((double)(ptrCurrent -> arrDelay), 2)* (double)(ptrCurrent -> freq) / numTotal;
		ptrCurrent = ptrCurrent -> ptrNext;
	}

	ptrCurrent = ptrLeft;
	while (ptrCurrent != NULL)
	{
		meanSqr += pow((double)(ptrCurrent -> arrDelay), 2) * (double)(ptrCurrent -> freq) / numTotal;
		ptrCurrent = ptrCurrent -> ptrNext;
	}
	double m = mean();

	return sqrt(meanSqr - pow(m, 2));
}

int hist::median(void)
{
	histEntry *ptrCurrent = NULL;
	int dif;
	int difCum = 0;
	if (numLeft == numRight)
	{
		if (numLeft != 0)
		{
			return ptrLeft -> arrDelay;
		}
	}
	else if (numRight > numLeft)
	{
		dif = (numRight - numLeft) / 2;
		ptrCurrent = ptrRight;
		while (ptrCurrent != NULL)
		{
			difCum += ptrCurrent -> freq;
			if (difCum > dif)
			{
				return ptrCurrent -> arrDelay;
			}
			ptrCurrent = ptrCurrent -> ptrNext;
		}
	}
	else
	{
		dif = (numLeft - numRight) / 2;
		ptrCurrent = ptrLeft;
		while (ptrCurrent != NULL)
		{
			difCum += ptrCurrent -> freq;
			if (difCum > dif)
			{
				return ptrCurrent -> arrDelay;
			}
			ptrCurrent = ptrCurrent -> ptrNext;
		}
	}
}
