// Trader Bot interface
// https://cse.unsw.edu.au/~cs1511/assignment/trader_bot/

// DO NOT CHANGE THIS FILE
// DO NOT ADD LINES TO THIS FILE
// DO NOT DELETE LINES FROM THIS FILE
// DO NOT CHANGE THIS FILE


// header guard: https://en.wikipedia.org/wiki/Include_guard
// This avoids errors if this file is included multiple times
// in a complex source file setup

#ifndef TRADER_BOT_H
#define TRADER_BOT_H


// These constants are upper bounds on various features of the Trader Bot world
//
// You may not need to use any of these constants in your program.
//
// The actual values for these features will vary but will always be <= the constants below.

#define MAX_TURNS                        999
#define MAX_LOCATIONS                    1024
#define MAX_TRADER_BOTS                  256
#define MAX_COMMODITIES                  128
#define MAX_COMMODITY_NAME_CHARS         64
#define MAX_LOCATION_NAME_CHARS          64
#define MAX_SUPPLIED_BOT_NAME_CHARS      32
#define MAX_BOT_NAME_CHARS               (MAX_SUPPLIED_BOT_NAME_CHARS + 8)


// Description of the state of a single bot
//
//  name - unique string of between 1 and MAX_BOT_NAME_CHARS
//
//  cash - how much cash the bot has
//
//  fuel - how much fuel the bot has
//
//  location - pointer to struct representing bot's current location, never NULL
//
//  cargo - pointer to linked list of cargo structs representing bot's current cargo
//          pointer is NULL if the bot has no cargo
//
//  turns_left - turns left in game
//
//  fuel_tank_capacity - maximum amount of fuel bot can carry
//
//  maximum_move_distance - maximum number of location bot can move
//
//  maximum_cargo_weight - maximum weight of cargo bot can carry
//
//  maximum_cargo_volume - maximum volume of cargo bot can carry
//

struct bot {
    char            *name;
    int             cash;
    int             fuel;
    struct location *location;
    struct cargo    *cargo;
    int             turns_left;
    int             fuel_tank_capacity;
    int             maximum_move;
    int             maximum_cargo_weight;
    int             maximum_cargo_volume;
};

#define LOCATION_START            0
#define LOCATION_SELLER           1
#define LOCATION_BUYER            2
#define LOCATION_PETROL_STATION   3
#define LOCATION_DUMP             4
#define LOCATION_OTHER            5


// Description of a location in the trader bot world
//
//  name - unique string of between 1 and MAX_LOCATION_NAME_CHARS
//
//  type - one of LOCATION_START, LOCATION_SELLER, LOCATION_BUYER, LOCATION_PETROL_STATION, LOCATION_DUMP
//
//  commodity - if type is LOCATION_SELLER or LOCATION_BUYER, commodity will point to
//              a commodity struct indicating what this location sells/buys.
//              if type is not LOCATION_SELLER or LOCATION_BUYER, commodity == NULL
//
//  price - if type is LOCATION_SELLER, LOCATION_BUYER or LOCATION_PETROL_STATION
//          will be a positive number indicating the price at which this location sells/buys
//
//  quantity - if type is LOCATION_SELLER, LOCATION_BUYER or LOCATION_PETROL_STATION
//             will be a non-negative number indicating the amount this location has left to sell
//
//  bots - pointer to a linked list of bots current at this location
//
//
//  next - pointer to struct representing next location, never NULL
//         The next fields of locations link them in a circular list
//
//  previous - pointer to struct representing previous location, never NULL
//             The previous fields of locations link them in a circular list
//             but in the reverse direction to next fields.
//

struct location {
    char                *name;
    int                 type;
    struct commodity    *commodity;
    int                 price;
    int                 quantity;
    struct location     *next;
    struct location     *previous;
    struct bot_list     *bots;
};


// A parcel of items of one type on board a truck
// There will be at most one parcel of items for any commodity
struct cargo {
    struct cargo        *next;
    struct commodity    *commodity;
    int                 quantity;
};


// a struct representing a commodity
// only one instance of this struct is created for each commodity
//
//  name - unique string of between 1 and MAX_COMMODITY_NAME_CHARS
//
//  weight - weight of a single item of this commodity
//
//  volume - volume of a single item of this commodity

struct commodity {
    char   *name; 
    int    weight; 
    int    volume; 
};


// linked list of bots
//
// bot - pointer to a struct representing a bot
//
// next - points to remainder of list
//        next is NULL if there are no more bots in list

struct bot_list {
    struct bot      *bot;
    struct bot_list *next;
};


// get_bot_name is called once at the start of the game to get a bot's name
// get_bot_name should return a string of between 1..MAX_SUPPLIED_BOT_NAME_CHARS
// The supplied chars for the name should only include [a-zA-Z0-9] '-' '_' ' ' 
// It is used to set the name field of the bot's struct.  The name field will
// have a numeric suffix added if necessary to make it unique.

char *get_bot_name(void);


#define ACTION_MOVE           0
#define ACTION_BUY            1
#define ACTION_SELL           2
#define ACTION_DUMP           3


// get_action is called once every turn to get a bot's action
//
// It is given b, a pointer to the struct representing the current state of the bot
//
// It should set two values indicating the bot's move for this turn.
//
// *action must be set to one of ACTION_MOVE, ACTION_BUY, ACTION_SELL, ACTION_DUMP
// 
//   for ACTION_MOVE *n should be set to an integer indicating how many locations to move
//                    a negative integer  indicates a move in the reverse direction
//
//   for ACTION_BUY, ACTION_SELL *n should be set to how many items the bot wishes to buy/sell
//
//   for ACTION_DUMP *n need not be set

void get_action(struct bot *b, int *action, int *n);


//
// These function are used in lab exercises
//
// You do not have to define print_world or nearest_fuel in the assignment 
// but you can not have a function of the same name with a different prototype

void print_world(struct bot *b);
int nearest_fuel(struct bot *b);

// DO NOT CHANGE THIS FILE
// DO NOT ADD LINES TO THIS FILE
// DO NOT DELETE LINES FROM THIS FILE
// DO NOT CHANGE THIS FILE

#endif
