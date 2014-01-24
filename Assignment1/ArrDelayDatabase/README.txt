This is the database mehtod implemented in cpp. Although c++ is a real pain in the ass but I feel more comfortable with it than R and shell. Again since I do not have enough disk space on my virtue machine so I just test it on two *.csv files (1988.csv and 2008_August.csv). I only store the non-NA, non-empty ArrDelay into the database and discard all other fields. Nevertheless, the size of the sqlite3 database generated from these two files is already 62.0MB. The test result (displayed in the console) is shown as follows:
_______________________________________________________________________________
Database created!
Table created!
 - The 1-th file: 2008_August.csv
   Importing to database...
   1202 batches of size 500 inserted.
   Elapsed time is: 3.75
 - The 2-th file: 1988.csv
   Importing to database...
   10275 batches of size 500 inserted.
   Elapsed time is: 23.35
*****************************************
2 files processed, 
Mean value is: 6.58541
Standard deviation is: 25.3262
Median is: 1
Total elased time is: 27.1
Table dropped!
_______________________________________________________________________________

I build this project in with g++ 4.8.1 via Eclipse IDE. The version of the sqlite is 3.7.17. Don't forget to add sqlite3 library to g++ linker!

To run this project, you need to place the decompressed *.csv files in a directory called "Data" which is in the same parent directory as the project directory "ArrDelayDatabase" as follows:
_______________________________________________________________________________
$ ls -B
ArrDelayDatabase  ArrDelayFastSample  ArrDelayWelfold  Data
_______________________________________________________________________________