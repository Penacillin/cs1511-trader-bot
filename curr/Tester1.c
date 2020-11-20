#include <stdio.h>
#include "trader_bot.h"



int main(void) {

	struct commodity c[] = {
		// name                   weight    volume
		{ "Beer",                  1300,     2012 },
		{ "Computers",            18709,     7030 },
		{ "Books",                 6713,    11488 },
		{ "Mars Bars",              195,      101 },
		{ "Televisions",          20000,     4662 },
	};

	struct location l[] = {
		// name                     type             commodity  price quantity  next  previous bots
		{ "CSE",               LOCATION_START,           NULL,      0,      0, &l[1], &l[29], NULL },
		{ "BP",                LOCATION_PETROL_STATION,  NULL,    113,     56, &l[2], &l[0], NULL },
		{ "Lenovo",            LOCATION_SELLER,         &c[0],   1864,     49, &l[3], &l[1], NULL },
		{ "Quadrangle",        LOCATION_OTHER,           NULL,      0,      0, &l[4], &l[2], NULL },
		{ "James Squires",     LOCATION_SELLER,         &c[1],     99,   1150, &l[5], &l[3], NULL },
		{ "Batch Brewing",     LOCATION_SELLER,         &c[1],     82,   2014, &l[6], &l[4], NULL },
		{ "Sony",              LOCATION_SELLER,         &c[2],    818,    112, &l[7], &l[5], NULL },
		{ "O'Reilly",          LOCATION_SELLER,         &c[3],    175,   1587, &l[8], &l[6], NULL },
		{ "J&B Hifi",          LOCATION_BUYER,          &c[0],   2535,     39, &l[9], &l[7], NULL },
		{ "Umart",             LOCATION_BUYER,          &c[0],   3001,     46, &l[10], &l[8], NULL },
		{ "Harvey-Norman",     LOCATION_BUYER,          &c[2],   1089,    219, &l[11], &l[9], NULL },
		{ "Coles",             LOCATION_BUYER,          &c[4],    358,    277, &l[12], &l[10], NULL },
		{ "Caltex",            LOCATION_PETROL_STATION,  NULL,    160,    105, &l[13], &l[11], NULL },
		{ "Prentice-Hall",     LOCATION_SELLER,         &c[3],    154,    580, &l[14], &l[12], NULL },
		{ "Regent Hotel",      LOCATION_BUYER,          &c[1],    164,    581, &l[15], &l[13], NULL },
		{ "LG",                LOCATION_SELLER,         &c[2],    672,    409, &l[16], &l[14], NULL },
		{ "Addison-Wesley",    LOCATION_SELLER,         &c[3],    139,   1177, &l[17], &l[15], NULL },
		{ "IGA UNSW",          LOCATION_BUYER,          &c[4],    327,    215, &l[18], &l[16], NULL },
		{ "Physics Lawn",      LOCATION_OTHER,           NULL,      0,      0, &l[19], &l[17], NULL },
		{ "Apple",             LOCATION_SELLER,         &c[0],   1069,    112, &l[20], &l[18], NULL },
		{ "Dell",              LOCATION_SELLER,         &c[0],   1651,     40, &l[21], &l[19], NULL },
		{ "Aldi",              LOCATION_BUYER,          &c[4],    367,    431, &l[22], &l[20], NULL },
		{ "Mars",              LOCATION_SELLER,         &c[4],    164,    795, &l[23], &l[21], NULL },
		{ "UNSW Bookshop",     LOCATION_BUYER,          &c[3],    273,    872, &l[24], &l[22], NULL },
		{ "Whitehouse",        LOCATION_BUYER,          &c[1],    134,   1017, &l[25], &l[23], NULL },
		{ "Good Guys",         LOCATION_BUYER,          &c[2],   2000,     25, &l[26], &l[24], NULL },
		{ "MSY",               LOCATION_BUYER,          &c[0],   1800,     35, &l[27], &l[25], NULL },
		{ "Racecourse",        LOCATION_OTHER,           NULL,      0,      0, &l[28], &l[26], NULL },
		{ "Dump",              LOCATION_DUMP,            NULL,      0,      0, &l[29], &l[27], NULL },
		{ "4 Pines",           LOCATION_SELLER,         &c[1],     82,   1703, &l[0], &l[28], NULL },
	};

	struct bot bots[] = {
		// name cash fuel turns_left location cargo fuel_tank_capacity maximum_move maximum_cargo_weight maximum_cargo_volume
		{ "T-101", 107117, 45, &l[7], NULL, 135, 45, 6, 1000000, 1000000 },
	};

	struct bot_list bot_lists[] = {
		// bot next
		{ &bots[0], NULL },
	};

	l[7].bots = &bot_lists[0];
	int action = 0;
	int n = 0;
	get_action(bots, &action, &n);

	printf("*** MOVE: %d , n: %d ***\n", action, n);
	return 0;
}
