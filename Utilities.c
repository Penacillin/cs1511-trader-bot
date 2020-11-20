#include "trader_bot.h"
#include "Utilities.h"
#include <stdlib.h>
#include <string.h>

// Gets the nearest fuel station from the location of the bot
int nearest_fuel(struct bot *b)
{
	int first = 1;
	int counter = 0;
	int dist = 0;
	struct location* iterBackward = b->location;
	for (struct location* iterForward = b->location;
		(iterForward != iterBackward && iterForward->next != iterBackward && dist == 0) || first;
		first = 0, iterForward = iterForward->next, iterBackward = iterBackward->previous, counter++)
	{
		if (iterForward->type == LOCATION_PETROL_STATION)
			return dist = counter;
		if (iterBackward->type == LOCATION_PETROL_STATION)
			return dist = (-1)*counter;

	}

	return dist;
}
#include <stdio.h>
// Gets the distance to the nearest petrol station from a location
int nearestFuelLoc(locPtr l, locPtr* servo, int min)
{
	int first = 1;
	int counter = 0;
	int dist = 0;
	struct location* iterBackward = l;
	locPtr iterForward = l;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward) || first;
		counter++)
	{		
		if (iterForward->type == LOCATION_PETROL_STATION && iterForward->quantity > min)
		{
			*servo = iterForward;
			return dist = counter;
		}

		if (iterBackward->type == LOCATION_PETROL_STATION  && iterBackward->quantity > min)
		{
			*servo = iterBackward;
			return dist = (-1)*counter;
		}

		iterForward = iterForward->next;
		iterBackward = iterBackward->previous;
		first = 0;
	}
	if (iterBackward->type == LOCATION_PETROL_STATION && iterBackward->quantity > min)
	{
		*servo = iterBackward;
		return dist = (-1)*counter;
	}

	if (iterForward->type == LOCATION_PETROL_STATION  && iterForward->quantity > min)
	{
		*servo = iterForward;
		return dist = counter;
	}
	*servo = NULL;
	//printf("nearestFuelLoc did not find any fuel stations\n");
	return dist;
}

// Gets the remaining weight left in the bot
int getRemainingWeight(struct bot* b)
{
	int totalWeight = 0;

	for (struct cargo* cargoIter = b->cargo; cargoIter != NULL; cargoIter = cargoIter->next)
	{
		totalWeight += cargoIter->commodity->weight * cargoIter->quantity;
	}

	return b->maximum_cargo_weight - totalWeight;
}

// Gets the remaining volume left in the bot's cargo
int getRemainingVolume(struct bot* b)
{
	int totalVolume = 0;

	for (struct cargo* cargoIter = b->cargo; cargoIter != NULL; cargoIter = cargoIter->next)
	{
		totalVolume += cargoIter->commodity->volume * cargoIter->quantity;
	}

	return b->maximum_cargo_volume - totalVolume;
}

int getMaxQuantityIntoCargo(botPtr b, struct commodity* com)
{
	int maxByVolume = getRemainingVolume(b) / com->volume;
	int maxByWeight = getRemainingWeight(b) / com->weight;

	return mmin(maxByVolume, maxByWeight);
}

// Gets the maximum quantity of a certain commodity that can currently
// be stored on Seolhyun's back
int getMaximumBuyQuantity(struct bot* b, locPtr seller, locPtr buyer)
{
	// get minimum quantity from buyer seller availability
	int buyerSellerMax = mmin(seller->quantity, buyer->quantity);
	int cargoMax = getMaxQuantityIntoCargo(b, seller->commodity);
	return mmin(buyerSellerMax, cargoMax);
}

// Gets the maximum quantity of a certain commodity that can currently
// be stored on Seolhyun's back
int getMaximumBuyQuantityRaw(struct bot* b, int seller, int buyer, struct commodity* com)
{
	// get minimum quantity from buyer seller availability
	int buyerSellerMax = mmin(seller, buyer);
	int cargoMax = getMaxQuantityIntoCargo(b, com);
	return mmin(buyerSellerMax, cargoMax);
}


// Gets shortest distance between two indexes
int getDistanceBetweenIndex(int a, int b, int size)
{
	if (a == b)
		return 0;

	if (abs(b - a) > size / 2)
	{
		return (-1) * (size - abs(b - a));
	}
	
	return b - a;
}

// Gets the distance between two locations
int getDistanceBetweenLocs(locPtr a, locPtr b)
{	
	int first = 1;
	int counter = 0;
	int dist = 0;
	struct location* iterBackward = a;
	locPtr iterForward = a;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward) || first;
		counter++)
	{
		if (iterForward == b)
		{
			return dist = counter;
		}

		if (iterBackward == b)
		{
			return dist = (-1)*counter;
		}

		iterForward = iterForward->next;
		iterBackward = iterBackward->previous;
		first = 0;
	}
	if (iterForward == b)
	{
		return dist = counter;
	}

	if (iterBackward == b)
	{
		return dist = (-1)*counter;
	}
	//printf("nearestFuelLoc did not find any fuel stations\n");
	
	printf("HOE HOE HOE HOE locations distance died :'( \n");
	return dist;
}

// Gets the move from desired index
// Scales down to maximum move
int getMoveNFromDesiredIndex(botPtr b, int desiredIndex, int nOfLocations)
{
	int distanceToBestIndex = getDistanceBetweenIndex(0, desiredIndex, nOfLocations);
	// if the distance is greater than maximum move, scale down
	if (abs(distanceToBestIndex) > b->maximum_move)
	{
		if (distanceToBestIndex > 0)
			return b->maximum_move;
		else
			return (-1)*b->maximum_move;
	}
	else
	{
		return distanceToBestIndex;
	}
}

// Gets the total amount of fuel left in the world
int getFuelLeftInWorld(botPtr b)
{
	int fuel = 0;
	locPtr end = b->location;
	int first = 1;
	for (struct location* iterForward = b->location;
		iterForward != end || first--;
		 iterForward = iterForward->next)
	{
		if (iterForward->type == LOCATION_PETROL_STATION)
			fuel += iterForward->quantity;
	}
	return fuel;
}

// Gets the number of turns that would be required to move a certain distance
int numOfTurnsForDist(botPtr b, int dist)
{
	dist = abs(dist);
	if (dist == 0)
	{
		return 0;
	}
	if (dist % b->maximum_move == 0)
		return dist / b->maximum_move;
	return dist / b->maximum_move + 1;
}

// Gets the amount of quantity a bot has of a certain commodity
int getQuantityOfCommodity(botPtr b, struct commodity* com)
{
	int quantity = 0;
	if (b->cargo == NULL)
		return 0;
	for (struct cargo* cargoIter = b->cargo; cargoIter != NULL; cargoIter = cargoIter->next)
	{
		if (strcmp(cargoIter->commodity->name, com->name) == 0)
		{
			quantity += cargoIter->quantity;
		} 
	}

	return quantity;
}

// Get the quantity that can be bought for a certain volume and weight of 
// a specific commidity
int getQuantityForVolWeight(struct commodity* com, int vol, int weight)
{
	return mmin(vol / com->volume, weight / com->weight);
}

// Min and max functions:
int mmin(int a, int b)
{
	return a > b ? b : a;
}


int mmax(int a, int b)
{
	return a < b ? b : a;
}

// Get the remaining buy quantity of a commodity around the world
int getRemainingBuyQuantity(botPtr b, struct commodity* com)
{
	int q = 0;
	locPtr end = b->location;
	int first = 1;
	for (struct location* iterForward = b->location;
		iterForward != end || first--;
		iterForward = iterForward->next)
	{
		if (iterForward->type == LOCATION_BUYER
			&& iterForward->commodity == com)
			q += iterForward->quantity;
	}
	return q;
}

// Gets the amount of commodity that is floating around on bots
int getRemainingFloatingQuantityInWorld(botPtr b, struct commodity* com)
{
	int q = 0;
	locPtr end = b->location;
	int first = 1;
	for (struct location* iterForward = b->location;
		iterForward != end || first--;
		iterForward = iterForward->next)
	{
		if (iterForward->type == LOCATION_BUYER
			&& iterForward->commodity == com)
			q += iterForward->quantity;
	}
	return q;
}

// Converts a out of bound index to be within the circle of indexes
int getRealIndexFromCircularIndex(int index, int nOfLocations)
{
	if (index < 0)
	{
		return nOfLocations + index;
	}
	else if (index >= nOfLocations)
	{
		return index - nOfLocations;
	}

	return index;
}