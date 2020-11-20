#include "Scorers.h"
#include "trader_bot.h"
#include "WorldAnalyzers.h"
#include "Utilities.h"
#include "MultiBot.h"
#include <math.h>
#include <stdlib.h>

// Gets a penalty for distance from 0 the index is
int getPenaltyForFarnessIndex(botPtr b, int index, int maxDistance)
{
	// Get distance  
	int penalty = abs(getDistanceBetweenIndex(0, index, maxDistance));

	penalty = penalty * getAverageFuelCost(b->location);

	return penalty;
}

// Gets a penalty based on raw distance
int getPenaltyForFarnessDist(botPtr b, int dist)
{
	int penalty = abs(dist);
	int numTurnsRequired = numOfTurnsForDist(b, abs(dist));
	penalty += abs(dist) * getAverageFuelCost(b->location);

	return penalty;
}

// Gets a penalty based on raw distance and bases fuel costs on closest
// fuel station
int getPenaltyForFarnessDistClosestFuel(botPtr b, int dist, locPtr l)
{
	int penalty = abs(dist);
	int numTurnsRequired = numOfTurnsForDist(b, abs(dist));
	locPtr closestServo;
	int distToFuel = nearestFuelLoc(l, &closestServo, mmax(b->fuel_tank_capacity - b->fuel, getNumOfBotsWorld(b) * 2));
	if (closestServo)
		penalty += abs(dist) * closestServo->price;
	else
		penalty += abs(dist) * getAverageFuelCost(b->location);

	return penalty;
}