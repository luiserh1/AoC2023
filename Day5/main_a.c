#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* previous;

	uint32_t num;
} SeedIdNode;

///////////////////////////////////////////////////////////////////////////////

static inline FILE* readFile(const char*);
void freeSeedNodes(SeedIdNode*);
void printSeeds(SeedIdNode*);
void printMap(uint32_t*, uint32_t, char*, char*);
static inline int isDigit(char);

///////////////////////////////////////////////////////////////////////////////

static inline FILE* readFile(const char* filename)
{
	// Open the file in read mode
	FILE* fptr = fopen(filename, "r");
	return fptr;
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

void printMap(uint32_t* map, uint32_t length, char* from, char* to)
{
	printf("%-15s%-15s\n", from, to);
	uint32_t loopsSinceChange = 0;
	int lastDif = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		if (i - map[i] == lastDif)
		{
			loopsSinceChange++;
		}
		else
		{
			lastDif = i - map[i];
			if (loopsSinceChange > 3)
				i -= 2;
			loopsSinceChange = 0;
		}

		if (loopsSinceChange < 3 || i > length - 3)
			printf("%-15d%-15d\n", i, map[i]);

		if (loopsSinceChange == 3)
			printf("%-15s%-15s\n", "...", "...");
	}
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
	SeedIdNode* seedsHead;                // seeds ids
	uint32_t* seedToSoilMap;              // key: uint32_t index (seed id),        value: uint32_t value in memory (soil id)
	uint32_t* soilToFertilizerMap;        // key: uint32_t index (soil id),        value: uint32_t value in memory (fertilizer id)
	uint32_t* fertilizerToWaterMap;;      // key: uint32_t index (fertilizer id),  value: uint32_t value in memory (water id)
	uint32_t* waterToLightMap;;           // key: uint32_t index (water id),       value: uint32_t value in memory (light id)
	uint32_t* lightToTemperatureMap;;     // key: uint32_t index (light id),       value: uint32_t value in memory (temperature id)
	uint32_t* temperatureToHumidityMap;;  // key: uint32_t index (temperature id), value: uint32_t value in memory (humidity id)
	uint32_t* humidityToLocationMap;;     // key: uint32_t index (humidity id),    value: uint32_t value in memory (location id)

	uint32_t maxSeedId = 0; // This value will be used to calculate the requiered of the maps

	// parsing: seeds
	char line[250];
	if (fgets(line, sizeof(line), fptr) == NULL)
		return 1;
	char* token = strtok(line, " "); // "seeds:"
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
	
	if (fgets(line, sizeof(line), fptr) == NULL) // expected blank line before seed-to-soil map
		return 2;
	
	uint32_t* allocateMap()
	{
		uint32_t* map = malloc(sizeof(uint32_t) * mapsSize);
		if (map == NULL)
			return NULL;
		for (uint32_t i = 0; i < mapsSize; i++)
			map[i] = i;

		return map;
	}

	int processNextMap(uint32_t* map)
	{
		if (fgets(line, sizeof(line), fptr) == NULL) // expected "name-of-map map:" line
			return 0;

		while (fgets(line, sizeof(line), fptr) != NULL)
		{
			if (!isDigit(line[0])) // expected blank line before soil-to-fertilizer
				break;

			char* destinetionRangeStartToken = strtok(line, " ");
			char* sourceRangeStartToken      = strtok(NULL, " ");
			char* rangeLengthToken           = strtok(NULL, " ");

			uint32_t destinetionRangeStart = atoi(destinetionRangeStartToken);
			uint32_t sourceRangeStart      = atoi(sourceRangeStartToken);
			uint32_t rangeLength           = atoi(rangeLengthToken);

			for (uint32_t i = 0; i < rangeLength; i++)
				map[sourceRangeStart + i] = destinetionRangeStart + i;
		}

		return 1;
	}

	// seed-to-soil
	printf("___________________________\n");
	seedToSoilMap = allocateMap();
	if (!processNextMap(seedToSoilMap))
		return 3;
	printMap(seedToSoilMap, mapsSize, "Seed", "Soil");

	// soil-to-fertilizer
	printf("___________________________\n");
	soilToFertilizerMap = allocateMap();
	if (!processNextMap(soilToFertilizerMap))
		return 4;
	printMap(soilToFertilizerMap, mapsSize, "Soil", "Fertilizer");

	// fertilizer-to-water
	printf("___________________________\n");
	fertilizerToWaterMap = allocateMap();
	if (!processNextMap(fertilizerToWaterMap))
		return 4;
	printMap(fertilizerToWaterMap, mapsSize, "Fertilizer", "Water");

	// water-to-light
	printf("___________________________\n");
	waterToLightMap = allocateMap();
	if (!processNextMap(waterToLightMap))
		return 4;
	printMap(waterToLightMap, mapsSize, "Water", "Light");

	// light-to-temperature
	printf("___________________________\n");
	lightToTemperatureMap = allocateMap();
	if (!processNextMap(lightToTemperatureMap))
		return 4;
	printMap(lightToTemperatureMap, mapsSize, "Light", "Temperature");

	// temperature-to-humidity
	printf("___________________________\n");
	temperatureToHumidityMap = allocateMap();
	if (!processNextMap(temperatureToHumidityMap))
		return 4;
	printMap(temperatureToHumidityMap, mapsSize, "Temperature", "Humidity");

	// humidity-to-location
	printf("___________________________\n");
	humidityToLocationMap = allocateMap();
	if (!processNextMap(humidityToLocationMap))
		return 4;
	printMap(humidityToLocationMap, mapsSize, "Humidity", "Location");

	// computing
	int minLocation = INT_MAX;
	SeedIdNode* currentNode = seedsHead;
	printf("\nMapping...\n");
	while (currentNode != NULL)
	{
		uint32_t seed = currentNode->num;
		printf("Seed % d -> ", seed);
		uint32_t soil = seedToSoilMap[seed];
		printf("Soil %d -> ", soil);
		uint32_t fert = soilToFertilizerMap[soil];
		printf("Fertilizer %d -> ", fert);
		uint32_t water = fertilizerToWaterMap[fert];
		printf("Water %d -> ", water);
		uint32_t light = waterToLightMap[water];
		printf("Light %d -> ", light);
		uint32_t temp = lightToTemperatureMap[light];
		printf("Temperature %d -> ", temp);
		uint32_t humi = temperatureToHumidityMap[temp];
		printf("Humidity %d -> ", humi);
		uint32_t loc = humidityToLocationMap[humi];
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
