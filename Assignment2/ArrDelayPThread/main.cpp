#include <iostream>
#include <iomanip>
#include <random>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


#include "hist.h"

using namespace std;

#define MAX_COUNT 4 // Maximum length of the number of files
#define MAX_CONTENTLINE 80 // Maximum of each line in the content of the tar file
#define MAX_FILELINE 2000 // Maximum length of each line in the csv file
#define PROBSAMPLE 0.0002 // Each line is sampled with this probability
#define DELIMITER ',' //
#define NUM_THREADS     4

int GetCountDeli(char *firstLine, char delimiter, char *fieldName1, char *fieldName2);
void InitData(struct Data *);
static void *ProcessFile(void *);

struct Data
{
	char** fileName;
	bool* flagProcessed;
	int countFile, countProcessed;
	hist freqTable; // Frequency table

	pthread_mutex_t accessFileName;
	pthread_mutex_t accessFreqTable;
};

struct ArgChildThread
{
	struct Data *data;
	int ID;
};


int main()
{
	struct Data data;
	struct ArgChildThread arg[NUM_THREADS];
	pthread_attr_t attr;
	//size_t stacksize;

	InitData(&data);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_t childThread[NUM_THREADS];

	//pthread_attr_getstacksize (&attr, &stacksize);
	//cout << stacksize << endl;

	struct timespec start, finish;
	double elapsed;
	int startCPU,finishCPU;
	double elapsedCPU;

	clock_gettime(CLOCK_MONOTONIC, &start);
	startCPU = clock();

	for(int i = 0; i < NUM_THREADS; i++)
	{
		arg[i].data = &data;
		arg[i].ID = i;
		if (pthread_create(&childThread[i], &attr, ProcessFile, (void *)(&arg[i])))
		{
			perror("pthread_create fail!");
		}
	}

	pthread_attr_destroy(&attr);

	for(int i = 0; i < NUM_THREADS; i++)
	{
		if(pthread_join(childThread[i], NULL))
		{
			perror("pthread_join fail!");
		}
	}

	double mean = data.freqTable.mean(); // We use the hist class to evaluate the mean, standard deviation and the median
	double stdev = data.freqTable.stdev();
	int median = data.freqTable.median();

	clock_gettime(CLOCK_MONOTONIC, &finish);
	finishCPU = clock();

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	elapsedCPU = (double)(finishCPU - startCPU) / CLOCKS_PER_SEC;

	cout << "*****************************************" << endl;
	cout << (data.freqTable.numLeft + data.freqTable.numRight) <<" samples grouped into " << data.freqTable.numEntry << " entries," <<endl;
	cout << "Mean value is: " << mean << endl;
	cout << "Standard deviation is: " << stdev << endl;
	cout << "Median is: " << median << endl;
	cout << "Wall-clock time is: " << elapsed <<endl;
	cout << "CPU time is:" << elapsedCPU << endl;

	pthread_exit(NULL);
}

int GetCountDeli(char *firstLine, char delimiter, char *fieldName1, char *fieldName2)
{
	int countDeli = 0;
	char *ptrField = strstr(firstLine, fieldName1); // Find the pointer to the field in the first line
	if (ptrField == NULL)
	{
		ptrField = strstr(firstLine, fieldName2);  // Try another field name

	}
	if (ptrField != NULL)
	{
		for (char *ptrChar = firstLine; ptrChar <= ptrField; ptrChar++)
		{
			if (*ptrChar == delimiter)
			{
				countDeli ++;
			}
		}
	}
	return countDeli;
}

void InitData(struct Data *ptrData)
{
	ptrData->countProcessed = 0;
	if (pthread_mutex_init(&(ptrData->accessFileName), NULL))
	{
		perror("pthread_mutex_init: ");
		return;
	}
	if (pthread_mutex_init(&(ptrData->accessFreqTable), NULL))
	{
		perror("pthread_mutex_init: ");
		return;
	}

	FILE *countStream = NULL;
	char lineCount[MAX_COUNT];
	char cmdCount[] = "find ../Data -type f -iname \"*.csv\" | wc -l"; // Shell command to count the number of files

	FILE *contentStream = NULL;
	char cmdContent[] = "find ../Data -type f -iname \"*.csv\" "; // Shell command to extract the content from the tar file


	if((countStream = popen(cmdCount, "r")) == NULL) //List the filenames in the tar file
	{
		cout<<"Cannot find .csv files!"<<endl;
		exit(1);
	}
	if (fgets(lineCount, MAX_COUNT, countStream) != NULL)
	{
		ptrData->countFile = atoi(lineCount);
	}
	pclose(countStream);

	if((contentStream = popen(cmdContent, "r")) == NULL) //List the filenames in the tar file
	{
		cout<<"Cannot find .csv files!"<<endl;
		exit(1);
	}

	if ((ptrData->flagProcessed = new bool[ptrData->countFile]) == NULL)
	{
		cout << "New failed!" << endl;
		exit(1);
	}
	for (int i = 0; i < ptrData->countFile; i++)
	{
		ptrData->flagProcessed[i] = false;
	}

	if ((ptrData->fileName = new char*[ptrData->countFile]) == NULL)
	{
		cout << "New failed!" << endl;
		exit(1);
	}
	for (int i = 0; i < ptrData->countFile; i++)
	{
		if ((ptrData->fileName[i] = new char[MAX_CONTENTLINE]) == NULL)
		{
			cout << "New failed!" << endl;
			exit(1);
		}
		if (fgets(ptrData->fileName[i], MAX_CONTENTLINE, contentStream) == NULL)
		{
			cout << "Read file name failed!";
			exit(1);
		}

	}
	pclose(contentStream);
	return;
}

static void *ProcessFile(void *ptrArg)
{
	struct Data *ptrData = ((struct ArgChildThread *)ptrArg)->data;
	FILE *fileStream = NULL;
	char fieldName1[] = "ArrDelay", fieldName2[] = "ARR_DELAY"; // The 2 possible field names
	char lineFile[MAX_FILELINE]; // char array for one line in the .csv file
	default_random_engine generator; // Need to use the g++ compiler option -std=gnu++11. My compiler is g++ 4.8.1
	bernoulli_distribution distribution(PROBSAMPLE);

	hist freqTable; // Frequency table
	while(1)
	{

		// Find a file name
		char *fileName = NULL;
		if (pthread_mutex_lock(&(ptrData->accessFileName)))
		{
			perror("pthread_mutex_lock: ");
			return NULL;
		}

		for (int i = 0; i < ptrData->countFile; i++)
		{
			if (!(ptrData->flagProcessed[i]))
			{
				fileName = ptrData->fileName[i];
				ptrData->flagProcessed[i] = true;
				break;
			}
		}

		if (pthread_mutex_unlock(&(ptrData->accessFileName)))
		{
			perror("pthread_mutex_unlock: ");
			return NULL;
		}

		if (fileName == NULL)
		{
			break;
		}

		char *ptrTemp= NULL;
		if ((ptrTemp = strchr(fileName, '\n')) != NULL) //Delete '\n' at the end of the line
		{
			*ptrTemp= '\0';
		}
		cout <<  "Thread " <<((struct ArgChildThread *)ptrArg)->ID << " is processing " << fileName << endl;

		fileStream = fopen(fileName, "r"); //read the file
		int countDeli = 0; // The number of delimiters before the field

		if (fgets(lineFile, MAX_FILELINE, fileStream) != NULL) //Get the first line, locate the field of ArrDelay
		{
			countDeli = GetCountDeli(lineFile, DELIMITER, fieldName1, fieldName2);
			if (countDeli == 0)
			{
				cout << "   Cannot find the field, please check the file." << endl;
				continue;
			}
		}
		else // If cannot, just skip this stupid file
		{
			cout << "   Cannot open this file!" << endl;
			continue;
		}

		while (fgets(lineFile, MAX_FILELINE, fileStream) != NULL) // For each line of the file
		{
			if (distribution(generator)) // If this line is lucky enough to be sampled
			{
				int i = 0, indDeli = 0;
				char *ptrArrDelay = NULL;
				int inQuotes = 0;

				while( i < MAX_FILELINE && lineFile[i] != '\0') // This part is pretty much the same as Prof.Duncan's processLine function
				{
					if(lineFile[i] == '"')
					{
						inQuotes = inQuotes ? 0 : 1;
					}
					else if (lineFile[i] == DELIMITER && !inQuotes)
					{
						indDeli++;
						if(indDeli == countDeli)
						{
							ptrArrDelay = lineFile + i + 1;
						}
						else if(indDeli == countDeli + 1)
						{
							lineFile[i] = '\0';  //changing line.
							if (strcmp(ptrArrDelay, "NA")  && ptrArrDelay != (lineFile + i)) //If the content is not "NA"
							{
								int arrDelay = atoi(ptrArrDelay);
								freqTable.AddEntry(arrDelay); //Add this ArrDelay to the frequency table
							}
							break;
						}
					}
					i++;
				}
			}
		}
		fclose(fileStream);
	}
	if (pthread_mutex_lock(&(ptrData->accessFreqTable)))
	{
		perror("pthread_mutex_lock: ");
		return NULL;
	}

	cout <<  "Thread " <<((struct ArgChildThread *)ptrArg)->ID << " is terminated" << endl;
	ptrData->freqTable.CatHist(freqTable);

	if (pthread_mutex_unlock(&(ptrData->accessFreqTable)))
	{
		perror("pthread_mutex_unlock: ");
		return NULL;
	}

	pthread_exit(NULL);

}
