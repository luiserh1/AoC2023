#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* previous;

	uint32_t num;
} SeedIdNode;

// The values from sourceRangeStart onward are mapped to destinationRangeStart onward
// This rule applies for rangeLength values
typedef struct
{
	void* previous;

	uint32_t destinationRangeStart;
	uint32_t sourceRangeStart;
	uint32_t rangeLength;
} MapWithRanges;

///////////////////////////////////////////////////////////////////////////////

static inline FILE* readFile(const char*);
void freeSeedNodes(SeedIdNode*);
void printSeeds(SeedIdNode*);
static inline int isDigit(char);
int insertOrdered(MapWithRanges**, uint32_t, uint32_t, uint32_t);
uint32_t getMappedValue(MapWithRanges*, uint32_t);
void printMapWithRanges(MapWithRanges*, uint32_t, char*, char*);
uint32_t getPreviousInCyclicArray(uint32_t*, long, long, long);

///////////////////////////////////////////////////////////////////////////////

static inline FILE* readFile(const char* filename)
{
	// Open the file in read mode
	FILE* fptr = fopen(filename, "r");
	return fptr;
}

uint32_t getPreviousInCyclicArray(uint32_t* arr, long length, long currentIndex, long prev)
{
	long prevIndex;
	if (prev > currentIndex)
		prevIndex = length - (prev - currentIndex);
	else
		prevIndex = currentIndex - prev;

	return arr[prevIndex];
}

void freeSeedNodes(SeedIdNode* seed)
{
	SeedIdNode* current = seed;
	while (current != NULL)
	{
		SeedIdNode* aux = (SeedIdNode*)current->previous;
		free(current);
		current = aux;
	}
}

void printSeeds(SeedIdNode* seed)
{
	SeedIdNode* current = seed;
	printf("Seeds:");
	while (current != NULL)
	{
		printf(" %d", current->num);
		current = current->previous;
	}
	printf("\n");
}

void printMapWithRanges(MapWithRanges* map, uint32_t maxValue, char* from, char* to)
{
	static const long NUM_OF_SAME_DIFFS_UNTIL_HIDE = 2;
	static const long NUM_OF_LAST_NUMS_DISPLAYED = 2;

	uint32_t prevNumbers[NUM_OF_SAME_DIFFS_UNTIL_HIDE];
	memset(prevNumbers, 0, sizeof(uint32_t) * NUM_OF_SAME_DIFFS_UNTIL_HIDE);
	long prevNumbersI = 0;

	printf("%-15s%-15s\n", from, to);
	uint32_t loopsSinceChange = 0;
	int lastDif = 0;
	for (uint32_t i = 0; i < maxValue; i++)
	{
		uint32_t value = getMappedValue(map, i);
		if (i - value == lastDif)
		{
			loopsSinceChange++;
		}
		else
		{
			lastDif = i - value;
			if (loopsSinceChange > NUM_OF_SAME_DIFFS_UNTIL_HIDE)
			{
				for (long j = 0; j < NUM_OF_SAME_DIFFS_UNTIL_HIDE; j++)
				{
					long prevIndex  = i - NUM_OF_SAME_DIFFS_UNTIL_HIDE + j;
					long prevAmount = NUM_OF_SAME_DIFFS_UNTIL_HIDE - j;
					long prevNum    = getPreviousInCyclicArray(prevNumbers, NUM_OF_SAME_DIFFS_UNTIL_HIDE, prevNumbersI, prevAmount);
					printf("%-15ld%-15ld\n", prevIndex, prevNum);
				} 
			}
			loopsSinceChange = 0;
		}

		if (loopsSinceChange <= NUM_OF_SAME_DIFFS_UNTIL_HIDE || i > maxValue - NUM_OF_LAST_NUMS_DISPLAYED)
			printf("%-15d%-15d\n", i, value);

		if (loopsSinceChange == NUM_OF_SAME_DIFFS_UNTIL_HIDE + 1)
			printf("%-15s%-15s\n", "...", "...");

		prevNumbers[prevNumbersI] = value;
		prevNumbersI++;
		if (prevNumbersI == NUM_OF_SAME_DIFFS_UNTIL_HIDE)
			prevNumbersI = 0;
	}
}

int insertOrdered(MapWithRanges** map, uint32_t destinationRangeStart, uint32_t sourceRangeStart, uint32_t rangeLength)
{
	MapWithRanges* newMap = malloc(sizeof(map));
	if (newMap == NULL)
		return 0;

	newMap->destinationRangeStart = destinationRangeStart;
	newMap->sourceRangeStart      = sourceRangeStart;
	newMap->rangeLength           = rangeLength;
	newMap->previous              = NULL;

	if (*map == NULL)
	{
		*map = newMap;
		return 1;
	}

	MapWithRanges* current = *map;
	while (current != NULL)
	{
		if (sourceRangeStart < current->sourceRangeStart)
		{
			newMap->previous  = current->previous;
			current->previous = newMap;
			break;
		}
		current = (*map)->previous;
	}

	return 1;
}

uint32_t getMappedValue(MapWithRanges* map, uint32_t at)
{
	MapWithRanges* current = map;
	while (current != NULL)
	{
		if (at >= current->sourceRangeStart && at < current->sourceRangeStart + current->rangeLength)
		{
			uint32_t posInRange = at - current->sourceRangeStart;
			return current->destinationRangeStart + posInRange;
		}

		current = current->previous;
	}

	return at;
}

static inline int isDigit(char c)
{
	if ((int)c > 47 && (int)c < 58)
		return 1;

	return 0;
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
	SeedIdNode* seedsHead                   = NULL;
	MapWithRanges* seedToSoilMap            = NULL;
	MapWithRanges* soilToFertilizerMap      = NULL;
	MapWithRanges* fertilizerToWaterMap     = NULL;
	MapWithRanges* waterToLightMap          = NULL;
	MapWithRanges* lightToTemperatureMap    = NULL;
	MapWithRanges* temperatureToHumidityMap = NULL;
	MapWithRanges* humidityToLocationMap    = NULL;

	uint32_t maxSeedId = 0; // This value will be used to calculate the requiered of the maps

	// parsing: seeds
	char line[250];
	if (fgets(line, sizeof(line), fptr) == NULL)
		return 1;
	/*char* token = */strtok(line, " "); // "seeds:"
	char* seedToken = strtok(NULL, " "); // the rest of the line (ints separated by white spaces)
	while (seedToken != NULL)
	{
		SeedIdNode* newSeed = malloc(sizeof(SeedIdNode));
		newSeed->previous = NULL;
		newSeed->num = atoi(seedToken);
		if (newSeed->num > maxSeedId)
			maxSeedId = newSeed->num;

		newSeed->previous = seedsHead;
		seedsHead = newSeed;

		seedToken = strtok(NULL, " ");
	}
	printSeeds(seedsHead);

	// calculating map sizes
	uint32_t mapsSize = 1;
	while (maxSeedId > 0)
	{
		mapsSize *= 10;
		maxSeedId /= 10;
	}

	// parsing: maps
	
	if (fgets(line, sizeof(line), fptr) == NULL) // expected blank line before "seed-to-soil" map
		return 2;

	int processNextMap(MapWithRanges** map)
	{
		if (fgets(line, sizeof(line), fptr) == NULL) // expected "name-of-map map:" line
			return 0;

		while (fgets(line, sizeof(line), fptr) != NULL)
		{
			if (!isDigit(line[0])) // expected: blank line before next map || the end of the file
				break;

			char* destinetionRangeStartToken = strtok(line, " ");
			char* sourceRangeStartToken      = strtok(NULL, " ");
			char* rangeLengthToken           = strtok(NULL, " ");

			uint32_t destinetionRangeStart = atoi(destinetionRangeStartToken);
			uint32_t sourceRangeStart      = atoi(sourceRangeStartToken);
			uint32_t rangeLength           = atoi(rangeLengthToken);

			if (!insertOrdered(map, destinetionRangeStart, sourceRangeStart, rangeLength))
				return 0;
		}

		return 1;
	}

	// seed-to-soil
	printf("___________________________\n");
	if (!processNextMap(&seedToSoilMap))
		return 3;
	printMapWithRanges(seedToSoilMap, mapsSize, "Seed", "Soil");

	// soil-to-fertilizer
	printf("___________________________\n");
	if (!processNextMap(&soilToFertilizerMap))
		return 4;
	printMapWithRanges(soilToFertilizerMap, mapsSize, "Soil", "Fertilizer");

	// fertilizer-to-water
	printf("___________________________\n");
	if (!processNextMap(&fertilizerToWaterMap))
		return 5;
	printMapWithRanges(fertilizerToWaterMap, mapsSize, "Fertilizer", "Water");

	// water-to-light
	printf("___________________________\n");
	if (!processNextMap(&waterToLightMap))
		return 6;
	printMapWithRanges(waterToLightMap, mapsSize, "Water", "Light");

	// light-to-temperature
	printf("___________________________\n");
	if (!processNextMap(&lightToTemperatureMap))
		return 7;
	printMapWithRanges(lightToTemperatureMap, mapsSize, "Light", "Temperature");

	// temperature-to-humidity
	printf("___________________________\n");
	if (!processNextMap(&temperatureToHumidityMap))
		return 8;
	printMapWithRanges(temperatureToHumidityMap, mapsSize, "Temperature", "Humidity");

	// humidity-to-location
	printf("___________________________\n");
	if (!processNextMap(&humidityToLocationMap))
		return 9;
	printMapWithRanges(humidityToLocationMap, mapsSize, "Humidity", "Location");

	// computing
	int minLocation = INT_MAX;
	SeedIdNode* currentNode = seedsHead;
	printf("\nMapping...\n");
	while (currentNode != NULL)
	{
		uint32_t seed = currentNode->num;
		printf("Seed % d -> ", seed);
		uint32_t soil = getMappedValue(seedToSoilMap, seed);
		printf("Soil %d -> ", soil);
		uint32_t fert = getMappedValue(soilToFertilizerMap, soil);
		printf("Fertilizer %d -> ", fert);
		uint32_t water = getMappedValue(fertilizerToWaterMap, fert);
		printf("Water %d -> ", water);
		uint32_t light = getMappedValue(waterToLightMap, water);
		printf("Light %d -> ", light);
		uint32_t temp = getMappedValue(lightToTemperatureMap, light);
		printf("Temperature %d -> ", temp);
		uint32_t humi = getMappedValue(temperatureToHumidityMap, temp);
		printf("Humidity %d -> ", humi);
		uint32_t loc = getMappedValue(humidityToLocationMap, humi);
		printf("[Locations %d]\n", loc);

		if (loc < minLocation)
			minLocation = loc;

		currentNode = currentNode->previous;
	}

	// Result

	printf("The result is: %d", minLocation);

	//// Cleanup

	// Free allocated memory
	freeSeedNodes(seedsHead);

	//// Succes

	return 0;
}
