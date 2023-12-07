#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline FILE* readFile(const char*);

char* allocateAndFillBuffer(FILE*, long*);
static inline void printBuffer(char*);

static inline int intValueOfChar(char);
static inline int concatenateAndSumDigits(char, char);

char getPreviousInCyclicArray(char*, long, long, long);

///////////////////////////////////////////////////////////////////////////////

static inline FILE* readFile(const char* filename)
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

static inline void printBuffer(char* buffer)
{
	printf("%s\n", buffer);
}

static inline int intValueOfChar(char digit)
{
	return (int)digit - 48;
}

static inline int concatenateAndSumDigits(char first, char second)
{
	int firstValue = intValueOfChar(first);
	int lastValue  = intValueOfChar(second);
	return firstValue * 10 + lastValue;
}

// Assuming prev < length
char getPreviousInCyclicArray(char* arr, long length, long currentIndex, long prev)
{
	long prevIndex;
	if (prev > currentIndex)
		prevIndex = length - (prev - currentIndex);
	else
		prevIndex = currentIndex - prev;

	return arr[prevIndex];
}

int main(int argc, const char* argv[])
{
	//// Argument parsing and file reading
	
	// The program accepts only one argument:
	// - the path of the file to read
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

	// Reading characters from the buffer
	// Storing every first and last digit before a new line
	// The digits might be represented as its alphabetical name:
	// one, two, three, four, five, six, seven, eight, nine, zero
	// Then adding first * 10 + last to a sum

	int sum = 0;

	char firstDigit = '\0';
	char lastDigit  = '\0';

	const int MAX_NUM_NAME_LENGTH = 5;
	char lastChars[MAX_NUM_NAME_LENGTH];
	memset(lastChars, '\0', sizeof(char) * 5);
	long lastCharsI = 0;

	void digitFound(char digit)
	{
		if (firstDigit == '\0')
			firstDigit = digit;
		lastDigit = digit;
	}

	void sumDigits()
	{
		int increment = concatenateAndSumDigits(firstDigit, lastDigit);
		sum += increment;
	}

	void resetLine()
	{
		firstDigit = '\0';
		lastDigit = '\0';
		memset(lastChars, '\0', sizeof(char) * 5);
		lastCharsI = 0;
	}

	for (long i = 0; i < length - 1; i++)
	{
		char currentChar = buffer[i];
		lastChars[lastCharsI] = currentChar;

		// Char is digit case
		if ((int)currentChar > 47 && (int)currentChar < 58)
		{
			digitFound(buffer[i]);
		}

		// Char might be part of a digit name case
		else if (currentChar != '\n')
		{
			switch (currentChar)
			{
			case 'e':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'n' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'o')
				{
					digitFound('1');
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'e' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'r' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'h' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 't')
				{
					digitFound('3');
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'v' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'i' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'f')
				{
					digitFound('5');
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'n' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'i' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'n')
				{
					digitFound('9');
				}

				break;
			case 'n':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'e' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'v' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'e' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 's')
				{
					digitFound('7');
				}

				break;
			case 'o':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'w' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 't')
				{
					digitFound('2');
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'r' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'e' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'z')
				{
					digitFound('0');
				}

				break;
			case 'r':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'u' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'o' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'f')
				{
					digitFound('4');
				}

				break;
			case 't':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'h' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'g' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'i' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 'e')
				{
					digitFound('8');
				}

				break;
			case 'x':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'i' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 's')
				{
					digitFound('6');
				}

				break;
			default:
				break;
			}
		}

		// End of line case
		else
		{
			sumDigits();
			resetLine();

			continue;
		}

		lastCharsI++;
		if (lastCharsI > 4)
			lastCharsI = 0;
	}
	sumDigits();

	// Print result
	printf("The result is: %d\n", sum);
	
	//// Cleanup

	// Free allocated memory
	free(buffer);

	return 0;
}
