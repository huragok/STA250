#include <iostream>
#include <iomanip>
#include <random>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>


#include "hist.h"

#define MAX_CONTENTLINE 80 // Maximum of each line in the content of the tar file
#define MAX_FILELINE 2000 // Maximum length of each line in the csv file
#define PROBSAMPLE 0.0002 // Each line is sampled with this probability
#define DELIMITER ',' //

using namespace std;

int GetCountDeli(char *firstLine, char delimiter, char *fieldName1, char *fieldName2);

int main()
{
	FILE *contentStream = NULL, *fileStream = NULL;
	char lineContent[MAX_CONTENTLINE]; // char array for one line in the content of the .tar file
	char lineFile[MAX_FILELINE]; // char array for one line in the .csv file
	int countFile = 0, countLine = 0, countSample = 0; // Number of files in the tar file, number of lines in each csv file, and number of samples from each csv file
	char cmdContent[] = "find ../Data -type f -iname \"*.csv\" "; // Shell command to extract the content from the tar file
	char fieldName1[] = "ArrDelay", fieldName2[] = "ARR_DELAY"; // The 2 possible field names
	default_random_engine generator; // Need to use the g++ compiler option -std=gnu++11. My compiler is g++ 4.8.1
	bernoulli_distribution distribution(PROBSAMPLE);
	hist freqTable; // Frequency table

	struct timespec start, finish;
	double elapsed;
	int startCPU,finishCPU;
	double elapsedCPU;

	clock_gettime(CLOCK_MONOTONIC, &start);
	startCPU = clock();

	if((contentStream = popen(cmdContent, "r")) == NULL) //List the filenames in the tar file
	{
		cout<<"Cannot find .csv files!"<<endl;
		return 1;
	}

	while(fgets(lineContent, MAX_CONTENTLINE, contentStream) != NULL) //For each csv file
	{
		countFile++;
		countLine = 0;
		countSample = 0;
		//cout << lineContent;

		char *ptrTemp= NULL;
		if ((ptrTemp = strchr(lineContent, '\n')) != NULL) //Delete '\n' at the end of the line
		{
			*ptrTemp= '\0';
		}

		cout << " - The " << countFile << "-th file: "<< lineContent + 8 << endl;// Output the file name

		/* Extract the file*/
		cout << "   Sampling..." << endl;

		fileStream = fopen(lineContent, "r"); //read the file

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
			countLine++;
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
					    	if (strcmp(ptrArrDelay, "NA")   && ptrArrDelay != (lineFile + i)) //If the content is not "NA"
					    	{
					    		countSample++;
					    		int arrDelay = atoi(ptrArrDelay);
					    		//cout << arrDelay << endl;
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

	pclose(contentStream);

	double mean = freqTable.mean(); // We use the hist class to evaluate the mean, standard deviation and the median
	double stdev = freqTable.stdev();
	int median = freqTable.median();

	clock_gettime(CLOCK_MONOTONIC, &finish);
	finishCPU = clock();

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	elapsedCPU = (double)(finishCPU - startCPU) / CLOCKS_PER_SEC;

	cout << "*****************************************" << endl;
	cout << countFile << " files processed, " << endl;
	cout << (freqTable.numLeft + freqTable.numRight) <<" samples grouped into " << freqTable.numEntry << " entries," <<endl;
	cout << "Mean value is: " << mean << endl;
	cout << "Standard deviation is: " << stdev << endl;
	cout << "Median is: " << median << endl;
	cout << "Wall-clock time is: " << elapsed <<endl;
	cout << "CPU time is:" << elapsedCPU << endl;

	return(0);
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
