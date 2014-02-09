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
					ptrTemp -> ptrNext = (ptrCurrent -> ptrNext);
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

void hist::CatHist(hist histToCat)
{
	histEntry *ptrCurrent = NULL, *ptrCurrentToCat = NULL, *ptrLast = NULL;

	ptrCurrent = ptrRight;
	ptrCurrentToCat = histToCat.ptrRight;
	if (ptrCurrent == NULL && ptrCurrentToCat != NULL)
	{
		ptrRight = new histEntry(ptrCurrentToCat->arrDelay);
		ptrRight->freq = ptrCurrentToCat->freq;
		numEntry += 1;
		numRight += ptrCurrentToCat->freq;
		ptrCurrent = ptrRight;
		ptrCurrentToCat = ptrCurrentToCat->ptrNext;
	}
	while (ptrCurrentToCat != NULL)
	{
		while ((ptrCurrent != NULL) && (ptrCurrent->arrDelay < ptrCurrentToCat->arrDelay))
		{
			ptrLast = ptrCurrent;
			ptrCurrent = ptrCurrent->ptrNext;
		}
		if (ptrCurrent == NULL)
		{
			ptrCurrent = new histEntry(ptrCurrentToCat->arrDelay);
			ptrCurrent->freq = ptrCurrentToCat->freq;
			ptrLast->ptrNext = ptrCurrent;
			numEntry += 1;
			numRight += ptrCurrentToCat->freq;
		}
		else if (ptrCurrent->arrDelay == ptrCurrentToCat->arrDelay)
		{
			ptrCurrent->freq += ptrCurrentToCat->freq;
			numRight += ptrCurrentToCat->freq;
		}
		else
		{
			histEntry *ptrTemp = new histEntry(ptrCurrentToCat->arrDelay);
			ptrTemp->freq = ptrCurrentToCat->freq;
			ptrTemp->ptrNext = ptrCurrent;
			ptrLast->ptrNext = ptrTemp;
			numEntry += 1;
			numRight += ptrCurrentToCat->freq;;
		}
		ptrCurrentToCat = ptrCurrentToCat->ptrNext;
	}

	ptrCurrent = ptrLeft;
	ptrCurrentToCat = histToCat.ptrLeft;
	if (ptrCurrent == NULL && ptrCurrentToCat != NULL)
	{
		ptrLeft = new histEntry(ptrCurrentToCat->arrDelay);
		ptrLeft->freq = ptrCurrentToCat->freq;
		numEntry += 1;
		numLeft += ptrCurrentToCat->freq;
		ptrCurrent = ptrLeft;
		ptrCurrentToCat = ptrCurrentToCat->ptrNext;
	}
	while (ptrCurrentToCat != NULL)
	{
		while ((ptrCurrent != NULL) && (ptrCurrent->arrDelay > ptrCurrentToCat->arrDelay))
		{
			ptrLast = ptrCurrent;
			ptrCurrent = ptrCurrent->ptrNext;
		}
		if (ptrCurrent == NULL)
		{
			ptrCurrent = new histEntry(ptrCurrentToCat->arrDelay);
			ptrCurrent->freq = ptrCurrentToCat->freq;
			ptrLast->ptrNext = ptrCurrent;
			numEntry += 1;
			numLeft += ptrCurrentToCat->freq;
		}
		else if (ptrCurrent->arrDelay == ptrCurrentToCat->arrDelay)
		{
			ptrCurrent->freq += ptrCurrentToCat->freq;
			numLeft += ptrCurrentToCat->freq;
		}
		else
		{
			histEntry *ptrTemp = new histEntry(ptrCurrentToCat->arrDelay);
			ptrTemp->freq = ptrCurrentToCat->freq;
			ptrTemp->ptrNext = ptrCurrent;
			ptrLast->ptrNext = ptrTemp;
			numEntry += 1;
			numLeft += ptrCurrentToCat->freq;
		}
		ptrCurrentToCat = ptrCurrentToCat->ptrNext;
	}
}

void hist::DispHist(void)
{
	histEntry *ptrCurrent = NULL;

	cout <<"DispHist______________"<<endl;
	ptrCurrent = ptrLeft;
	while (ptrCurrent != NULL)
	{
		cout << ptrCurrent->arrDelay << " " << ptrCurrent->freq << endl;
		ptrCurrent = ptrCurrent -> ptrNext;
	}

	ptrCurrent = ptrRight;
	while (ptrCurrent != NULL)
	{
		cout << ptrCurrent->arrDelay << " " << ptrCurrent->freq << endl;
		ptrCurrent = ptrCurrent -> ptrNext;
	}
	cout <<"End DispHist______________" <<endl;
}
