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

int intValueOfChar(char digit)
{
	return (int)digit - 48;
}

typedef struct
{
	void* next;

	uint8_t r, g, b;
} GameSet;

typedef struct Game
{
	GameSet* headOfSet;
	void* next;

	uint8_t index;
} Game;

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

	// FSM
	enum State
	{
		READING_GAME_WORD,
		READING_GAME_INDEX,
		WAITING_AMOUNT,
		READING_SET_COLOR_AMOUNT,
		READING_SET_COLOR,
	} state = READING_GAME_WORD;
	// Initial state: READING_GAME_WORD
	// CURRENT_STATE              NEW_CHAR         NEW_STATE                  ACTIONS
	// -------------            | --------       | ---------                | -------
	// READING_GAME_WORD        + alpahabetical -> READING_GAME_WORD        : conitnue
	// READING_GAME_WORD        + space         -> READING_GAME_INDEX       : conitnue
	// READING_GAME_WORD        + NULL char     -> *                        : compute and show result
	// READING_GAME_INDEX       + digit         -> READING_GAME_INDEX       : start storing digits
	// READING_GAME_INDEX       + colon         -> WAITING_AMOUNT           : store game index from the digits, create a new empty set and queue it
	// WAITING_AMOUNT           + space         -> READING_SET_COLOR_AMOUNT : continue
	// READING_SET_COLOR_AMOUNT + digit         -> READING_SET_COLOR_AMOUNT : start storing digits
	// READING_SET_COLOR_AMOUNT + space         -> READING_SET_COLOR        : store the amount from the digits
	// READING_SET_COLOR        + space         -> READING_SET_COLOR        : continue
	// READING_SET_COLOR        + alpahabetical -> READING_SET_COLOR        : from the first char ('r', 'g' or, 'b') associate the previous value to a color
	// READING_SET_COLOR        + comma         -> WAITING_AMOUNT           : conitnue
	// READING_SET_COLOR        + semi_colon    -> WAITING_AMOUNT           : create a new empty set and queue it
	// READING_SET_COLOR        + line_break    -> READING_GAME_WORD        : conitnue

	Game* gamesHead = NULL;
	char gameDigits[3];
	int gameDigitsI = 0;
	char amountDigits[3];
	uint8_t lastReadAmount = 0;
	int amountDigitsI = 0;
	for (long i = 0; i < length; i++)
	{
		char currentChar = buffer[i];
		switch (state)
		{
		case READING_GAME_WORD:
			if (currentChar == ' ')
				state = READING_GAME_INDEX;
			break;
		case READING_GAME_INDEX:
			if (currentChar == ':')
			{
				int sumIndx = 0;
				gameDigitsI--;
				for (int j = gameDigitsI; j > -1; j--)
				{
					int digitAsInt = intValueOfChar(gameDigits[j]);
					int posInNum   = gameDigitsI - j;
					int digitSum   = digitAsInt * pow(10, posInNum);
					sumIndx += digitSum;
				}

				Game* newGame = malloc(sizeof(Game));
				if (newGame == NULL)
					return 1;
				newGame->headOfSet = malloc(sizeof(GameSet));
				if (newGame->headOfSet == NULL)
					return 2;
				newGame->headOfSet->next = NULL;
				newGame->headOfSet->r = 0;
				newGame->headOfSet->g = 0;
				newGame->headOfSet->b = 0;
				newGame->index = sumIndx;
				newGame->next = gamesHead;
				gamesHead = newGame;

				gameDigitsI = 0;
				state = WAITING_AMOUNT;
			}
			else // currentChar is digit
			{
				gameDigits[gameDigitsI] = currentChar;
				gameDigitsI++;
			}
			break;
		case WAITING_AMOUNT:
			state = READING_SET_COLOR_AMOUNT;
			break;
		case READING_SET_COLOR_AMOUNT:
			if (currentChar == ' ')
			{
				amountDigitsI--;
				for (int j = amountDigitsI; j > -1; j--)
				{
					int digitAsInt = intValueOfChar(amountDigits[j]);
					int posInNum   = amountDigitsI - j;
					int digitSum   = digitAsInt * pow(10, posInNum);
					lastReadAmount += digitSum;
				}

				amountDigitsI = 0;
				state = READING_SET_COLOR;
			}
			else // currentChar is digit
			{
				amountDigits[amountDigitsI] = currentChar;
				amountDigitsI++;
			}
			break;
		case READING_SET_COLOR:
			if (currentChar == ',')
			{
				state = WAITING_AMOUNT;
			}
			else if (currentChar == ';')
			{
				GameSet *newSet = malloc(sizeof(GameSet));
				if (newSet == NULL)
					return 3;
				newSet->next = gamesHead->headOfSet;
				newSet->r = 0;
				newSet->g = 0;
				newSet->b = 0;
				gamesHead->headOfSet = newSet;

				state = WAITING_AMOUNT;
			}
			else if (currentChar == '\n')
			{
				state = READING_GAME_WORD;
			}
			// reading the colour name. true if it's the first char of the word
			else if (currentChar != ' ' && lastReadAmount > 0)
			{
				if (currentChar == 'r')
				{
					gamesHead->headOfSet->r = lastReadAmount;
					printf("Red value: %d\n", gamesHead->headOfSet->r);
				}
				else if (currentChar == 'g')
				{
					gamesHead->headOfSet->g = lastReadAmount;
					printf("Green value: %d\n", gamesHead->headOfSet->g);
				}
				else if (currentChar == 'b')
				{
					gamesHead->headOfSet->b = lastReadAmount;
					printf("Blue value: %d\n", gamesHead->headOfSet->b);
				}

				lastReadAmount = 0;
			}
			break;
		}
	}

	// Restrictions to a valid game
	// 12 red MAX, 13 green MAX, 14 blue MAX
	const int MAX_RED   = 12;
	const int MAX_GREEN = 13;
	const int MAX_BLUE  = 14;
	
	Game* currentGame = gamesHead;
	int sumOfValidGamesIndices = 0;
	while (currentGame != NULL)
	{
		printf("Analysing GAME %d\n", currentGame->index);

		GameSet* currentGameSet = currentGame->headOfSet;
		int isValid = 1;
		while (currentGameSet != NULL)
		{
			printf(" - Analysing set of (%d r, %d g, %d b)\n", currentGameSet->r, currentGameSet->g, currentGameSet->b);

			if (currentGameSet->r > MAX_RED   ||
				currentGameSet->g > MAX_GREEN ||
				currentGameSet->b > MAX_BLUE)
			{
				isValid = 0;
				//break;
			}

			currentGameSet = (GameSet*)currentGameSet->next;
		}
		if (isValid)
			sumOfValidGamesIndices += currentGame->index;
		else
			printf(">Game is not valid!\n");

		currentGame = (Game*)currentGame->next;
	}

	// Result

	printf("The result is: %d", sumOfValidGamesIndices);

	//// Cleanup

	// Free allocated memory
	free(buffer);

	return 0;
}
