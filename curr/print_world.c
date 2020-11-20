#include "trader_bot.h"
#include "WorldAnalyzers.h"
#include "Utilities.h"
#include <stdio.h>

void print_world(struct bot *b)
{
    struct location* end = b->location;
    int first = 1;
	int nx = 0;
	locPtr s = 0;
    for(struct location* iter = b->location ; iter != end || first; first = 0, iter = iter->next)
    {
        switch(iter->type)
        {
        case LOCATION_START:
            printf("%s: start\n", iter->name);
            break;
        case LOCATION_SELLER:
            printf("%s: will sell %d units of %s for $%d\n", iter->name, iter->quantity, iter->commodity->name, iter->price);
			
			printf("\t\tMAX PROFIT %d ****\n", getMaximumPossibleProfit(b, iter, &nx, &nx, &s));
            break;
        case LOCATION_DUMP:
            printf("%s: dump\n", iter->name);
            break;
        case LOCATION_OTHER:
            printf("%s: other\n", iter->name);
            break;
        case LOCATION_BUYER:
            printf("%s: will buy %d units of %s for $%d\n", iter->name, iter->quantity, iter->commodity->name, iter->price);
            break;
        case LOCATION_PETROL_STATION:
            printf("%s: Petrol station %d units of available fuel for $%d\n", iter->name, iter->quantity, iter->price);
            break;
        }
    }
}

void print_world_S(struct bot *b, int nOfLocations, int scores[nOfLocations][SCORES_DATA_POINTS])
{
	struct location* end = b->location;
	int first = 1;
	int index = 0;
	for (struct location* iter = b->location; iter != end || first; first = 0, iter = iter->next)
	{
		switch (iter->type)
		{
		case LOCATION_START:
			printf("%s: start\n", iter->name);
			break;
		case LOCATION_SELLER:
			printf("%-15s: will sell %d units of %-15s for $%d\n", iter->name, iter->quantity, iter->commodity->name, iter->price);
			break;
		case LOCATION_DUMP:
			printf("%s: dump\n", iter->name);
			break;
		case LOCATION_OTHER:
			printf("%s: other\n", iter->name);
			break;
		case LOCATION_BUYER:
			printf("%-15s: will buy %d units of %-15s for $%d\n", iter->name, iter->quantity, iter->commodity->name, iter->price);

			break;
		case LOCATION_PETROL_STATION:
			printf("%-15s: Petrol station %d units of available fuel for $%d\n", iter->name, iter->quantity, iter->price);
			break;
		}
		printf(ANSI_COLOR_CYAN "SCORE %d  - Penalty: %d" ANSI_COLOR_RESET "\n", scores[index][0], scores[index][3]);
		index++;
	}
}
