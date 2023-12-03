#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	// The digits might be represented as its alphabetical name:
	// one, two, three, four, five, six, seven, eight, nine, zero
	// If we find a:
	// 'e': E + i + g + h + t || s + E + v + e + n || z + E + r + o || o + n + E || t + h + r + E + e ||
	//      f + i + v + E || s + e + v + E + n ||  n + i + n + E || t + h + r + e + E 
	// 'f': F + o + u + r || F + i + v + e
	// 'g': e + i + G + h + t
	// 'h': t + H + r + e + e || e + i + g + H + t
	// 'i': f + I + v + e || s + I + x || e + I + g + h + t || n + I + n + e
	// 'n': N + i + n + e || o + N + e || n + i + N + e || s + e + v + e + N
	// 'o': O + n + e || f + O + u + r || t + w + O + || z + e + r + O
	// 'r': t + h + R + e + e || z + e + R + o || f + o + u + R 
	// 's': S + i + x || S + e + v + e + n
	// 't': T + w + o || T + h + r + e + e || e + i + g + h + T
	// 'u': f + o + U + r
	// 'v': f + i + V + e || s + e + V + e + n
	// 'w': t + W + o
	// 'x': s + i + X
	// 'z': Z + e + r + o
	// Then adding first * 10 + last to a sum
	int sum = 0;
	char firstDigit = '\0';
	char lastDigit  = '\0';
	const int MAX_NUM_NAME_LENGTH = 5;
	char lastChars[MAX_NUM_NAME_LENGTH];
	memset(lastChars, '\0', sizeof(char) * 5);
	long lastCharsI = 0;
	for (long i = 0; i < length - 1; i++)
	{
		char currentChar = buffer[i];
		lastChars[lastCharsI] = currentChar;

		// Char is digit case
		if ((int)currentChar > 47 && (int)currentChar < 58)
		{
			if (firstDigit == '\0')
				firstDigit = buffer[i];
			lastDigit = buffer[i];
		}

		// Char might be part of a digit name case
		if (currentChar != '\n')
		{
			switch (currentChar)
			{
			case 'e':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'n' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'o')
				{
					if (firstDigit == '\0')
						firstDigit = '1';
					lastDigit = '1';
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'e' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'r' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'h' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 't')
				{
					if (firstDigit == '\0')
						firstDigit = '3';
					lastDigit = '3';
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'v' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'i' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'f')
				{
					if (firstDigit == '\0')
						firstDigit = '5';
					lastDigit = '5';
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'n' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'i' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'n')
				{
					if (firstDigit == '\0')
						firstDigit = '9';
					lastDigit = '9';
				}

				break;
			case 'n':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'e' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'v' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'e' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 's')
				{
					if (firstDigit == '\0')
						firstDigit = '7';
					lastDigit = '7';
				}

				break;
			case 'o':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'w' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 't')
				{
					if (firstDigit == '\0')
						firstDigit = '2';
					lastDigit = '2';
				}

				else if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'r' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'e' &&
					     getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'z')
				{
					if (firstDigit == '\0')
						firstDigit = '0';
					lastDigit = '0';
				}

				break;
			case 'r':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'u' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'o' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'f')
				{
					if (firstDigit == '\0')
						firstDigit = '4';
					lastDigit = '4';
				}

				break;
			case 't':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'h' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 'g' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 3) == 'i' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 4) == 'e')
				{
					if (firstDigit == '\0')
						firstDigit = '8';
					lastDigit = '8';
				}

				break;
			case 'x':
				if (getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 1) == 'i' &&
					getPreviousInCyclicArray(lastChars, MAX_NUM_NAME_LENGTH, lastCharsI, 2) == 's')
				{
					if (firstDigit == '\0')
						firstDigit = '6';
					lastDigit = '6';
				}

				break;
			default:
				break;
			}
		}

		// End of line case
		else
		{
			// sum
			int increment = concatenateAndSumDigits(firstDigit, lastDigit);
			printf("Sum. this row: %d\n", increment);
			sum += increment;

			// reset
			firstDigit = '\0';
			lastDigit  = '\0';
			memset(lastChars, '\0', sizeof(char) * 5);
			lastCharsI = 0;

			continue;
		}

		lastCharsI++;
		if (lastCharsI > 4)
			lastCharsI = 0;
	}
	int increment = concatenateAndSumDigits(firstDigit, lastDigit);
	printf("Sum. last row: %d\n", increment);
	sum += increment;

	// Print result
	printf("The result is: %d\n", sum);
	
	//// Cleanup

	// Free allocated memory
	free(buffer);

	return 0;
}
