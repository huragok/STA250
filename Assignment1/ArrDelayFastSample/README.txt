This is the fast sampling mehtod implemented in cpp. I implemented it on my virtual box machine. Unfortunately, I allocated fixed size disk for my virtualBox so I cannot decompress all *.csv files to fully test it. Nevertheless, I have tested it on a small set of 11 *.csv files. The test result (displayed in the console) is shown as follows:
_______________________________________________________________________________
- The 1-th file: 2008_August.csv
   Sampling...
   64/612279 lines sampled
   Elapsed time is: 0.52
 - The 2-th file: 1988.csv
   Sampling...
   481/5202096 lines sampled
   Elapsed time is: 1.62
 - The 3-th file: 2008_December.csv
   Sampling...
   59/544956 lines sampled
   Elapsed time is: 0.47
 - The 4-th file: 1989.csv
   Sampling...
   501/5041200 lines sampled
   Elapsed time is: 1.56
 - The 5-th file: 1996.csv
   Sampling...
   527/5351983 lines sampled
   Elapsed time is: 1.71
 - The 6-th file: 1987.csv
   Sampling...
   124/1311826 lines sampled
   Elapsed time is: 0.41
 - The 7-th file: 2008_April.csv
   Sampling...
   65/598126 lines sampled
   Elapsed time is: 0.38
 - The 8-th file: 2008_January.csv
   Sampling...
   67/605765 lines sampled
   Elapsed time is: 0.5
 - The 9-th file: 1990.csv
   Sampling...
   491/5270893 lines sampled
   Elapsed time is: 1.64
 - The 10-th file: 2008_February.csv
   Sampling...
   68/569236 lines sampled
   Elapsed time is: 0.46
 - The 11-th file: 1997.csv
   Sampling...
   556/5411843 lines sampled
   Elapsed time is: 1.72
*****************************************
11 files processed, 
3003 samples grouped into 175 entries,
Mean value is: 8.57209
Standard deviation is: 28.9517
Median is: 2
Total elased time is: 10.99
_______________________________________________________________________________

Another problem is that I messed up with my github setup in my virtual machine so I have to copy the code files to my windows host then commit and push them from there.

I build this project in with g++ 4.8.1 via Eclipse IDE. Since I used the default_random_engine from <random>, i need to use the g++ compiler option -std=gnu++11.

To run this project, you need to place the decompressed *.csv files in a directory called "Data" which is in the same parent directory as the project directory "ArrDelayFastSample" as follows:

_______________________________________________________________________________
$ ls -B
ArrDelayDatabase  ArrDelayFastSample  ArrDelayWelfold  Data
_______________________________________________________________________________