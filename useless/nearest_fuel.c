#include "trader_bot.h"

int nearest_fuel(struct bot *b)
{
    
    int first = 1;
    int counter = 0;
    int dist = 0;
    struct location* iterBackward = b->location;
    for(struct location* iterForward = b->location;
     (iterForward != iterBackward && iterForward->next != iterBackward && dist == 0) || first;
      first = 0, iterForward = iterForward->next, iterBackward = iterBackward->previous, counter++)
    {
        if(iterForward->type == LOCATION_PETROL_STATION)
            return dist = counter;
        else if(iterBackward->type == LOCATION_PETROL_STATION)
            return dist = (-1)*counter;
        
    }
    
    return dist;
}
