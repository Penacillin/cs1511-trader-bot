#include "WorldAnalyzers.h"
#include "trader_bot.h"
#include "Utilities.h"
#include "Scorers.h"
#include "MultiBot.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_N_BUYERS_PER_SELLER 5000

typedef struct bot* botPtr;
typedef struct location* locPtr;
int getDistanceToLocation(botPtr b, locPtr l) {
	
	int counter = 0; // Should be 1, but gets more profit with 0 even tho its wrong for some reason don't ask me why i relaly have no clue
	if (b->location == l)
		return 0;
	locPtr iterBackward = b->location->previous;
	locPtr iterForward = b->location->next;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward);
		iterForward = iterForward->next, iterBackward = iterBackward->previous, counter++)
	{
		if (iterForward == l)
			return counter;
		else if (iterBackward == l)
			return (-1)*counter;

	}

	if (iterForward == l)
		return counter;
	else if (iterBackward == l)
		return (-1)*counter;

	printf("getDIstanceToLocation fucked up exiting\n");
	exit(1);
}

// Takes seller l, then finds the closest buyer and puts it 
// into buyer and returns the distance between them
int getDistanceToBuyer(locPtr l, locPtr* buyer, int index)
{
	int counter = 1;
	int rank = 0;
	struct location* iterBackward = l->previous;
	struct location* iterForward = l->next;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward);
		iterForward = iterForward->next, iterBackward = iterBackward->previous, counter++)
	{
		if (iterForward->type == LOCATION_BUYER
			&& strcmp(iterForward->commodity->name, l->commodity->name) == 0) 
		{
			if (rank == index)
			{
				*buyer = iterForward;
				return counter;
			}
			else
				rank++;
		}			
		if (iterBackward->type == LOCATION_BUYER
			&& strcmp(iterBackward->commodity->name, l->commodity->name) == 0)
		{			
			if (rank == index)
			{
				*buyer = iterBackward;
				return (-1)*counter;
			}
			else
				rank++;
		}
	}
	if (iterForward->type == LOCATION_BUYER
		&& strcmp(iterForward->commodity->name, l->commodity->name) == 0)
	{
		if (rank == index)
		{
			*buyer = iterForward;
			return counter;
		}
		else
			rank++;
	}
	if (iterBackward->type == LOCATION_BUYER
		&& strcmp(iterBackward->commodity->name, l->commodity->name) == 0)
	{
		if (rank == index)
		{
			*buyer = iterBackward;
			return (-1)*counter;
		}
		else
			rank++;
	}
	return 0;
	printf("getDistanceToBuyer fucked up exiting\n");
	exit(1);
}

// Get profit based on current location of bot and a seller
// Takes into account maximum quantity that can be bought,
// Distance between buyer and seller
int getMaximumPossibleProfit(botPtr b, locPtr seller, int* quantityToBuy, 
	int* distToBuyer, locPtr* desiredBuyer)
{
	if (seller->type != LOCATION_SELLER)
	{
		printf("oi mate \n");
		exit(1);
	}
	locPtr buyer = 0;
	int scoreSum = 0; 
	// Gets buyer and distance to closest buyer
	int distance = 0;
	int buyerCount = 0;


	// Buyer summar for seller:includes:
	//  quantity, price, distance, score, index
	int buyerSummary[MAX_N_BUYERS_PER_SELLER][5];
	int mostProfitableScore = 0;

	float qDiv = getNumOfEmptyBotsWithinRegion(b, seller) *2.0 / 3.0;
	int willBeBought = qDiv * getMaxQuantityIntoCargo(b, seller->commodity);
	if (qDiv  < 1) qDiv = 1;
	int newSellerQuantity = mmax(seller->quantity / qDiv, seller->quantity -  willBeBought);
	printf("Seller %s: , , buyerDivider: %f , newSellerQ %d (from %d)\n", seller->name, qDiv, newSellerQuantity, seller->quantity);
	int fuelLeftInWorld = getFuelLeftInWorld(b);
	while((distance = getDistanceToBuyer(seller, &buyer, buyerCount)) != 0)
	{
		//printf("distance %d to buyer %s from seller %s \n", distance, buyer->name, seller->name);
		fflush(stdout);
		//printf("Distance to buyer is %d\n", distance);
		if (buyer == 0)
		{
			fprintf(stderr, "Buyer not found\n");
			exit(1);
		}

		buyerSummary[buyerCount][1] = buyer->price;
		buyerSummary[buyerCount][2] = distance;
		buyerSummary[buyerCount][4] = getDistanceToLocation(b, buyer);

		// Using buyer and seller, get maximum quantity that can be bought
		// Factors in cargo available too
		int maxQuantity = 0;
		maxQuantity = getMaximumBuyQuantity(b, seller, buyer);

		// Reduce maximum quantity based on cash available
		maxQuantity = mmin(maxQuantity, b->cash / seller->price);
		buyerSummary[buyerCount][0] = maxQuantity;

		// Make a guessed quantity based on other bots
		int guessedQuantity = 0;
		int newBuyerQuantity = buyer->quantity / getBuyerDivider(b, buyer);
		//printf("Buyer %s: maxQ: %d, , buyerDivider: %d\n", buyer->name,
		//	maxQuantity, getBuyerDivider(b, buyer));		
		guessedQuantity = mmin(b->cash/seller->price,
			getMaximumBuyQuantityRaw(b, newSellerQuantity, newBuyerQuantity, seller->commodity));

		//printf("maxq: %d, guessQ: %d\n", maxQuantity, guessedQuantity);
			

		// Multiply quantity by profit made per item to get total profit
		int directProfit = guessedQuantity * (buyer->price - seller->price);

		//printf("SELLER %d BUYER %d | %d FARNESS PENALTY %d \n", seller->price, buyer->price, distance,  getPenaltyForFarnessDist(b, distance));

		int score = directProfit;	

		//printf("num %d of turns for %d, %d\n", score, numOfTurnsRequired, totalTransactionTravel);
		//score = score * 3 / 2;

		// Reduce profit by petrol costs between buyer and seller AND number of turns
		int numOfTurnsRequired = numOfTurnsForDist(b, abs(distance));
		
		score -= getPenaltyForFarnessDist(b, distance);
		score /= numOfTurnsRequired + 1;

		// If theres not enough turns for buyer
		if (numOfTurnsRequired > b->turns_left)
			score = 0;
		if (b->fuel < abs(distance) && b->turns_left < 4)
			score = 0;
		// If the buyer is impossibly out of reach, zero it
		if (fuelLeftInWorld + b->fuel < abs(distance))
			score = 0;

		buyerSummary[buyerCount][3] = score;	

		if (score > mostProfitableScore)
		{
			mostProfitableScore = score;
			*distToBuyer = buyerSummary[buyerCount][2];
			*desiredBuyer = buyer;
			*quantityToBuy = maxQuantity;
		}

		buyer = 0;
		buyerCount++;
	}
	//printf("seller %s has %d buyers\n", seller->name, buyerCount);

	return mostProfitableScore;
}

// Get the penalty for sellers based on the bot pushing the fuel/turn limits
int getSellerRangePenalty(botPtr b, locPtr seller, int distance, int score, int nOfLocations)
{
	printf("penalzing %s:: " ,seller->name);
	int numTurnsRequired = numOfTurnsForDist(b, distance);
	if (distance > 0 && b->turns_left < 4 + numTurnsRequired)
	{
		printf("seller can't be used in turns\n");
		return score; // Penalize the whole score if the seller can't be utilized within turns left
	}
	if (distance == 0 && b->turns_left < 3 + numTurnsRequired)
	{
		printf("seller can't be used in turns\n");
		return score;
	}
	
	// If the bot has more fuel than the world #YOLO
	//if (b->fuel > 2 * getFuelLeftInWorld(b))
	//	return 0;

	// if its outside fuel range, don't bother
	if (distance > b->fuel)
	{
		printf("seller too far\n");
		return score;
	}
	locPtr servo = 0;
	

	// if it would be stranded afterwards, drop its score
	int distToNearestFuelLoc = nearestFuelLoc(seller, &servo, mmax(distance,0));

	if (abs(distToNearestFuelLoc) + distance > b->fuel)
	{
		printf("seller stranded by nearest fuel loc\n");
		return score;
	}

	// Get the expected number of bots that will probably buy fuel here
	
	int wantedFuelByBots = 0;
	int guessQ = 0;
	if (servo)
	{
		guessQ = servo->quantity;
		int servoBotDivisor = expectedServoDivisor(b, servo, nOfLocations, &wantedFuelByBots);
		printf("expected divisor for %s is %d (max moves %d) wnatedFuel: %d (Guess quantity of fuel: %d)\n",
			servo->name, servoBotDivisor, b->maximum_move, wantedFuelByBots, guessQ);

		guessQ = mmax(servo->quantity / servoBotDivisor, servo->quantity - wantedFuelByBots);
	}

	if (servo != 0 && abs(distToNearestFuelLoc) + distance > guessQ + b->fuel)
	{
		printf("nearest fuel loc not enough fuel stranded \n");
		return score;
	}

	// if theres plenty of turns left and enough fuel left in the world,
	// make sure this seller isn't connected to a useless servo
	if (b->turns_left > 10 && getFuelLeftInWorld(b) > b->fuel_tank_capacity)
	{
		locPtr decentServo = 0;
		int decentFuel = mmax(b->fuel_tank_capacity - b->fuel, getNumOfBotsWorld(b) * 2);
		int distToDecentFuel = nearestFuelLoc(seller, &decentServo, decentFuel);
		printf("Seller (%s) penalizer: dist to decent fuel: %d (%d)\n", seller->name, distToDecentFuel, decentFuel);
		
		if (decentServo)
		{
			guessQ = decentServo->quantity;
			int servoBotDivisor = expectedServoDivisor(b, decentServo, nOfLocations, &wantedFuelByBots);
			printf("expected divisor for %s is %d (max moves %d) wnatedFuel: %d (Guess quantity of fuel: %d)\n",
				decentServo->name, servoBotDivisor, b->maximum_move, wantedFuelByBots, guessQ);

			guessQ = mmax(decentServo->quantity / servoBotDivisor, decentServo->quantity - wantedFuelByBots);
		}

		if (decentServo && guessQ < b->fuel_tank_capacity - b->fuel)
			return score;
		if (decentServo && distToDecentFuel + distance > b->fuel)
			return score;
	}

	// if the buyer is out of range after going to seller
	// add a penalty 
	return 0;
}

// Returns the profit to be made at a buyer (only exists if bot has appropriate commodity)
int getBuyerProfit(botPtr b, locPtr buyer, int* quantityToSell)
{
	if (!b || !buyer || buyer->type != LOCATION_BUYER)
	{
		printf("oi mate\n");
		exit(1);
	}

	// Check if we have the commodity for this buyer
	
	// If no cargo, return 0 profit
	if (!b->cargo)
		return 0;

	// Loop through cargo to find matching commodity
	for (struct cargo* cargoIter = b->cargo; cargoIter != NULL; cargoIter = cargoIter->next)
	{
		if (!strcmp(cargoIter->commodity->name, buyer->commodity->name))
		{
			// If the commodity matches, find profit
			int buyerDivider = 0;
			
			// Get quantity divider for buyer based on nearby bots with commodity
			buyerDivider += getBuyerDivider(b, buyer);
			
			int buyerQuantity = buyer->quantity;
			int guessQuantity = mmin(cargoIter->quantity, buyerQuantity / buyerDivider);
			// Calculate profit taking into account other bots
			int profit = buyer->price * guessQuantity;

			// attempt to sell all
			*quantityToSell = mmin(cargoIter->quantity, buyerQuantity);
			printf("***Buyer: %s  Buyer divider %d (%d -> %d)\n", buyer->name, buyerDivider,
				*quantityToSell, guessQuantity);

			int numOfTurnsRequired = numOfTurnsForDist(b, abs(getDistanceToLocation(b, buyer)));			
			if (numOfTurnsRequired == 0) numOfTurnsRequired = 1;

			//profit /= numOfTurnsRequired;

			return profit;

		}
	}

	// If we didn't find any matching commodites, there are no profits at
	// this buyer
	return 0;

}


// Gets the penalty of a buyer for pushing the limits of the fuel
int getBuyerRangePenalty(botPtr b, locPtr buyer, int distance, int score)
{
	if (!b || !buyer || buyer->type != LOCATION_BUYER)
	{
		printf("oi mate\n");
		exit(1);
	}

	if (b->location == buyer)
	{
		return 0;
	}
	
	if (distance > 0 && b->turns_left < 2)
	{
		return score; // Penalize the whole score if the buyer can't be utilized within turns left
	}

	// if its outside fuel range, don't bother
	if (distance > b->fuel)
		return score;

	// If the bot has more fuel than the world #YOLO
	if (b->fuel > 2 * getFuelLeftInWorld(b))
		return 0;

	// if it would be stranded afterwards, drop its score
	// Unless theres no fuel left in the world to do so anyways,
	// so don't penalize
	locPtr servo;
	int distToNearestFuelLoc = nearestFuelLoc(buyer, &servo, mmax((b->fuel_tank_capacity - b->fuel) / 2 ,mmax(distance, getNumOfBotsWorld(b) * 2)));
	
	if ((abs(distToNearestFuelLoc) + distance > b->fuel
		|| abs(distToNearestFuelLoc) + distance > getFuelLeftInWorld(b))
		&& b->turns_left >= 4)
		return score;

	// Even if theres fuel available, check if a refuel path will take too many turns
	//int turnsRequired = numOfTurnsForDist(b, distance) + 1;


	// if the buyer is out of range after going to seller
	// add a penalty 
	return 0;
}

// Get the worthiness of going to a petrol station
// Base it on fuel left, and distance to station
// and distance to high profitability area
int getWorthOfPetrolStation(botPtr b, locPtr servo, int servoIndex, int desiredIndex,
							int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS],
							int* fuelToBuy)
{
	// Empty servos are useless or servos that are too far
	if (servo->quantity == 0
		|| abs(getDistanceBetweenIndex(0, servoIndex, nOfLocations)) > b->fuel)
	{
		return 0;
	}

	if (scores[desiredIndex][3] == 0)
	{
		printf("fuel station useless because desiredIndex is safe\n");
	}
	
	// Maximum amount of fuel that could ever possibly be required for the rest of the game
	// If bot has more fuel than this, petrol station has 0 worth
	int maxFuelRequired = b->turns_left * b->maximum_move;
	if (b->fuel > maxFuelRequired)
		return 0;

	// Get the expected number of bots that will probably buy fuel here
	int wantedFuelByBots = 0;
	int guessQ = servo->quantity;
	int servoBotDivisor = expectedServoDivisor(b, servo, nOfLocations, &wantedFuelByBots);
	printf("expected divisor for %s is %d (max moves %d) wnatedFuel: %d (Guess quantity of fuel: %d)\n",
		servo->name, servoBotDivisor, b->maximum_move, wantedFuelByBots,guessQ);

	guessQ = mmax(servo->quantity / servoBotDivisor, servo->quantity - wantedFuelByBots);

	// if we're already on a servo, and our fuel is low, just buy fuel
	if (b->location == servo && b->turns_left > 15 && b->fuel < b->fuel_tank_capacity / 10
		&& guessQ > b->fuel_tank_capacity - b->fuel)
	{
		*fuelToBuy = b->maximum_move * 2 / 5 * b->turns_left;
		return scores[desiredIndex][3] * 2;
	}

	// If the bot isn't planning to move, getting more fuel is retarded? i think?
	if(desiredIndex == 0)
		return 0;
	
	// Find location where the bot wants to end up
	locPtr dest = b->location;
	int desiredDistance = getDistanceBetweenIndex(0, desiredIndex, nOfLocations);
	for (int i = 0; i < abs(desiredDistance); i++)
	{
		if (desiredDistance > 0)
			dest = dest->next;
		else
			dest = dest->previous;
	}
	printf("desired Location name is %s (%d) \n", dest->name, desiredIndex);

	int score = 0;
	// Based on the desired place to go, check if theres enough moves left
	// to make travelling to petrol worth it
	// TODO: change this to take into account how far each move is
	int minimumUsefulMoves = 0;


	if (scores[desiredIndex][1] == LOCATION_SELLER)
	{
		minimumUsefulMoves = 6;//move,fuel,move,buy,move,sell
	}
	if (scores[desiredIndex][1] == LOCATION_BUYER)
	{
		minimumUsefulMoves = 4;//move,fuel,move,buy
	}
	if (b->location == servo)
		minimumUsefulMoves--;

	// If there are multiple bots for a station
	if (servoBotDivisor > 1)
	{
		// If the maximum amount of fuel required is less than than capacity
		if (maxFuelRequired < b->fuel_tank_capacity)
		{
			// If the wanted amount of fuel is unavailable
			if (b->fuel_tank_capacity - maxFuelRequired - b->fuel > servo->quantity)
			{
				printf("station %s zeroed 2\n", servo->name);
				return 0;
			}
		}
		else
		{
			// If there isn't enough fuel to refuel completely (random assumption l0lz yolo)
			if (mmin(b->maximum_move * 2 / 5 * b->turns_left,
				b->fuel_tank_capacity - b->fuel) > servo->quantity && b->turns_left > 20)
			{
				printf("station %s zeroed 3\n", servo->name);
				return 0;
			}
		}
	}

	int distToNearestServo = 0;	
	locPtr servoClosestToDest = NULL;
	{
		distToNearestServo = abs(nearestFuelLoc(dest, &servoClosestToDest, abs(desiredDistance)));
		distToNearestServo = abs(nearestFuelLoc(dest, &servoClosestToDest, mmax(abs(desiredDistance) + distToNearestServo, getNumOfBotsWorld(b) * 3)));
	}

	//  Check if the fuel at the fuel station is enough to supply the journey
	int minFuel = 0;
	locPtr servoServo;
	if (getFuelLeftInWorld(b) < getNumOfBotsWithinRegion_FuelCap(servo, nOfLocations / 3, 20, NULL) * b->turns_left  * b->maximum_move / 2)
		minFuel = 0;
	else
		minFuel = distToNearestServo;
	if (guessQ + b->fuel < b->fuel_tank_capacity / 2
		&& guessQ + b->fuel < abs(nearestFuelLoc(servo, &servoServo, abs(desiredDistance)) * 2 ))
	{
		printf("FUEL::: %s not enough fuel to supply journey (div: %d, dist: %d, desiredDistance: %d)\n",
			servo->name, servoBotDivisor, distToNearestServo, desiredDistance);
		return 0;
	}

	// If the desired place got penalized, and theres enough turns left,
	// Then add the desired score to the fuel station
	if (scores[desiredIndex][3] > 0 && b->turns_left > minimumUsefulMoves)
	{
		int numOfTurnsRequired = numOfTurnsForDist(b, getDistanceBetweenIndex(servoIndex, desiredIndex, nOfLocations));
		if (numOfTurnsRequired == 0) numOfTurnsRequired = 1;
		printf("adding  %s the score %d - %d\n", servo->name, scores[desiredIndex][3], getPenaltyForFarnessDist(b, abs(getDistanceBetweenIndex(servoIndex, desiredIndex, nOfLocations))));

		int stationWorthMultiplier = mmin((b->turns_left - numOfTurnsRequired - 2) / 4, (b->fuel_tank_capacity - b->fuel) / b->maximum_move / 2);
		printf("station %s multiplier is %d\n", servo->name, stationWorthMultiplier);

		score += scores[desiredIndex][3] 
			* mmin((b->turns_left - numOfTurnsRequired - 2) / 4, (b->fuel_tank_capacity-b->fuel)  / b->maximum_move / 2 )
			- getPenaltyForFarnessDist(b, abs(getDistanceBetweenIndex(servoIndex, desiredIndex, nOfLocations)));
		scores[desiredIndex][0] = 0;
		//int numOfTurnsRequired = numOfTurnsForDist(b, abs(getDistanceBetweenIndex(0, servoIndex, nOfLocations)))
			//+ numOfTurnsForDist(b, abs(getDistanceBetweenIndex(servoIndex, desiredIndex, nOfLocations)));
		
		score /= numOfTurnsRequired;
	}
	printf("desired distance for fuel station %d (score: %d), moves left %d, nearest fuel %d\n",
		desiredDistance, scores[desiredIndex][3],b->turns_left, distToNearestServo);	

	// Buy the amount of fuel that should be enough for the rest of the game
	minFuel = getDistanceBetweenIndex(servoIndex, desiredIndex, nOfLocations)
		+ getDistanceBetweenIndex(0, servoIndex, nOfLocations);

	*fuelToBuy = mmax(b->maximum_move * 2 / 5 * b->turns_left, minFuel);

	return score;
}

// Returns number of locations in the world
int getNumberOfLocations(botPtr b)
{
	struct location* end = b->location;
	int first = 1;
	int count = 0;
	for (struct location* iter = b->location; iter != end || first; first = 0, iter = iter->next)
	{
		count++;
	}

	return count;
}

// Gets the average fuel cost around the world
int getAverageFuelCost(locPtr l)
{
	int quantitySum = 0;
	int priceSum = 0;

	locPtr iterBackward = l->previous;
	locPtr iterForward = l->next;
	for (;
		(iterForward != iterBackward && iterForward->next != iterBackward);
		iterForward = iterForward->next, iterBackward = iterBackward->previous)
	{
		if (iterForward->type == LOCATION_PETROL_STATION)
		{
			quantitySum += iterForward->quantity;
			priceSum += iterForward->price * iterForward->quantity;
		}
		if (iterBackward->type == LOCATION_PETROL_STATION)
		{
			quantitySum += iterBackward->quantity;
			priceSum += iterBackward->price * iterBackward->quantity;
		}

	}

	if (iterForward->type == LOCATION_PETROL_STATION)
	{
		quantitySum += iterForward->quantity;
		priceSum += iterForward->price * iterForward->quantity;
	}
	else if (iterBackward->type == LOCATION_PETROL_STATION)
	{
		quantitySum += iterBackward->quantity;
		priceSum += iterBackward->price * iterBackward->quantity;
	}

	// if no fuel left in the world, the average cost of fuel is 0
	if (quantitySum == 0)
		return 0;

	//printf("Average %d %d fuel cost : %d\n", priceSum, quantitySum, priceSum / quantitySum);
	return priceSum / quantitySum;
}

int getWorthOfDump(botPtr b, int desiredScore, int desiredIndex, locPtr desiredLoc,
	int index, int nOfLocations)
{
	// If our cargo is empty, 0 worth
	if (b->cargo == NULL)
		return 0;

	if (desiredLoc->type != LOCATION_SELLER)
		return 0;

	// get distance between dump and where we want to go
	int distToDump = abs(getDistanceBetweenIndex(desiredIndex, index, nOfLocations));
	int distBotToDump = abs(getDistanceBetweenIndex(0, index, nOfLocations));
	locPtr servo = 0;
	if (b->fuel < mmin(distToDump + distBotToDump, mmin(nOfLocations / 4, b->fuel_tank_capacity / 2)))
	{	
		return 0;
	}

	
	int score = 0;
	for (struct cargo* cargoIter = b->cargo; cargoIter != NULL; cargoIter = cargoIter->next)
	{
		if (desiredLoc->commodity != cargoIter->commodity && b->turns_left > 10)
		{
			int q = getQuantityForVolWeight(desiredLoc->commodity, cargoIter->quantity * cargoIter->commodity->volume,
				cargoIter->quantity * cargoIter->commodity->weight);

			int remainingBuyInWorld = getRemainingBuyQuantity(b, cargoIter->commodity);
			int remainingQInBots = getRemainingFloatingQuantityInWorld(b, cargoIter->commodity);

			int div = 0;
			if (remainingQInBots + cargoIter->quantity > remainingBuyInWorld)
				div = 2;
			else
				div = 8;
			
			score += q * desiredLoc->price * b->turns_left / div;
			score -= distBotToDump * getAverageFuelCost(desiredLoc);
			int turnsRequired = numOfTurnsForDist(b, distBotToDump +  distToDump);
			score /= turnsRequired + 1;
		}
	}
	return score;
}


void flowPetrolBenefits(botPtr b, int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS],
	int servoIndex, locPtr servo)
{
	int points = servo->quantity * servo->price / 300;

	for (int i = 1; i < b->maximum_move; i++)
	{		
		int rIndex = getRealIndexFromCircularIndex(servoIndex + i, nOfLocations);
		//printf("flowing on %d\n", scores[rIndex][0] * points / i * b->fuel / b->fuel_tank_capacity);
		//scores[rIndex][0] += scores[rIndex][0] * points / i * b->fuel / b->fuel_tank_capacity;
	}
}