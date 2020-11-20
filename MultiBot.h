#pragma once
typedef struct bot* botPtr;
typedef struct location* locPtr;
// Gets the number of bots in the locations within the region specified
int getNumOfBotsWithinRegion(locPtr l, int region);
int getNumOfBotsWorld(botPtr b);
int expectedServoDivisor(botPtr b, locPtr l, int nOfLocations, int* wFuel);
// Returns expected number of bots expected to use a buyer in the next turn
int getBuyerDivider(botPtr b, locPtr buyer, int* totalBuyerQ);
int numBotsAtLocWithCom(locPtr l, struct commodity* com, int* quantitySum);
int getNumOfEmptyBotsAtLoc(locPtr l);
int getNumOfEmptyBotsWithinRegion(botPtr b, locPtr l);
int getNumOfBotsWithinRegion_FuelCap(locPtr l, int region, int fuelCap, int* wantedFuel);
