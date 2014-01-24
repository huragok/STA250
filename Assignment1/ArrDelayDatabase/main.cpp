#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#define MAX_CONTENTLINE 80 // Maximum of each line in the content of the tar file
#define MAX_FILELINE 2000 // Maximum length of each line in the csv file
#define DELIMITER ',' //

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
int GetCountDeli(char *firstLine, char delimiter, char *fieldName1, char *fieldName2);
char *BuildBatchInsert(int n, const char *batch, int maxLenEntry);

int main()
{
	int countFile = 0;
	FILE *contentStream = NULL;
	char lineContent[MAX_CONTENTLINE]; // char array for one line in the content of the .tar file
	char cmdContent[] = "find ../Data -type f -iname \"*.csv\" "; // Shell command to extract the content from the tar file

	FILE *fileStream = NULL;
	char lineFile[MAX_FILELINE]; // char array for one line in the .csv file

	char fieldName1[] = "ArrDelay", fieldName2[] = "ARR_DELAY"; // The 2 possible field names


	sqlite3 *db;
	int rc;
	char *zErrMsg = NULL;
	char sqlCreateTable[] = "CREATE TABLE FLIGHTINFO(\n	ID INTEGER PRIMARY KEY AUTOINCREMENT, \n ARRDELAY FLOAT NOT NULL);";
	char *sqlInsert= NULL ;
	char sqlDropTable[] = "DROP TABLE FLIGHTINFO;";
	char sqlTemp[] = "INSERT INTO FLIGHTINFO(ARRDELAY) VALUES\n(-5.00;";
	int maxLenEntry = 12;
	int maxLenBatch = 50;
	char *batch = new char((maxLenEntry + 1) * maxLenBatch);
	int countBatch = 0;

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


	rc = sqlite3_exec(db, sqlCreateTable, callback, 0, &zErrMsg);
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
							strcpy(batch + (indEntry - 1)* (maxLenEntry + 1), ptrArrDelay);

							if (indEntry == maxLenBatch)
							{
								countBatch++ ;
								sqlInsert = BuildBatchInsert(maxLenBatch, batch, maxLenEntry);
								cout << sqlInsert << endl;
								rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &zErrMsg);
								if( rc != SQLITE_OK )
								{
									cerr << "   " << countLine<<" SQL error: " << zErrMsg << endl;
									sqlite3_free(zErrMsg);
								}
								indEntry = 0;
								cout << "   Batch " << countBatch << " inserted." << endl;
								delete sqlInsert;
							}
						}
						break;
					}
				}
				i++;
			}
		}
		if (indEntry > 0)
		{
			sqlInsert = BuildBatchInsert(indEntry, batch, maxLenEntry);
			//cout << countLine << endl;
			cout << sqlInsert << endl;

			rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &zErrMsg);
			if( rc != SQLITE_OK )
			{
				cerr << "   " << countLine<<" SQL error: " << zErrMsg << endl;
				sqlite3_free(zErrMsg);
			}
			indEntry = 0;
		}
	}
	pclose(contentStream);

	delete batch;

	rc = sqlite3_exec(db, sqlDropTable, callback, 0, &zErrMsg);
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

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++)
   {
	   cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
   }
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

char *BuildBatchInsert(int n, const char *batch, int maxLenEntry)
{
	char *sqlBatchInsert = new char(40 + n * (4 + maxLenEntry));
	int lenEntry = 0;
	char* lineHead = NULL;

	strcpy(sqlBatchInsert, "INSERT INTO FLIGHTINFO(ARRDELAY) VALUES\n");

	lineHead = sqlBatchInsert + 40;
	for (int i = 0; i < n; i++)
	{
		*lineHead = '(';
		strcpy(lineHead + 1, batch + i * (maxLenEntry + 1));
		lenEntry = strlen(batch + i * (maxLenEntry + 1));
		strcpy(lineHead + 1 + lenEntry, "),\n");
		lineHead += (lenEntry + 4);
	}
	*(lineHead - 2) = ';';
	*(lineHead - 1) = '\0';

	return sqlBatchInsert;
}
