#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* previous;

	uint8_t num;
} CardSeriesNumber;

typedef struct
{
	void* next;

	CardSeriesNumber* ownNumsHead;
	CardSeriesNumber* winNumsHead;

	uint8_t index;
	uint32_t amount;
} Card;

///////////////////////////////////////////////////////////////////////////////

void freeCardSeriesNumber(CardSeriesNumber* set);
void freeCard(Card* game);
static inline FILE* readFile(const char* filename);
static inline int intValueOfChar(char digit);

///////////////////////////////////////////////////////////////////////////////

// Card Series Number Head expected
void freeCardSeriesNumber(CardSeriesNumber* set)
{
	CardSeriesNumber* current = set;
	while (current != NULL)
	{
		CardSeriesNumber* aux = (CardSeriesNumber*)current->previous;
		free(current);
		current = aux;
	}
}

// Card Tail expected
void freeCard(Card* game)
{
	Card* currentGame = game;
	while (currentGame != NULL)
	{
		Card* aux = (Card*)currentGame->next;
		freeCardSeriesNumber(currentGame->ownNumsHead);
		freeCardSeriesNumber(currentGame->winNumsHead);
		free(currentGame);
		currentGame = aux;
	}
}

static inline FILE* readFile(const char* filename)
{
	// Open the file in read mode
	FILE* fptr = fopen(filename, "r");
	return fptr;
}

static inline int intValueOfChar(char digit)
{
	return (int)digit - 48;
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

	//// Work

	// Parsing file into data structure
	Card* cardTail = NULL;
	Card* cardPrev = NULL;

	char line[125];
	const char delimSection1[2] = ":";
	const char delimSection2[2] = "|";
	const char delimBetweenNumbers[2] = " ";
	while (fgets(line, sizeof(line), fptr) != NULL)
	{
		// Line format:
		// Card <index>: <player_number>* | <winning_number>*
		// Sections:
		//  - First -       - Second -        - Third -
		// Section Delimiters:
		//             :                  |

		// Section 1
		char* section1Token = strtok(line, delimSection1);
		char* restOfLine = strtok(NULL, delimSection1);
		//printf("Section 1: %s\n", section1Token);
		int cardIndex;
		sscanf(section1Token, "Card %d", &cardIndex);
		//printf("Card Index: %d\n", cardIndex);
		Card* newCard = malloc(sizeof(Card));
		if (newCard == NULL)
			return 1;
		newCard->index = cardIndex;
		newCard->amount = 1;

		// Section 2
		CardSeriesNumber* parseNumsToken(char* numToken)
		{
			CardSeriesNumber* resHead = NULL;
			while (numToken != NULL /*&& numToken != '\0'*/)
			{
				int num;
				sscanf(numToken, "%d", &num);
				CardSeriesNumber* newSeriesNum = malloc(sizeof(CardSeriesNumber));
				if (newSeriesNum == NULL)
					return NULL;
				newSeriesNum->num = num;
				newSeriesNum->previous = resHead;
				resHead = newSeriesNum;

				numToken = strtok(NULL, delimBetweenNumbers);
			}
			return resHead;
		}

		char* section2Token = strtok(restOfLine, delimSection2);
		restOfLine = strtok(NULL, delimSection2);
		//printf("Section 2: %s\n", section2Token);
		char* numToken = strtok(section2Token, delimBetweenNumbers);
		newCard->ownNumsHead = parseNumsToken(numToken);
		if (newCard->ownNumsHead == NULL)
			return 2;

		// Section 3
		//printf("Section 3: %s\n", restOfLine);
		numToken = strtok(restOfLine, delimBetweenNumbers);
		newCard->winNumsHead = parseNumsToken(numToken);
		if (newCard->winNumsHead == NULL)
			return 3;

		// Line fully processed
		if (cardTail == NULL)
			cardTail = newCard;
		if (cardPrev != NULL)
			cardPrev->next = newCard;
		newCard->next = NULL;
		cardPrev = newCard;
	}

	// Computing result

	int sumOfScratchcards = 0;
	Card* currentCard = cardTail;
	while (currentCard != NULL)
	{
		printf("Checking Card %d...\n", currentCard->index);
		int winningPointsAmount = 0;

		CardSeriesNumber* currentOwnNumber = currentCard->ownNumsHead;
		while (currentOwnNumber != NULL)
		{
			CardSeriesNumber* nextWinningNumber = NULL;
			CardSeriesNumber* currentWinningNumber = currentCard->winNumsHead;
			while (currentWinningNumber != NULL)
			{
				if (currentOwnNumber->num == currentWinningNumber->num)
				{
					printf("\t + %d is a winning number\n", currentOwnNumber->num);
					winningPointsAmount++;

					CardSeriesNumber* auxPrev = currentWinningNumber->previous;
					// Removing from winning numbers list
					if (nextWinningNumber == NULL)
						currentCard->winNumsHead = currentWinningNumber->previous;
					else
						nextWinningNumber->previous = currentWinningNumber->previous;
					free(currentWinningNumber);
					currentWinningNumber = auxPrev;
				}
				else
				{
					nextWinningNumber = currentWinningNumber;
					currentWinningNumber = currentWinningNumber->previous;
				}
			}

			currentOwnNumber = currentOwnNumber->previous;
		}

		Card* nextCard = currentCard->next;
		for (int i = winningPointsAmount; i > 0; i--)
		{
			if (nextCard == NULL)
				break;

			nextCard->amount += currentCard->amount;

			nextCard = nextCard->next;
		}

		printf("\t > Amount of scretchcards: %d\n", currentCard->amount);
		sumOfScratchcards += currentCard->amount;

		currentCard = currentCard->next;
	}

	// Result

	printf("The result is: %d\n", sumOfScratchcards);

	//// Cleanup

	// Free allocated memory
	freeCard(cardTail);

	//// Succes

	return 0;
}
