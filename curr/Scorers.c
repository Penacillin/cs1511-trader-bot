#include "Scorers.h"
#include "trader_bot.h"
#include "WorldAnalyzers.h"
#include "Utilities.h"
#include <math.h>
#include <stdlib.h>

// Gets a penalty for distance from 0 the index is
int getPenaltyForFarnessIndex(botPtr b, int index, int maxDistance)
{
	// Get distance  
	int penalty = abs(getDistanceBetweenIndex(0, index, maxDistance));

	penalty = penalty * getAverageFuelCost(b->location);
	// penalty * (penalty / b->maximum_move)

	return penalty;
}

// Gets a penalty based on raw distance
int getPenaltyForFarnessDist(botPtr b, int dist)
{
	int penalty = abs(dist);
	int numTurnsRequired = numOfTurnsForDist(b, abs(dist));
	penalty += abs(dist) * getAverageFuelCost(b->location);
	//	+ penalty * (abs(dist) / b->maximum_move);

	return penalty;
}