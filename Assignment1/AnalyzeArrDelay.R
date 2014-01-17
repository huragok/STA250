SumFile <- function(fileName, blockSize = 1)
{
	m = 0 # Mean
	v = 0 # Sample Variance
	hist = table(0) # The histogram (table)

	lenBlockCum = 0 # The total number of observation anlyzed so far
	command = sprintf("bunzip2 -c %s | cut -f15 -d ',' | grep -v ArrDelay | grep -v NA", fileName)
	connection = pipe(command, 'r')

	indBlock = 1
	
	while(length((block = scan(connection, '', blockSize, quiet = TRUE))) > 0)
	{	
		block = as.integer(block)
		lenBlock = length(block) # The actual size of the current block
		mBlock = mean(block)
		vBlock = (lenBlock - 1) / lenBlock * var(block) # Sample variance of the current block
		histBlock = table(block)

		delta = mBlock - m
		wBlock = lenBlock / (lenBlockCum + lenBlock)
		wBlockCum = 1 - wBlock

		m = wBlock * mBlock + wBlockCum * m # Update the mean
		v = wBlock * vBlock + wBlockCum * v + wBlock * wBlock * (delta ^ 2) # Update the sample variance
		ind = intersect(names(hist), names(histBlock)) # The intersection of the set of ArrDelay in the new block and the old blocks
		hist = c(hist[!(names(hist) %in% ind)], histBlock[!(names(histBlock) %in% ind)], hist[ind] + histBlock[ind]) # Combine two histograms
		
		lenBlockCum = lenBlockCum + lenBlock
		print(indBlock)
		indBlock = indBlock + 1
	}
	close(connection)

	hist["0"] = hist["0"] - table(0)
	fileSummary = list(m = m, v = v, hist = hist, lenFile = lenBlockCum)
	return(fileSummary)
}

BLOCKSIZE = 50000

fileNameVec = list.files(pattern = "^[12].+csv.bz2")
numFile = length(fileNameVec)

mCum = 0 # The mean value of the files processed so far
vCum = 0 # The sample variance of the files processed so far
histCum = 0 # The histogram of the files processed so far
lenCum = 0 # Total number of observations in the files processed so far

mIter = vector(mode = "numeric", length = numFile)
vIter = vector(mode = "numeric", length = numFile)
indIter = 1

for (fileName in fileNameVec)
{
	sumFile = SumFile(fileName, BLOCKSIZE)
	
	delta = sumFile$m - mCum
	wFile = sumFile$lenFile / (sumFile$lenFile + lenCum)
	wFileCum = 1 - wFile

	mCum = wFile * sumFile$m + wFileCum * mCum # Update the mean
	vCum = wFile * sumFile$v + wFileCum * vCum + wFile * wFileCum * (delta ^ 2) # Update the sample variance
	ind = intersect(names(histCum), names(sumFile$hist)) # The intersection of the set of ArrDelay in the new file and the processed files
	histCum = c(histCum[!(names(histCum) %in% ind)], sumFile$hist[!(names(sumFile$hist) %in% ind)], histCum[ind] + sumFile$hist[ind]) # Combine two histograms

	mIter[indIter] = mCum
	vIter[indIter] = vCum

	lenCum = lenCum + sumFile$lenFile
	indIter = indIter + 1

	print(cat(fileName, " has been processed!"));
	print(mCum)
	print(vCum)
}

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
print(median)

indFile = seq(1, numFile)
plot(indFile, mIter)
plot(indFile, vIter)