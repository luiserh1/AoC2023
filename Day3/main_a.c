#include <math.h>
#include <stdio.h>
#include <stdlib.h>

FILE* readFile(const char* filename)
{
	// Open the file in read mode
	FILE* fptr = fopen(filename, "r");
	return fptr;
}

char* allocateAndFillBuffer(FILE* fptr, long *l)
{
	char* buffer = 0;
	fseek(fptr, 0, SEEK_END);
	long length = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	buffer = malloc(length);
	if (buffer)
		fread(buffer, 1, length, fptr);
	fclose(fptr);
	*l = length;
	return buffer;
}

static inline long getIndexFor2DMatrix(long i, long j, long rowLength)
{
	return i * rowLength + j;
}

// Each line is a different row
// @param buffer contains the values read from the file
// @param length is the size of the buffer
// @param [out] numRows is the amount of new lines in the buffer.
// @param [out] rowLength is the amount of characters per row in the matrix.
// @note All the rows are assumed to have the same length
char* allocate2DMatrixFromBuffer(char* buffer, long length, long* numRowsPtr, long* rowLengthPtr)
{
	// Searching for a line break
	long charI;
	for (charI = 0; charI < length; charI++)
		if (buffer[charI] == '\r' || buffer[charI] == '\n')
			break;
	long rowLength = charI;
	long charsUntilLineBreak = rowLength + 1; // line break included
	if (buffer[charI] == '\r')
		charsUntilLineBreak++;
	long numRows = length / charsUntilLineBreak;
	long lastLineNumChars = length % charsUntilLineBreak;
	if (lastLineNumChars >= rowLength) // the document end is no like nd of the lines
		numRows++;

	// Filling the matrix with the relevant data
	char* matrix = (char*)malloc(numRows * rowLength * sizeof(char));
	for (charI = 0; charI < length; charI++)
	{
		long i = charI / charsUntilLineBreak;
		long j = charI % charsUntilLineBreak;
		if (j < rowLength) // this omits '\r' ad '\n'
			matrix[getIndexFor2DMatrix(i, j, rowLength)] = buffer[charI];
	}
	*rowLengthPtr = rowLength;
	*numRowsPtr = numRows;
	return matrix;
}

static inline int isDigit(char c)
{
	if ((int)c > 47 && (int)c < 58)
		return 1;

	return 0;
}

static inline int isSymbol(char c)
{
	if ((int)c > 34 && (int)c < 46) // Remember: '.' is excluded
		return 1;

	return 0;
}

static inline int intValueOfChar(char digit)
{
	return (int)digit - 48;
}

void print2DMatrix(char* mat, long numRows, long rowLength)
{
	for (long i = 0; i < numRows; i++)
	{
		for (long j = 0; j < rowLength; j++)
			printf("%c", mat[getIndexFor2DMatrix(i, j, rowLength)]);
		printf("\n");
	}
}

void printIsSymbolIn2DMatrix(char* mat, long numRows, long rowLength)
{
	for (long i = 0; i < numRows; i++)
	{
		for (long j = 0; j < rowLength; j++)
			if (isSymbol(mat[getIndexFor2DMatrix(i, j, rowLength)]))
				printf("%c", 'O');
			else
				printf("%c", '-');
		printf("\n");
	}
}

int main(int argc, const char* argv[])
{
	//// Argument parsing and file reading
	
	// The program accepts only one argument:
	// - the psth of the file to read
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	// Reading the example data file
	const char* filename = argv[1];
	FILE* fptr = readFile(filename);
	if (fptr == NULL)
	{
		printf("Cannot open file %s\n", filename);
		return 2;
	}
	// Store content of the file
	long length;
	char* buffer = allocateAndFillBuffer(fptr, &length);
	if (buffer == NULL)
	{
		printf("Cannot read file %s\n", filename);
		return 3;
	}

	//// Work

	// Content to 2D Matrix
	long numRows, rowLength;
	char* mat = allocate2DMatrixFromBuffer(buffer, length, &numRows, &rowLength);

	// Debug printing
	printf("Matrix after parsing\n");
	print2DMatrix(mat, numRows, rowLength);
	printf("Matrix where the position of the symbols is highlighted\n");
	printIsSymbolIn2DMatrix(mat, numRows, rowLength);

	// Computing the result

	int sum = 0;
	// modifies: sum, mat
	int sumNumberAtPosAndRemoveFromMatrix(long i, long j)
	{
		long matIndex = getIndexFor2DMatrix(i, j, rowLength);
		if (!isDigit(mat[matIndex]))
			return 0;

		long charJ = j + 1;
		while (charJ < rowLength)
		{
			matIndex = getIndexFor2DMatrix(i, charJ, rowLength);
			char currentChar = mat[matIndex];
			if (!isDigit(currentChar))
				break;

			charJ++;
		}

		charJ--;
		matIndex = getIndexFor2DMatrix(i, charJ, rowLength);
		char currentChar = mat[matIndex];
		int res = 0;
		int digitPos = 0;
		while (isDigit(currentChar))
		{
			// sum
			int dig = intValueOfChar(currentChar);
			int digSum = dig * pow(10, digitPos);
			res += digSum;

			// remove
			matIndex = getIndexFor2DMatrix(i, charJ, rowLength);
			mat[matIndex] = '.';

			// update loop variables
			digitPos++;
			charJ--;
			matIndex--;
			currentChar = mat[matIndex];
		}

		return res;
	}

	for (long i = 0; i < numRows; i++)
	{
		for (long j = 0; j < rowLength; j++)
		{
			if (isSymbol(mat[getIndexFor2DMatrix(i, j, rowLength)]))
			{
				// Top Left
				if (i > 0 && j > 0)
					sum += sumNumberAtPosAndRemoveFromMatrix(i - 1, j - 1);
				// Top
				if (i > 0)
					sum += sumNumberAtPosAndRemoveFromMatrix(i - 1, j);
				// Top right
				if (i > 0 && j < rowLength - 1)
					sum += sumNumberAtPosAndRemoveFromMatrix(i - 1, j + 1);
				// Right
				if (j < rowLength - 1)
					sum += sumNumberAtPosAndRemoveFromMatrix(i, j + 1);
				// Bottom right
				if (i < numRows - 1 && j < rowLength - 1)
					sum += sumNumberAtPosAndRemoveFromMatrix(i + 1, j + 1);
				// Bottom
				if (i < numRows - 1)
					sum += sumNumberAtPosAndRemoveFromMatrix(i + 1, j);
				// Bottom left
				if (i < numRows - 1 && j > 0)
					sum += sumNumberAtPosAndRemoveFromMatrix(i + 1, j - 1);
				// Left
				if (j > 0)
					sum += sumNumberAtPosAndRemoveFromMatrix(i, j - 1);
			}
		}
	}

	// Print result

	printf("Matrix after replacing the digits of valid numbers by dots\n");
	print2DMatrix(mat, numRows, rowLength);

	printf("\nThe result is: %d\n", sum);

	//// Cleanup

	// Free allocated memory
	free(buffer);

	return 0;
}
