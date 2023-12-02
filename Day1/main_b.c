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
	// The digits might be represented as its alphabetical name:
	// one, two, three, four, five, six, seven, eight, nine, zero
	// If we find a:
	// 'e': E + i + g + h + t || s + E + v + e + n || z + E + r + o || o + n + E || t + h + r + E + e ||
	//       f + i + v + E || s + e + v + E + n ||  n + i + n + E || t + h + r + e + E 
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
	char lastChars[5];
	memset(lastChars, '\0', sizeof(char) * 5);
	int lastCharsI = 0;
	for (int i = 0; i < length - 1; i++)
	{
		if (buffer[i] == '\n')
		{
			int increment = concatenateAndSumDigits(firstDigit, lastDigit);
			printf("Sum. this row: %d\n", increment);
			sum += increment;
			firstDigit = '\0';
			lastDigit  = '\0';
			lastCharsI = 0;

			continue;
		}

		if ((int)buffer[i] > 47 && (int)buffer[i] < 58)
		{
			if (firstDigit == '\0')
				firstDigit = buffer[i];
			lastDigit = buffer[i];
			lastCharsI = 0;

			continue;
		}

		switch (buffer[i])
		{
		case 'e':
			switch (lastCharsI)
			{
			case 0: // Eight?
				lastChars[lastCharsI] = 'e';
				lastCharsI++;
				break;
			case 1: // sEven?, zEro?
				if (lastChars[0] == 's' || lastChars[0] == 'z')
				{
					lastChars[lastCharsI] = 'e';
					lastCharsI++;
					break;
				}

				lastCharsI = 0;
				break;
			case 2: // onE?
				if (lastChars[1] == 'n')
				{
					if (firstDigit == '\0')
						firstDigit = '1';
					lastDigit = '1';
				}

				lastCharsI = 0;
				break;
			case 3: // fivE sevEn? thrEe? ninE
				if ((lastChars[1] == 'e' && lastChars[2] == 'v') || lastChars[2] == 'r')
				{
					lastChars[lastCharsI] = 'e';
					lastCharsI++;
					break;
				}

				if (lastChars[2] == 'v')
				{
					if (firstDigit == '\0')
						firstDigit = '5';
					lastDigit = '5';
				}

				if (lastChars[2] == 'n')
				{
					if (firstDigit == '\0')
						firstDigit = '9';
					lastDigit = '9';
				}

				lastCharsI = 0;
				break;
			case 4: // threE
				if (lastChars[3] == 'e')
				{
					if (firstDigit == '\0')
						firstDigit = '3';
					lastDigit = '3';
				}

				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'f':
			switch (lastCharsI)
			{
			case 0: // Four? Five?
				lastChars[lastCharsI] = 'f';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'g':
			switch (lastCharsI)
			{
			case 2: // eiGht?
				lastChars[lastCharsI] = 'g';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'h':
			switch (lastCharsI)
			{
			case 1: // tHree?
			case 3: // eigHt?
				lastChars[lastCharsI] = 'h';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'i':
			switch (lastCharsI)
			{
			case 1: // fIve?, sIx?, eIght?, nIne?
				lastChars[lastCharsI] = 'i';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'n':
			switch (lastCharsI)
			{
			case 0: // Nine?
			case 1: // oNe?
			case 2: // niNe?
				lastChars[lastCharsI] = 'n';
				lastCharsI++;
				break;
			case 5: // seveN
				if (firstDigit == '\0')
					firstDigit = '7';
				lastDigit = '7';
				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'o':
			switch (lastCharsI)
			{
			case 0: // One?
			case 1: // fOur?
				lastChars[lastCharsI] = 'o';
				lastCharsI++;
				break;
			case 2: // twO
				if (firstDigit == '\0')
					firstDigit = '2';
				lastDigit = '2';
				lastCharsI = 0;
				break;
			case 3: // zerO
				if (firstDigit == '\0')
					firstDigit = '0';
				lastDigit = '0';
				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'r':
			switch (lastCharsI)
			{
			case 2: // thRee? zeRo?
				lastChars[lastCharsI] = 'r';
				lastCharsI++;
				break;
			case 3: // fouR
				if (firstDigit == '\0')
					firstDigit = '4';
				lastDigit = '4';
				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 's':
			switch (lastCharsI)
			{
			case 0: // Six? Seven?
				lastChars[lastCharsI] = 's';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 't':
			switch (lastCharsI)
			{
			case 0: // Two? Three?
				lastChars[lastCharsI] = 't';
				lastCharsI++;
				break;
			case 4: // eighT
				if (firstDigit == '\0')
					firstDigit = '8';
				lastDigit = '8';
				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}
		
			break;
		case 'u':
			switch (lastCharsI)
			{
			case 2: // foUr?
				lastChars[lastCharsI] = 'u';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'v':
			switch (lastCharsI)
			{
			case 2: // fiVe? seVen?
				lastChars[lastCharsI] = 'v';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'w':
			switch (lastCharsI)
			{
			case 1: // tWo?
				lastChars[lastCharsI] = 'w';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'x':
			switch (lastCharsI)
			{
			case 2: // siX
				if (firstDigit == '\0')
					firstDigit = '6';
				lastDigit = '6';
				lastCharsI = 0;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		case 'z':
			switch (lastCharsI)
			{
			case 0: // Zero
				lastChars[lastCharsI] = 'z';
				lastCharsI++;
				break;
			default:
				lastCharsI = 0;
				break;
			}

			break;
		default:
			lastCharsI = 0;
			break;
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
