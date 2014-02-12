library(parallel)

setwd("/home/wenhao/Documents/Wenhao Wu/UC Davis/2014 Winter/STA 250/Assignment2/Data")
fileVec = list.files(pattern = "*.csv")
argList = as.list(fileVec)
for (i in 1:length(fileVec)) {
	argList[[i]] = list(fileName = fileVec[i], blockSize = 20000)
}


funFreqTable <- function(arg) {
	hist <- table(0) # The histogram (table)

	headerFields <- unlist(strsplit(readLines(arg$fileName, n=1), split=","))
	col <- which(headerFields == "ArrDelay")
	if (length(col) == 0) {
		col <- which(headerFields == "\"ARR_DELAY\"")
	}
	
	cmd = sprintf("cat %s | cut -f%d -d ',' | grep -v ArrDelay | grep -v \"ARR_DELAY\" | grep -v NA", arg$fileName, col)
	connection = pipe(cmd, 'r')

	indBlock <- 1
	while(length((block = scan(connection, '', arg$blockSize, quiet = TRUE))) > 0)
	{	
		block = as.integer(block)
		lenBlock = length(block) # The actual size of the current block
		histBlock = table(block)

		ind = intersect(names(hist), names(histBlock)) # The intersection of the set of ArrDelay in the new block and the old blocks
		hist = c(hist[!(names(hist) %in% ind)], histBlock[!(names(histBlock) %in% ind)], hist[ind] + histBlock[ind]) # Combine two histograms
		
		indBlock = indBlock + 1
	}
	close(connection)
	print(paste("File ", arg$fileName, " processed!"))
	return(hist)
}

start <- proc.time()
#histList = lapply(argList, funFreqTable)
histList = mclapply(argList, funFreqTable, mc.cores = 1, mc.preschedule = FALSE)

histCum = 0 # The histogram of the files processed so far
for (hist in histList)
{
	ind = intersect(names(histCum), names(hist)) # The intersection of the set of ArrDelay in the new file and the processed files
	histCum = c(histCum[!(names(histCum) %in% ind)], hist[!(names(hist) %in% ind)], histCum[ind] + hist[ind]) # Combine two histograms
}

# Evaluate the mean and variance
freq = as.integer(histCum)
value = as.integer(names(histCum))
total = sum(freq)
prob = freq / total

mean = sum(value * prob)
SD = sqrt(sum(value ^ 2 * prob) - mean ^ 2)

# Evaluate the median
histCum["0"] = histCum["0"] - table(0)
histCum = histCum[order(as.integer(names(histCum)))]
value = as.integer(names(histCum))
count = unname(histCum)
total = sum(count)
cumSumCount = c(0,cumsum(count))
median = NA
for (i in 1 : length(histCum))
{
	if (cumSumCount[i] < total/2 && cumSumCount[i + 1] >= total/2)
	{
		median = value[i + 1]
	}
}

time = proc.time() - start

print("*****************************************")
print(paste("Mean value is: ", mean))
print(paste("Standard deviation is: ", SD))
print(paste("Median is: ", median))
print(time)

result = list(time = time, results = c(mean = mean, median = median, sd = SD),
     system = Sys.info(),  session = sessionInfo(),processor = "Intel(R) Core(TM) i7-3612QM CPU @ 2.10GHz 2.10GHz",
     memory = "8.00GB", hard_drive = "1TB at 5400rpm", virtual_machine = "VirtualBox v4.3.6")

save(result, file="result.rda")



