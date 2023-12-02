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

void printBuffer(char* buffer)
{
	printf("%s\n", buffer);
}


int intValueOfChar(char digit)
{
	return (int)digit - 48;
}

int concatenateAndSumDigits(char first, char second)
{
	int firstValue = intValueOfChar(first);
	int lastValue  = intValueOfChar(second);
	return firstValue * 10 + lastValue;
}

int main(int argc, const char* argv[])
{
	//// Argument parsing and file reading
	
	// The program accepts only one argument:
	// - the psth of the file to read
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	// Reading the example data file
	const char* filename = argv[1];
	FILE* fptr = readFile(filename);
	if (fptr == NULL)
	{
		printf("Cannot open file %s\n", filename);
		return 0;
	}
	// Store content of the file
	long length;
	char* buffer = allocateAndFillBuffer(fptr, &length);
	if (buffer == NULL)
	{
		printf("Cannot read file %s\n", filename);
		return 0;
	}

	//// Work

	// Reading characters from the buffer
	// Storing every first and last digit before a new line
	// Then adding first * 10 + last to a sum
	int sum = 0;
	char firstDigit = '\0';
	char lastDigit  = '\0';
	for (int i = 0; i < length - 1; i++)
	{
		if (buffer[i] == '\n')
		{
			sum       += concatenateAndSumDigits(firstDigit, lastDigit);
			firstDigit = '\0';
			lastDigit  = '\0';
		}
		else if ((int)buffer[i] > 47 && (int)buffer[i] < 58)
		{
			if (firstDigit == '\0')
				firstDigit = buffer[i];
			lastDigit = buffer[i];
		}
	}
	sum += concatenateAndSumDigits(firstDigit, lastDigit);

	// Print result
	printf("The result is: %d\n", sum);
	
	//// Cleanup

	// Free allocated memory
	free(buffer);

	return 0;
}
