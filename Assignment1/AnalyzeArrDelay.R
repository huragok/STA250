SumFile <- function(fileName, blockSize = 1)
{
	m = 0 # Mean
	v = 0 # Sample Variance

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
		
		delta = mBlock - m
		wBlock = lenBlock / (lenBlockCum + lenBlock)
		wBlockCum = 1 - wBlock

		m = wBlock * mBlock + wBlockCum * m
		v = wBlock * vBlock + wBlockCum * v + wBlock * wBlock * (delta ^ 2)
		
		lenBlockCum = lenBlockCum + lenBlock
		print(indBlock)
		indBlock = indBlock + 1
	}
	close(connection)
	fileSummary = list(m = m, v = v, lenFile = lenBlockCum)
	return(fileSummary)
}

BLOCKSIZE = 50000

fileNameVec = list.files(pattern = "^[12].+csv.bz2")
numFile = length(fileNameVec)

mCum = 0 # The mean value of the files processed so far
vCum = 0 # The sample variance of the files processed so far
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

	mCum = wFile * sumFile$m + wFileCum * mCum
	vCum = wFile * sumFile$v + wFileCum * vCum + wFile * wFileCum * (delta ^ 2)

	mIter[indIter] = mCum
	vIter[indIter] = vCum

	lenCum = lenCum + sumFile$lenFile
	indIter = indIter + 1

	print(cat(fileName, " has been processed!"));
	print(mCum)
	print(vCum)
}

indFile = seq(1, numFile)
plot(indFile, mIter)
plot(indFile, vIter)