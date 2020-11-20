#pragma once
typedef struct bot* botPtr;
typedef struct location* locPtr;
#define SCORES_DATA_POINTS 5

int getMaximumPossibleProfit(botPtr b, locPtr seller, int* quantityToBuy,
	int* distToBuyer, locPtr* desiredBuyer);
int getNumberOfLocations(botPtr b);
int getAverageFuelCost(locPtr l);
int getDistanceToBuyer(locPtr l, locPtr* buyer, int index);
int getDistanceToLocation(botPtr b, locPtr l);
int getBuyerProfit(botPtr b, locPtr buyer, int* quantityToSell);
int getWorthOfPetrolStation(botPtr b, locPtr servo, int servoIndex, int desiredIndex,
	int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS], int* fuelToBuy);
int getSellerRangePenalty(botPtr b, locPtr seller, int distance, int score, int nOfLocations);
int getBuyerRangePenalty(botPtr b, locPtr buyer, int distance, int score);

int getWorthOfDump(botPtr b, int desiredScore, int desiredIndex, locPtr desiredLoc,
	int index, int nOfLocations);

void flowPetrolBenefits(botPtr b, int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS], int servoIndex, locPtr servo);

int getAmountOfFuelToBuy(botPtr b, int desiredIndex, int nOfLocations, 
	int scores[nOfLocations][SCORES_DATA_POINTS]);