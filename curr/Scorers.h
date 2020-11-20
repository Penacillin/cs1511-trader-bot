#pragma once
typedef struct bot* botPtr;
typedef struct location* locPtr;

int getPenaltyForFarnessIndex(botPtr b, int distance, int maxDistance);
int getPenaltyForFarnessDist(botPtr, int distance);