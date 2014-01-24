#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <math.h>
#include <ctime>

#define MAX_CONTENTLINE 80 // Maximum of each line in the content of the tar file
#define MAX_FILELINE 2000 // Maximum length of each line in the csv file
#define DELIMITER ',' //

using namespace std;

static int callback(void *singleValue, int argc, char **argv, char **azColName); //  return a single value from sqlite3 command
int GetCountDeli(char *firstLine, char delimiter, char *fieldName1, char *fieldName2); // Function used to count the number of delimiters in front of the desired field
char *BuildBatchInsert(int n, const int *batch, int maxLenEntry); // return a sqlite3 insert command string to insert n values from batch

int main()
{
	int countFile = 0; // Number of files processed
	FILE *contentStream = NULL; // stream to read the content of *.csv files in the data directory
	char lineContent[MAX_CONTENTLINE]; // char array for one line in the content of the .tar file
	char cmdContent[] = "find ../Data -type f -iname \"*.csv\" "; // Shell command to extract the content from the tar file

	FILE *fileStream = NULL; // stream to read each *.csv file
	char lineFile[MAX_FILELINE]; // char array for one line in the .csv file
	char fieldName1[] = "ArrDelay", fieldName2[] = "ARR_DELAY"; // The 2 possible field names

	sqlite3 *db;
	int rc;
	char *zErrMsg = NULL;
	char sqlCreateTable[] = "CREATE TABLE FLIGHTINFO(\n	ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \n ARRDELAY INTEGER NOT NULL);";
	char *sqlInsert= NULL ; //batch inserting sql command
	char sqlAvg[] = "SELECT AVG(ARRDELAY)\nFROM FLIGHTINFO;"; // sql command to compute mean
	char sqlAvgSqr[] = "SELECT AVG(ARRDELAY * ARRDELAY)\nFROM FLIGHTINFO;"; // sql command to compute mean square
	char sqlMedian[] = "SELECT AVG(ARRDELAY)\n"\
					   "FROM (SELECT ARRDELAY\n"\
					         "FROM FLIGHTINFO\n"\
					         "ORDER BY ARRDELAY\n"\
					         "LIMIT 2 - (SELECT COUNT(*) FROM FLIGHTINFO) % 2\n"\
					         "OFFSET (SELECT (COUNT(*) - 1) / 2\n"\
					         "FROM FLIGHTINFO))"; // sql command to compute median

	char sqlDropTable[] = "DROP TABLE FLIGHTINFO;"; // sql command to drop table
	int maxLenEntry = 6; // Maximum length of the string corresponding to each ArrDelay
	int maxLenBatch = 500; // Max number of records to insert in each batch. In my computer, 1000 won't work.
	int *batch = new int[maxLenBatch]; // Array to store the batch of ArrDelay to be inserted
	int countBatch = 0; // Number of batches inserted in current file

	clock_t start;
	double t1, t2;

	/* create a database */
	rc = sqlite3_open("flightInfoTemp.db", &db);
	if(rc != 0)
	{
	      cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
	      exit(0);
	}
	else
	{
		cout << "Database created!" << endl;
	}

	/* Create a table */
	rc = sqlite3_exec(db, sqlCreateTable, NULL, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		cerr << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		cout << "Table created!" << endl;
	}


	if((contentStream = popen(cmdContent, "r")) == NULL) //List the filenames in the tar file
	{
		cout<<"Cannot find .csv files!"<<endl;
		return 1;
	}
	start = clock();
	t1 = start;
	while(fgets(lineContent, MAX_CONTENTLINE, contentStream) != NULL) //For each csv file
	{
		countBatch = 0;
		countFile++;
		char *ptrTemp= NULL;
		if ((ptrTemp = strchr(lineContent, '\n')) != NULL) //Delete '\n' at the end of the line
		{
			*ptrTemp= '\0';
		}
		cout << " - The " << countFile << "-th file: "<< lineContent + 8 << endl;// Output the file name
		cout << "   Importing to database..." << endl;

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

		int countLine = 0;
		int indEntry = 0;

		while (fgets(lineFile, MAX_FILELINE, fileStream) != NULL) // For each line of the file
		{
			countLine++;

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
						if (strcmp(ptrArrDelay, "NA") && ptrArrDelay != (lineFile + i)) //If the content is not "NA" or empty
						{

							indEntry++;
							batch[indEntry - 1] = atoi(ptrArrDelay); // add the ArrDelay to the batch

							if (indEntry == maxLenBatch) // When the batch is full
							{
								countBatch++ ;
								sqlInsert = BuildBatchInsert(maxLenBatch, batch, maxLenEntry); // Build the batch inserting command
								//cout << sqlInsert << endl;
								rc = sqlite3_exec(db, sqlInsert, NULL, 0, &zErrMsg); // Insert the batch
								if( rc != SQLITE_OK )
								{
									cerr << "   " << countLine<<" SQL error: " << zErrMsg << endl;
									sqlite3_free(zErrMsg);
								}
								indEntry = 0;
								//cout << "   Batch " << countBatch << " inserted." << endl;
								delete [] sqlInsert;
							}
						}
						break;
					}
				}
				i++;
			}
		}
		if (indEntry > 0) // When the file scanned to the bottom, insert the rest of the record in the batch to the table
		{
			countBatch++ ;
			sqlInsert = BuildBatchInsert(indEntry, batch, maxLenEntry); // Build the batch inserting command
			//cout << sqlInsert << endl;

			rc = sqlite3_exec(db, sqlInsert, NULL, 0, &zErrMsg); // Insert the batch
			if( rc != SQLITE_OK )
			{
				cerr << "   " << countLine<<" SQL error: " << zErrMsg << endl;
				sqlite3_free(zErrMsg);
			}
			//cout << "   Batch " << countBatch << " inserted." << endl;
			delete [] sqlInsert;
		}

		t2 = clock();

		cout << "   " << countBatch << " batches of size " << maxLenBatch << " inserted." << endl;
		cout << "   " << "Elapsed time is: " << (t2 - t1) / (double) CLOCKS_PER_SEC << endl;
		t1 = t2;
	}
	pclose(contentStream);

	delete [] batch;

	/* Evaluate the mean, variance and the median */
	cout << "*****************************************" << endl;
	cout << countFile << " files processed, " << endl;
	double mean = 0;
	rc = sqlite3_exec(db, sqlAvg, callback, (void *)&mean, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		cerr << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		cout << "Mean value is: " << mean << endl;
	}

	double meanSqr = 0;
	rc = sqlite3_exec(db, sqlAvgSqr, callback, (void *)&meanSqr, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		cerr << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		cout << "Standard deviation is: " << sqrt(meanSqr - mean * mean) << endl;
	}

	double median = 0;
	rc = sqlite3_exec(db, sqlMedian, callback, (void *)&median, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		cerr << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		cout << "Median is: " << median << endl;
	}

	cout << "Total elased time is: " << (t2 - start) / (double) CLOCKS_PER_SEC << endl;

	rc = sqlite3_exec(db, sqlDropTable, NULL, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		cerr << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		cout << "Table dropped!" << endl;
	}

	sqlite3_close(db);
	return(0);
}

static int callback(void *singleValue, int argc, char **argv, char **azColName)
{
	*((double *)singleValue) = atof(argv[0]);
	return 0;
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

/* Here we make use of this feature coming with sqlite 3.7.11+ to insert a batch of multiple records in one command,
huge performance gain! */
char *BuildBatchInsert(int n, const int *batch, int maxLenEntry)
{
	char *sqlBatchInsert = new char[40 + n * (4 + maxLenEntry)];
	int lenEntry = 0;
	char* lineHead = NULL;

	strcpy(sqlBatchInsert, "INSERT INTO FLIGHTINFO(ARRDELAY) VALUES\n");

	lineHead = sqlBatchInsert + 40;
	for (int i = 0; i < n; i++)
	{
		*lineHead = '(';
		lenEntry = sprintf(lineHead + 1, "%d", batch[i]);
		strcpy(lineHead + 1 + lenEntry, "),\n");
		lineHead += (lenEntry + 4);
	}
	*(lineHead - 2) = ';';
	*(lineHead - 1) = '\0';

	return sqlBatchInsert;
}
