This is the frequency tab;e mehtod implemented with parallel package. I implemented it on my virtual box machine which doesn't support multicore, and tested it on a small set of *.csv files. The test result (displayed in the console) is shown as follows:
_______________________________________________________________________________
[1] "File  1988.csv  processed!"
[1] "File  1990.csv  processed!"
[1] "File  1991.csv  processed!"
[1] "File  1992.csv  processed!"
[1] "File  1993.csv  processed!"
[1] "File  1994.csv  processed!"
[1] "*****************************************"
[1] "Mean value is:  5.67355118832785"
[1] "Standard deviation is:  23.2763252292727"
[1] "Median is:  2"
   user  system elapsed 
 42.843   7.188  52.766 
_______________________________________________________________________________

So far I have just tested this program with 1 core since virtual box doesn't support multicore. To test it with more cores, just change the mc.cores argument in mclapply function.
