#include "trader_bot.h"
#include "MultiBot.h"
#include "Utilities.h"
#include "WorldAnalyzers.h"

#include <stdlib.h>
#include <stdio.h>
int numBotsAtLoc(locPtr l)
{
	int count = 0;
	if (l->bots == NULL)
		return 0;
	for (struct bot_list* i = l->bots; i != NULL; i = i->next)
	{
		if (i->bot->location == l)
			count++;
	}
	return count;
}


int getNumOfEmptyBotsAtLoc(locPtr l)
{
	int count = 0;
	if (l->bots == NULL)
		return 0;
	for (struct bot_list* i = l->bots; i != NULL; i = i->next)
	{
		if (i->bot->location == l && i->bot->cargo == NULL
			&& i->bot->cash > 200000)
			count++;
	}
	if (count == 0)
		count = 0;
	return count;
}

int getNumOfEmptyBotsWithinRegion(botPtr b, locPtr l)
{
	int counter = 0;
	int curr = 0;
	int region = b->maximum_move;
	counter += getNumOfEmptyBotsAtLoc(l);

	locPtr iterBackward = l->previous;
	locPtr iterForward = l->next;
	for (;
		curr < region;
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		counter += getNumOfEmptyBotsAtLoc(iterForward) 
			+ getNumOfEmptyBotsAtLoc(iterBackward);
		//printf("numbotsregioncom counter is now %d (+%d+%d)\n", counter, numBotsAtLocWithCom(iterForward, com), numBotsAtLocWithCom(iterBackward, com));
		curr++;
	}
	return counter;
}

int numBotsAtLocWithCom(locPtr l, struct commodity* com, int* quantitySum)
{
	int count = 0;
	if (l->bots == NULL)
		return 0;
	int qSum = 0;
	for (struct bot_list* i = l->bots; i != NULL; i = i->next)
	{	
		int q = getQuantityOfCommodity(i->bot, com);
		if (i->bot->location == l && q > 0)
		{
			count++;
			qSum += q;
		}
	}
	if(quantitySum)
		*quantitySum = qSum;
	return count;
}

int getNumOfBotsWorld(botPtr b)
{
	int counter = 0;
	locPtr iterBackward = b->location->previous;
	locPtr iterForward = b->location->next;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward);
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		counter += numBotsAtLoc(iterForward) + numBotsAtLoc(iterBackward);
	}
	if (iterForward == iterBackward)
		counter += numBotsAtLoc(iterForward);
	else
		counter += numBotsAtLoc(iterForward) + numBotsAtLoc(iterBackward);

	return counter;
}

int getNumOfBotsWithinRegion(locPtr l, int region)
{
	int counter = 0;
	int curr = 0;
	counter += numBotsAtLoc(l);

	locPtr iterBackward = l->previous;
	locPtr iterForward = l->next;
	for (;
		curr < region;
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		counter += numBotsAtLoc(iterForward) + numBotsAtLoc(iterBackward);
		curr++;
	}
	return counter;
}

// Get the number of bots at a location with a maximum amount of fuel
int numBotsAtLoc_FuelCap(locPtr l, int fuelCap, int* wantedFuel)
{
	int avFuelCost = getAverageFuelCost(l);
	int count = 0;
	if (l->bots == NULL)
		return 0;
	for (struct bot_list* i = l->bots; i != NULL; i = i->next)
	{
		if (i->bot->location == l && i->bot->fuel <= fuelCap
			&& i->bot->cash > 20 * avFuelCost)
		{
			count++;
			*wantedFuel += i->bot->fuel_tank_capacity - i->bot->fuel;
		}
	}
	*wantedFuel /= 2;
	return count;
}

int getNumOfBotsWithinRegion_FuelCap(locPtr l, int region, int fuelCap, int* wantedFuel)
{
	int counter = 0;
	int curr = 0;
	int wFuel = 0;
	counter += numBotsAtLoc_FuelCap(l, fuelCap, &wFuel);

	locPtr iterBackward = l->previous;
	locPtr iterForward = l->next;
	for (;
		curr < region;
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		if (iterForward->type != LOCATION_PETROL_STATION)
			counter += numBotsAtLoc_FuelCap(iterForward, fuelCap, &wFuel);
		if(iterBackward->type != LOCATION_PETROL_STATION)
			counter +=  numBotsAtLoc_FuelCap(iterBackward, fuelCap, &wFuel);

		curr++;
	}
	if (wantedFuel)
		*wantedFuel = wFuel;
	return counter;
}

int getNumOfBotsWithinRegionWithCom(locPtr l, int region, struct commodity* com)
{
	int counter = 0;
	int curr = 0;
	counter += numBotsAtLocWithCom(l, com, NULL);

	locPtr iterBackward = l->previous;
	locPtr iterForward = l->next;
	for (;
		curr < region;
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		counter += numBotsAtLocWithCom(iterForward, com, NULL) 
			+ numBotsAtLocWithCom(iterBackward, com, NULL);
		//printf("numbotsregioncom counter is now %d (+%d+%d)\n", counter, numBotsAtLocWithCom(iterForward, com), numBotsAtLocWithCom(iterBackward, com));
		curr++;
	}
	return counter;
}

// Gets the number of bots within range of a servo that 
// probably need fuel
int expectedServoDivisor(botPtr b, locPtr l, int nOfLocations, int* wFuel)
{
	int criticalFuel = b->fuel_tank_capacity / 5;
	int wantedFuel = 0;
	if (b->location == l)
	{
		int div = numBotsAtLoc_FuelCap(b->location, criticalFuel, wFuel);
		if (div == 0) return 1;
	}
	int div = getNumOfBotsWithinRegion_FuelCap(l, b->maximum_move, criticalFuel, &wantedFuel) * 2 / 3 + 1;
	if (wFuel)
		*wFuel = wantedFuel;
	return div;
}

// Gets the quantity divider for buyers
int getBuyerDivider(botPtr b, locPtr buyer)
{
	int botsOnBuyers = numBotsAtLocWithCom(buyer, buyer->commodity, NULL);
	if (b->location == buyer && botsOnBuyers > 0)
		return botsOnBuyers;
	
	int turnsRequired = numOfTurnsForDist(b, getDistanceBetweenLocs(b->location, buyer));
	
	int botsEqualToUs = getNumOfBotsWithinRegionWithCom(buyer, turnsRequired * b->maximum_move, buyer->commodity);
	
	if (botsEqualToUs == 0)
		return 1;

	return botsEqualToUs;
}
