#pragma once
typedef struct location* locPtr;
typedef struct bot* botPtr;

int nearest_fuel(struct bot *b);
int nearestFuelLoc(locPtr l, locPtr* servo, int min);
int getRemainingWeight(struct bot* b);
int getRemainingVolume(struct bot* b);
int getQuantityForVolWeight(struct commodity* com, int vol, int weight);
int getMaximumBuyQuantity(struct bot* b, locPtr seller, locPtr buyer);
int getMaximumBuyQuantityRaw(struct bot* b, int seller, int buyer, struct commodity* com);

int getDistanceBetweenIndex(int a, int b, int size);
int getDistanceBetweenLocs(locPtr a, locPtr b);
int getMoveNFromDesiredIndex(botPtr b, int desiredIndex, int nOfLocations);
int getFuelLeftInWorld(botPtr b);
int numOfTurnsForDist(botPtr b, int dist);

int getClosestDump(botPtr b);

int getQuantityOfCommodity(botPtr b, struct commodity* com);
int getRemainingBuyQuantity(botPtr b, struct commodity* com);
int getRemainingFloatingQuantityInWorld(botPtr b, struct commodity* com);

int getMaxQuantityIntoCargo(botPtr b, struct commodity* com);

int mmin(int a, int b);
int mmax(int a, int b);

int getRealIndexFromCircularIndex(int index, int nOfLocations);

#define SCORES_DATA_POINTS 5
void print_world_S(struct bot *b, int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS]);

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"