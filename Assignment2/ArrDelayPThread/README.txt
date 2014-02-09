This is the fast sampling mehtod implemented in cpp with pthread. I implemented it on my virtual box machine which doesn't support multicore, and tested it on a small set of 11 *.csv files. The test result (displayed in the console) is shown as follows:
_______________________________________________________________________________
Thread 3 is processing ../Data/2008_August.csv
Thread 2 is processing ../Data/1988.csv
Thread 1 is processing ../Data/1991.csv
Thread 0 is processing ../Data/1990.csv
Thread 3 is processing ../Data/1992.csv
Thread 2 is processing ../Data/1994.csv
Thread 1 is processing ../Data/1993.csv
Thread 0 is terminated
Thread 3 is terminated
Thread 2 is terminated
Thread 1 is terminated
*****************************************
6238 samples grouped into 199 entries,
Mean value is: 5.30747
Standard deviation is: 21.713
Median is: 1
Wall-clock time is: 289.074
CPU time is:23.36
_______________________________________________________________________________

On the contrary, when we run the non-threaded fastSample algorithm with same parameters and files, the result looks like this

_______________________________________________________________________________

 - The 1-th file: 2008_August.csv
   Sampling...
 - The 2-th file: 1988.csv
   Sampling...
 - The 3-th file: 1991.csv
   Sampling...
 - The 4-th file: 1990.csv
   Sampling...
 - The 5-th file: 1992.csv
   Sampling...
 - The 6-th file: 1994.csv
   Sampling...
 - The 7-th file: 1993.csv
   Sampling...
*****************************************
7 files processed, 
6237 samples grouped into 183 entries,
Mean value is: 5.51515
Standard deviation is: 23.7742
Median is: 0
Wall-clock time is: 59.2904
CPU time is:16.62
_______________________________________________________________________________

It seems that since I am using a task pool model with 4 working threads, the speed is 4 times slower! The problem is that the main bottle neck is the hard disk reading speed. On my pathetic computer there is no SSD and I am using an embarassing 5400rpm hard disk. Also currently I am running this program on virtualBox which doesn't support multicore yet. 

I build this project in with g++ 4.8.1 via Eclipse IDE. Since I used the default_random_engine from <random>, i need to use the g++ compiler option -std=gnu++11.

To run this project, you need to place the decompressed *.csv files in a directory called "Data" which is in the same parent directory as the project directory "ArrDelayPThread" as follows:

_______________________________________________________________________________
$ ls -B
ArrDelayPThread  Data
_______________________________________________________________________________