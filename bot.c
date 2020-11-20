#include "trader_bot.h"
#include "Utilities.h"
#include "WorldAnalyzers.h"
#include "Scorers.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

char *get_bot_name(void)
{
    return "Kim Seolhyun x Shin Hyejeong";
}


void get_action(struct bot *b, int *action, int *n)
{
	int nOfLocations = getNumberOfLocations(b);

	// List of scores, with index 0 being where the bot is currently
	// SCORE | TYPE | RELEVANT QUANTITY | post fuel requirement
	int scores[nOfLocations][SCORES_DATA_POINTS];

	for (int i = 0; i < nOfLocations; i++)
	{
		for (int j = 0; j < SCORES_DATA_POINTS; j++)
		{
			scores[i][j] = 0;
		}
	}

	locPtr sellersBuyer = 0;
	// Loop through locations and apply scores
	locPtr end = b->location;
	int index = 0;
	int first = 1;
	for (locPtr locIter = b->location; locIter != end || first; locIter = locIter->next, index++)
	{
		// If it's a seller, add score for its profitability
		if (locIter->type == LOCATION_SELLER)
		{
			scores[index][0] += getMaximumPossibleProfit(b, locIter, &(scores[index][2]), &(scores[index][3]), &sellersBuyer);
			int numOfTurns = numOfTurnsForDist(b, abs(getDistanceBetweenIndex(0, index, nOfLocations)));
			//scores[index][0] /=  numOfTurns > 0 ? numOfTurns : 1;
			scores[index][1] = LOCATION_SELLER;
		}
		// If it's a buyer, calculate any profits it has (depending on bot's cargo)
		else if (locIter->type == LOCATION_BUYER)
		{
			scores[index][0] += getBuyerProfit(b, locIter, &(scores[index][2]));
			scores[index][1] = LOCATION_BUYER;
		}
		// If it's a petrol station, check if we might wanna go there
		else if (locIter->type == LOCATION_PETROL_STATION)
		{
			scores[index][1] = LOCATION_PETROL_STATION;
			
		}
		else if (locIter->type == LOCATION_DUMP)
		{
			scores[index][1] = LOCATION_DUMP;
			scores[index][0] += 0;
		}

		// Substract score for locations farness from bot, with multipliers for being
		// for the average price of petrol		
		scores[index][0] -= getPenaltyForFarnessIndex(b, index, nOfLocations);
		
		int numOfTurnsRequired = numOfTurnsForDist(b, abs(getDistanceToLocation(b, locIter))) + 1;
	

		first = 0;

	} // Finish stage 1 of scores (buyers and sellers)
	
	// Get the desired Index (highest score)
	int bestIndex = 0;
	int maxProfit = 0; // Set minimum profit to 0, else don't move
	locPtr desiredLoc = b->location;
	locPtr li = b->location;
	for (int i = 0; i < nOfLocations; i++)
	{
		if (scores[i][0] > maxProfit)
		{
			maxProfit = scores[i][0];
			bestIndex = i;
			desiredLoc = li;
		}
		li = li->next;
	}

	// Apply penalty for being out of range	and look for dumps
	first = 1;
	index = 0;
	for (locPtr locIter = b->location; locIter != end || first; locIter = locIter->next, index++)
	{
		// If it's a seller, apply any fuel based penalties
		if (locIter->type == LOCATION_SELLER)
		{
			scores[index][3] = getSellerRangePenalty(b, locIter,
				abs(getDistanceBetweenIndex(0,index, nOfLocations)),
				 scores[index][0], nOfLocations);
			scores[index][0] -= scores[index][3];
		}
		// If its a buyer, apply the buyer penalties
		if (locIter->type == LOCATION_BUYER)
		{
			scores[index][3] = getBuyerRangePenalty(b, locIter,
				abs(getDistanceBetweenIndex(0, index, nOfLocations)),
				scores[index][0]);
			scores[index][0] -= scores[index][3];
		}
		// If its a dump, apply the worth of a dump
		else if (locIter->type == LOCATION_DUMP)
		{
			scores[index][0] += getWorthOfDump(b, maxProfit, bestIndex, desiredLoc, index, nOfLocations);
		}
		first = 0;
	}

	// Apply worths of petrol stations
	first = 1;
	index = 0;
	for (locPtr locIter = b->location; locIter != end || first--; locIter = locIter->next, index++)
	{
		// If it's a petrol station, check if we might wanna go there
		if (locIter->type == LOCATION_PETROL_STATION)
		{
			printf("%s station score before adding it %d\n", locIter->name, scores[index][0]);
			scores[index][0] += getWorthOfPetrolStation(b, locIter, index, bestIndex, nOfLocations, scores, &(scores[index][2]));
			//flowPetrolBenefits(b, nOfLocations, scores, index, locIter);
		}
		// Divide the score based on how many turns it would take to get there
		int numOfTurnsRequired = numOfTurnsForDist(b, abs(getDistanceToLocation(b, locIter))) + 1;
		scores[index][0] = scores[index][0] / (numOfTurnsRequired * 2.0 / 3);	
	}

	// Finished finding all the scores
	print_world_S(b, nOfLocations, scores);

	// break for error
	//if (b->turns_left == 43)
	//	exit(2);

	// Look for best index again hurhurhurhrurhu
	bestIndex = 0;
	maxProfit = 0; // Set minimum profit to 0, else don't move
	for (int i = 0; i < nOfLocations; i++)
	{
		if (scores[i][0] > maxProfit)
		{
			maxProfit = scores[i][0];
			bestIndex = i;
		}
	}
	printf("BOT: desiredIndex %d, turns left: %d, profit: %d, fuel: %d\n", bestIndex, b->turns_left, maxProfit, b->fuel);

	// If there is no profit to be made, do nothing
	if (maxProfit == 0)
	{
		*n = 0;
		*action = ACTION_MOVE;
		if (*n == 0 && b->turns_left > 7)
		{
			printf("MAX PROFIT 0 ?????????????\n");
			*n = 0;
			//exit(1);
		}
	}

	// Return move:
	// If the best move is to do whatever we're on right now:
	if (bestIndex == 0 && maxProfit > 0)
	{
		if (b->location->type == LOCATION_BUYER)
		{
			*action = ACTION_SELL;
			
		}
		else if (b->location->type == LOCATION_SELLER || b->location->type == LOCATION_PETROL_STATION)
		{
			*action = ACTION_BUY;
		}
		else if (b->location->type == LOCATION_DUMP)
		{
			*action = ACTION_DUMP;
		}
		*n = scores[bestIndex][2];
		return;
	}

	// Or move somewhere else
	*action = ACTION_MOVE;
	*n = getMoveNFromDesiredIndex(b, bestIndex, nOfLocations);
	
	// Postproces - if no good mvoe found, try to do soemthng with remaining fuel
	// Exact same process as before, except no penalties are applied
	// and no fuel station scores are applied
	if (*action == ACTION_MOVE && *n == 0 && b->cargo != NULL)
	{
		end = b->location;
		index = 0;
		first = 1;
		for (locPtr locIter = b->location; locIter != end || first--; locIter = locIter->next, index++)
		{
			// If it's a seller, add score for its profitability
			if (locIter->type == LOCATION_SELLER)
			{
				scores[index][0] = getMaximumPossibleProfit(b, locIter, &(scores[index][2]), &(scores[index][3]), &sellersBuyer);
				scores[index][1] = LOCATION_SELLER;
			}
			// If it's a buyer, calculate any profits it has (depending on bot's cargo)
			if (locIter->type == LOCATION_BUYER)
			{
				scores[index][0] = getBuyerProfit(b, locIter, &(scores[index][2]));
				scores[index][1] = LOCATION_BUYER;
			}
			int numOfTurnsRequired = numOfTurnsForDist(b, abs(getDistanceToLocation(b, locIter))) + 1;
			scores[index][0] = scores[index][0] / (numOfTurnsRequired );
		}

		// Look for best index again hurhurhurhrurhuhurhurhurhurhurhurhurhuhurhur
		bestIndex = 0;
		maxProfit = 0; // Set minimum profit to 0, else don't move
		for (int i = 0; i < nOfLocations; i++)
		{
			if (scores[i][0] > maxProfit)
			{
				maxProfit = scores[i][0];
				bestIndex = i;
			}
		}
		// Return move:
		// If the best move is to do whatever we're on right now:
		if (bestIndex == 0 && maxProfit > 0)
		{
			if (b->location->type == LOCATION_BUYER)
			{
				*action = ACTION_SELL;

			}
			else if (b->location->type == LOCATION_SELLER || b->location->type == LOCATION_PETROL_STATION)
			{
				*action = ACTION_BUY;
			}
			else if (b->location->type == LOCATION_DUMP)
			{
				*action = ACTION_DUMP;
			}
			*n = scores[bestIndex][2];
			return;
		}
		*action = ACTION_MOVE;
		*n = getMoveNFromDesiredIndex(b, bestIndex, nOfLocations);
	}

	if (maxProfit == 0)
	{
		*n = 0;
		*action = ACTION_MOVE;
		if (*n == 0 && b->turns_left > 7)
		{
			//int x = 5 / *n;
			printf("MAX PROFIT 0 ?????????????\n");
			*n = -999999;
			//exit(1);
		}
		return;
	}
}
