/*
 * processor.h
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 *  @brief: Gives abstraction to the main part of the application, the processor command line.
 *  Uses all other files in the project in order to properly parse, send, and communicate data throught a message
 *  buffer, and output and receive through USART
 */

#ifndef CASH_REGISTER_H_
#define CASH_REGISTER_H_

#include "usart.h"
#include "ctype.h"
#include <string.h>
#include "mfrc522.h"
#include "timing.h"

/*
 * Enum to keep track of which item was added
 */
typedef enum
{
	ITEM_NONE, ITEM_POPCORN, ITEM_SODA, ITEM_NACHOS
} ItemType;

/*
 * Enum to keep track of the different states the cash register could be
 */
typedef enum
{
	MAIN_MENU, MOVIE_MENU, CHECKOUT, CASH_MENU, THANK_YOU_SCREEN
} menu_states;

/*
 * To keep track of all the items ordered
 */
typedef struct
{
	uint8_t popcorn;
	uint8_t soda;
	uint8_t nachos;
	uint8_t total_items;
	uint8_t selected_items[3];
	uint8_t num_different_items;
	uint8_t movie_tickets;
} ordered_items;

/*
 * Struct to keep track of the cash the custumer has given
 */
typedef struct
{
	uint8_t cents;
	uint8_t nickels;
	uint8_t dimes;
	uint8_t quaters;
	uint8_t dollars;
	uint8_t five_dollars;
	uint8_t ten_dollars;
	uint8_t twenty_dollars;
	uint8_t fifty_dollars;
	uint8_t hundred_dollars;
} given_cash_tracker;

#define MAX_TIMES (5)

/*
 * Struct to take care of movie items and how many movies their are and which auditorium
 * they belong to
 */
typedef struct
{
	char *movie;
	uint8_t tickets_available[MAX_TIMES];
	uint8_t times_available;
	uint8_t auditorium[MAX_TIMES];
	char *times[MAX_TIMES];
} movie_t;

/*
 * clear message used in putty to clear the screen to refresh and make things less messy
 */
void clear();
/*
 * @brief Used to process characters user might input, currently only supports
 * up arrow, down arrow, left arrow, right arrow, and the delete character
 */
void process_char();
/*
 * @brief Displays the movie selection menu with available showtimes and ticket counts.
 */
void movie_menu();

/*
 * @brief: handles which menu should be prented based on the state of the Cash Register
 */
void print_menu();

/*
 * @brief: adds an item to the main menu, where custumer is ordering food or tickets
 */
void add_item();

/*
 * @brief: removes any items the custumer might regret ordering if they ordered something it
 * takes it back
 */
void remove_item();

/*
 * @brief: Prints out the cash payment menu screen where employee can add what money was given to them
 * by the customer
 */
void cash_payment_menu();

/*
 * Handles logic when a custumer selects to pay with cash
 */
void make_cash_payment();

/*
 * Handles logic when a custumer selects to pay with card, the MFRC522 is used to detect
 * when the custumer taps their card
 */
void make_card_payment();

/*
 * @brief: adds a movie ticket to the order, will add a ticket at a specified time
 * and store it
 */
void add_ticket();
/*
 * @brief: removes a movie ticket to the order, will remove the ticket if the custumer has already
 * added it to their cart else it does nothing
 */
void remove_ticket();

/*
 * @brief: adds cash user might add
 */
void add_cash_given();

/*
 * @brief: Removes any cash the custumer may have given
 */
void remove_cash_given();

#endif /* CASH_REGISTER_H_ */
