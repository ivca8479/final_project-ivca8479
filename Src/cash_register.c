/*
 * processor.c
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 *
 *      @brief
 *      Main file that controls the cash register as a state machine
 *      Their is main menu state where the employee can add items to the order
 *      The movie menu where employee can help custumer pick a movie at a time
 *      The cash menu where user adds cash given to them by the user
 *      And the thank you menu which is the reciept that will be given to the user
 */

#include "cash_register.h"

#define BUFFER_SIZE	(1000)
#define MAX_ARGC		(10)
// To account for the '<<' characters, always need to consider them
#define MIN_CHARS		(2)
// led arguments can only be on or off
#define MAX_LED_PARAMS (2)

#define DELETE_CHAR (127)

#define UP_ARROW	(38)
#define DOWN_ARROW	(39)

#define POPCORN_PRICE (3.0)
#define SODA_PRICE (1.0)
#define NACHOS_PRICE (4.0)
// 7% tax
#define TAX_RATE (0.07)

#define CHECKOUT_CARD_PAYMENT	(0)
#define CHECKOUT_CASH_PAYMENT	(1)
#define CHECKOUT_CANCEL			(2)

#define ITEMS_AVAILABLE	(4)

#define CASH_OPTIONS	(12)

#define TRUE	(1)
#define FALSE	(0)

#define NUM_MOVIES	(3)

menu_states menu_state;
ordered_items item_tracker;

#define MAX_ORDER_ITEMS (20)
// Time to go from transcaction finished to next transcaction
#define RESET_TIME	(100)

#define ONE_HUNDRED_MS	(1)

#define GO_BACK_CASE	(10)
#define COMPLETE_TRANSCACTION_CASE	(11)

// Static variables in order to help the statemachine and keep track of things throughout the program
static double total_price;
static double net_price;
static double tax;
static double cash_given;
static uint8_t stop_putting_cash;
static given_cash_tracker given_tracker;
static uint8_t ticket_tracker[NUM_MOVIES][MAX_TIMES];
static uint8_t time_select;

// keeps track of the arrow in the screen which points to item to help user identify
// what would be selected if they pressed entered
uint8_t select;

// Available movies table with initial conditions for the times available
static movie_t movies[] =
{
{ .movie = "The Accountant", .tickets_available =
{ 5, 1, 8 }, .times =
{ "12:30 PM", "3:00 PM", "5:30 PM", NULL }, .auditorium =
{ 1, 4, 8 }, .times_available = 3 },
{ .movie = "The Minecraft Movie", .tickets_available =
{ 3, 2, 8 }, .times =
{ "11:00 AM", "2:15 PM", "4:45 PM", NULL }, .auditorium =
{ 12, 1, 6 }, .times_available = 3 },
{ .movie = "Thunderbolts", .tickets_available =
{ 4, 5 }, .times =
{ "1:00 PM", "3:30 PM", NULL }, .auditorium =
{ 9, 3, 12 }, .times_available = 2

} };

void clear()
{
	printf("\x1b[H\x1b[2J");
}

void process_char()
{
	// Used help from stack overflow to figure out how to process keyboard arrows
	uint8_t c = getchar(); // read first char
	if (c == '\x1B') // Escape character
	{
		// Peek ahead
		if (getchar() == '[')
		{
			// Third character tells you which  was selected
			c = getchar();
			switch (c)
			{
			case 'A': // Up Arrow
				if (menu_state == MAIN_MENU || menu_state == CASH_MENU
						|| menu_state == MOVIE_MENU)
				{
					if (select > 0)
					{
						select--;
						time_select = 0;
					}
				}
				else if (menu_state == CHECKOUT)
				{
					// User was at the Cancel, but then pressed up will go to the pay by card
					if (select == CHECKOUT_CANCEL)
					{
						select = 0;
					}
				}
				break;
			case 'B': // Down Arrow
				if (menu_state == MAIN_MENU)
				{
					if (select < ITEMS_AVAILABLE)
					{
						select++;
					}
				}
				else if (menu_state == CHECKOUT)
				{
					if (select == CHECKOUT_CARD_PAYMENT
							|| select == CHECKOUT_CASH_PAYMENT)
					{
						select = CHECKOUT_CANCEL;
					}
				}
				else if (menu_state == CASH_MENU)
				{
					if (select < (CASH_OPTIONS - 1))
					{
						select++;
					}
				}
				else if (menu_state == MOVIE_MENU)
				{
					if (select < NUM_MOVIES + 1)
					{
						select++;
						time_select = 0;
					}
				}
				break;
			case 'C': // Right arrow
				// Go right if user is in the checkout screen and has arrows selected at card payment
				if (menu_state == CHECKOUT && select == CHECKOUT_CARD_PAYMENT)
				{
					select++;
				}
				else if (menu_state == MOVIE_MENU)
				{
					if (time_select < (movies[select].times_available - 1))
					{
						time_select++;
					}
				}
				break;
			case 'D': // Left Arrow
				if (menu_state == CHECKOUT && select == CHECKOUT_CASH_PAYMENT)
				{
					select--;
				}
				else if (menu_state == MOVIE_MENU)
				{
					if (time_select > 0)
					{
						time_select--;
					}
				}
				break;
			default:
				break;
			}
			print_menu();
		}
	}
	else
	{
		switch (c)
		{
		// User pressed the enter key to add an item or select something from the current select
		case '\r':
			if (menu_state == MAIN_MENU)
			{
				add_item();
			}
			else if (menu_state == CHECKOUT)
			{
				if (select == 0)
				{
					make_card_payment();
				}
				else if (select == 1)
				{
					menu_state = CASH_MENU;
					select = 0;
				}
				// Pressed Canceled
				else
				{
					menu_state = MAIN_MENU;
					select = 0;
				}
			}
			else if (menu_state == CASH_MENU)
			{
				add_cash_given();
			}
			else if (menu_state == MOVIE_MENU)
			{
				add_ticket();
			}
			break;
		case DELETE_CHAR:
			// Delete Char removes anything the custumer might've added, except in the scanner which will exit out
			//
			if (menu_state == MAIN_MENU)
			{
				remove_item();
			}
			else if (menu_state == CASH_MENU)
			{
				remove_cash_given();
			}
			else if (menu_state == MOVIE_MENU && select < NUM_MOVIES + 2)
			{
				remove_ticket();
			}
			break;
		default:
			break;
		}
		print_menu();
	}
}

void movie_menu()
{
	for (int i = 0; i < NUM_MOVIES; i++)
	{
		if (i == select)
			// Use the arrow to help user know which item you are selecting
			printf(">> %s\r\n", movies[i].movie);
		else
			printf("   %s\r\n", movies[i].movie);

		for (int j = 0; j < MAX_TIMES && movies[i].times[j] != NULL; j++)
		{
			if (i == select && j == time_select)
			{
				// Use [] to help user know which time is being selected
				printf("   [%s] ", movies[i].times[j]);
			}
			else
			{
				printf("    %s  ", movies[i].times[j]);
			}
			// Print amount of tickets available at the time for the screening
			if (movies[i].tickets_available[j] > 0)
			{
				printf("Available Tickets: %d\r\n",
						movies[i].tickets_available[j]);
			}
			else
			{
				printf("SOLD OUT\r\n");
			}
		}
	}
	if (select == 3)
	{
		printf(">> GO back\r\n");
	}
	else
	{
		printf("   GO back\r\n");
	}
	if (select == 4)
	{
		printf(">> Checkout\r\n");
	}
	else
	{
		printf("   Checkout\r\n");
	}

}

void add_ticket()
{
	// if thier are seats available for the movie trying to select or you want to go back
	// to the old menu or to checkout then enter the switch statement
	if (movies[select].tickets_available[time_select] > 0 || select == 3
			|| select == 4)
	{
		switch (select)
		{
		case 0:
			if (movies[select].tickets_available[time_select] != 0)
			{
				movies[select].tickets_available[time_select]--;
				net_price += 6;
				item_tracker.movie_tickets++;
				ticket_tracker[select][time_select]++;
			}
			break;
		case 1:
			if (movies[select].tickets_available[time_select] != 0)
			{
				movies[select].tickets_available[time_select]--;
				net_price += 6;
				item_tracker.movie_tickets++;
				ticket_tracker[select][time_select]++;
			}
			break;
		case 2:
			if (movies[select].tickets_available[time_select] != 0)
			{
				movies[select].tickets_available[time_select]--;
				net_price += 6;
				item_tracker.movie_tickets++;
				ticket_tracker[select][time_select]++;
			}
			break;
		case 3:
			menu_state = MAIN_MENU;
			select = 0;
			time_select = 0;
			break;
		case 4:
			menu_state = CHECKOUT;
			select = 0;
			time_select = 0;
			tax = net_price * TAX_RATE;
			total_price = net_price + tax;
			break;
		default:
			break;
		}
	}
}

void remove_ticket()
{
	// If user wants to remove a previously selected movie
	if (ticket_tracker[select][time_select] > 0)
	{
		switch (select)
		{
		case 0:
			movies[select].tickets_available[time_select]++;
			ticket_tracker[select][time_select]--;
			if (movies[select].tickets_available[time_select] == 0)
			{
				movies[select].times_available++;
			}
			break;
		case 1:
			movies[select].tickets_available[time_select]++;
			ticket_tracker[select][time_select]--;
			if (movies[select].tickets_available[time_select] == 0)
			{
				movies[select].times_available++;
			}
			break;
		case 2:
			movies[select].tickets_available[time_select]++;
			ticket_tracker[select][time_select]--;
			if (movies[select].tickets_available[time_select] == 0)
			{
				movies[select].times_available++;
			}
			break;
		default:
			break;
		}
	}
}

void checkout_menu()
{
	printf("====== CHECKOUT ======\r\n");
	printf("\r\nYour Order:\r\n");
	printf("----------------------------\r\n");
	if (item_tracker.movie_tickets > 0)
	{
		for (int i = 0; i < NUM_MOVIES; i++)
		{
			for (int j = 0; j < MAX_TIMES && movies[i].times[j] != NULL; j++)
			{
				if (ticket_tracker[i][j] > 0)
				{
					printf("%s x%d\r\n%s\r\n\r\n\r\n", movies[i].movie,
							ticket_tracker[i][j], movies[i].times[j]);
				}
			}
		}
	}

	if (item_tracker.popcorn > 0)
	{
		printf("Popcorn x%d ---- $%.2f\r\n", item_tracker.popcorn,
				item_tracker.popcorn * POPCORN_PRICE);
	}
	if (item_tracker.soda > 0)
	{
		printf("Soda    x%d ---- $%.2f\r\n", item_tracker.soda,
				item_tracker.soda * SODA_PRICE);
	}
	if (item_tracker.nachos > 0)
	{
		printf("Nachos  x%d ---- $%.2f\r\n", item_tracker.nachos,
				item_tracker.nachos * NACHOS_PRICE);
	}
	// Print the price summarry
	printf("----------------------------\r\n");
	printf("Subtotal: $%.2f\r\n", net_price);
	printf("Tax:      $%.2f\r\n", tax);
	printf("TOTAL:    $%.2f\r\n", total_price);

	printf("\r\nSelect Payment Method:\r\n");
	printf("----------------------------\r\n");

	// The user can select what payment method they want, use arrows to indicate where you are
	switch (select)
	{
	case CHECKOUT_CARD_PAYMENT:
		printf(">> Scan Credit Card                       Insert Cash\r\n");
		printf("                         CANCEL\r\n");
		break;
	case CHECKOUT_CASH_PAYMENT:
		printf("   Scan Credit Card                    >> Insert Cash\r\n\r\n");
		printf("                         CANCEL\r\n");
		break;
	case CHECKOUT_CANCEL:
		printf("   Scan Credit Card                       Insert Cash\r\n");
		printf("                     >> CANCEL\r\n");
		break;
	}
}

void main_menu()
{
	printf("====== MAIN MENU ======\r\n");
	// Use an arrow to indicate where the user is currently selecting in the menu
	printf("Select an item to add:\r\n\r\n");
	printf("%sMovie      $6.00\r\n", select == 0 ? ">> " : "   ");
	printf("%sPopcorn    $3.00\r\n", select == 1 ? ">> " : "   ");
	printf("%sSoda       $1.00\r\n", select == 2 ? ">> " : "   ");
	printf("%sNachos     $4.00\r\n", select == 3 ? ">> " : "   ");
	printf("%sCHECKOUT\r\n", select == 4 ? ">> " : "   ");

	// Print the current order custumer has added
	printf("\r\nCurrent Order:\r\n");
	printf("----------------------------\r\n");
	// Print out which movie the user has added to watch
	if (item_tracker.movie_tickets > 0)
	{
		for (int i = 0; i < NUM_MOVIES; i++)
		{
			for (int j = 0; j < MAX_TIMES && movies[i].times[j] != NULL; j++)
			{
				if (ticket_tracker[i][j] > 0)
				{
					printf("%s x%d\r\n%s\r\n\r\n", movies[i].movie,
							ticket_tracker[i][j], movies[i].times[j]);
				}
			}
		}
	}
	// Print how much each item is adding to the cost as well as the quanity of items user has ordered
	if (item_tracker.popcorn > 0)
	{
		printf("Popcorn x%d ---- $%.2f\r\n", item_tracker.popcorn,
				item_tracker.popcorn * POPCORN_PRICE);
	}
	if (item_tracker.soda > 0)
	{
		printf("Soda    x%d ---- $%.2f\r\n", item_tracker.soda,
				item_tracker.soda * SODA_PRICE);
	}
	if (item_tracker.nachos > 0)
	{
		printf("Nachos  x%d ---- $%.2f\r\n", item_tracker.nachos,
				item_tracker.nachos * NACHOS_PRICE);
	}
	// Print the price summary
	tax = net_price * TAX_RATE;
	total_price = tax + net_price;
	printf("----------------------------\r\n");
	printf("Subtotal: $%.2f\r\n", net_price);
	printf("Tax:      $%.2f\r\n", tax);
	printf("TOTAL:    $%.2f\r\n", total_price);
}

void thank_you_screen()
{
	printf("====== THANK YOU ======\r\n");
	// Print employee who took the order
	printf("Served by: Ivan Calderon\r\n\r\n");
	printf("Order Summary:\r\n");
	printf("----------------------------\r\n");

	// Print out movies custumer bought as well as their indicated times, and auditorium
	if (item_tracker.movie_tickets > 0)
	{
		for (int i = 0; i < NUM_MOVIES; i++)
		{
			for (int j = 0; j < MAX_TIMES && movies[i].times[j] != NULL; j++)
			{
				if (ticket_tracker[i][j] > 0)
				{
					// In movie ticket format
					printf("----------------------------\r\n");
					printf("%s x%d\r\n%s\r\nAUD#%d\r\n", movies[i].movie,
							ticket_tracker[i][j], movies[i].times[j],
							movies[i].auditorium[j]);
					printf("----------------------------\r\n");
				}
			}
		}
	}
	// Print out the total cost of each item and their quantity as well as how much they add to the order
	if (item_tracker.popcorn > 0)
	{
		printf("Popcorn x%d ---- $%.2f\r\n", item_tracker.popcorn,
				item_tracker.popcorn * POPCORN_PRICE);
	}
	if (item_tracker.soda > 0)
	{
		printf("Soda    x%d ---- $%.2f\r\n", item_tracker.soda,
				item_tracker.soda * SODA_PRICE);
	}
	if (item_tracker.nachos > 0)
	{
		printf("Nachos  x%d ---- $%.2f\r\n", item_tracker.nachos,
				item_tracker.nachos * NACHOS_PRICE);
	}
	printf("----------------------------\r\n");

	printf("Subtotal: $%.2f\r\n", net_price);
	printf("Tax:      $%.2f\r\n", tax);
	printf("TOTAL:    $%.2f\r\n", total_price);
	if (cash_given > 0)
	{
		printf("Cash Given: %.2f\r\n", cash_given);
		if (cash_given > total_price)
		{
			// Money to give back to the custumer
			printf("  Returned: %.2f\r\n", (-1) * (total_price - cash_given));
		}
	}
	// if the custumer ordered any tickets add instructions on where to show another employee
	if (item_tracker.movie_tickets > 0)
	{
		printf("\r\nPlease Show This Receipt at the Hall Entrance\r\n");
	}
	printf("\r\n********* Enjoy The Movie! *********\r\n");

	// reset the tickets bought for the next cusutmer
	for (int i = 0; i < NUM_MOVIES; i++)
	{
		for (int j = 0; j < MAX_TIMES; j++)
		{
			ticket_tracker[i][j] = 0;
		}
	}
	menu_state = MAIN_MENU;

	// Reset order
	total_price = 0;
	net_price = 0;
	tax = 0;
	item_tracker.popcorn = 0;
	item_tracker.soda = 0;
	item_tracker.nachos = 0;
	item_tracker.movie_tickets = 0;
	select = 0;
	// Delay for 10 seconds then reset
	delay(RESET_TIME);
	print_menu();
}

void print_menu()
{
	clear();
	switch (menu_state)
	{
	case MAIN_MENU:
		main_menu();
		break;
	case MOVIE_MENU:
		movie_menu();
		break;
	case CHECKOUT:
		checkout_menu();
		break;
	case CASH_MENU:
		cash_payment_menu();
		break;
	case THANK_YOU_SCREEN:
		thank_you_screen();
		break;
	}
}

void add_item()
{
	switch (select)
	{
	case 0:
		menu_state = MOVIE_MENU;
		break;
	case 1:
		item_tracker.popcorn++;
		net_price += POPCORN_PRICE;
		break;
	case 2:
		item_tracker.soda++;
		net_price += SODA_PRICE;
		break;
	case 3:
		item_tracker.nachos++;
		net_price += NACHOS_PRICE;
		break;
	case 4:
		menu_state = CHECKOUT;
		select = 0;
		break;
	}
}

void remove_item()
{
	if (net_price > 0)
	{
		switch (select)
		{
		case 0:
			if (item_tracker.popcorn > 0)
			{
				item_tracker.popcorn--;
				item_tracker.total_items--;
				net_price -= POPCORN_PRICE;
			}
			break;
		case 1:
			if (item_tracker.soda > 0)
			{
				item_tracker.soda--;
				item_tracker.total_items--;
				net_price -= SODA_PRICE;
			}
			break;
		case 2:
			if (item_tracker.nachos > 0)
			{
				item_tracker.nachos--;
				item_tracker.total_items--;
				net_price -= NACHOS_PRICE;
			}
			break;
		}
	}
}

void make_card_payment()
{
	printf("Please scan your card\r\n");
	uint8_t ch;
	while (1) // Loop until either card detected or cancel
	{
		if (mfrc522_is_card_present())
		{
			// Card detected go to thank you screen
			menu_state = THANK_YOU_SCREEN;
			// reset select
			select = 0;
			return;
		}
		// Check if user sent a character
		if (usart_check_ready(&ch))
		{
			// Check if that character is the delete character to exit out of polling loop
			if (ch == DELETE_CHAR)
			{
				menu_state = CHECKOUT;
				select = 0;
				return;
			}
		}
		// delay for 100ms for next poll
		delay(ONE_HUNDRED_MS);
	}
}

const char *cash_options[] =
{ "0.01", "0.05", "0.10", "0.25", "1.00", "5.00", "10.00", "20.00", "50.00",
		"100.00", "Cancel", "Complete Transaction" };

void cash_payment_menu()
{
	for (int i = 0; i < CASH_OPTIONS; i++)
	{
		if (select == i)
			printf(">> %s\r\n", cash_options[i]);
		else
			printf("   %s\r\n", cash_options[i]);
	}

	printf("\r\nCash Given: %.2f\r\n", cash_given);
	printf("TOTAL: %.2f\r\n", total_price);

	double amount_due = total_price - cash_given;
	if (amount_due >= 0)
	{
		printf("Cash Due: %.2f\r\n", amount_due);
	}
	else
	{
		printf("Give back: %.2f\r\n", -amount_due);
		stop_putting_cash = TRUE;
	}
}

void add_cash_given()
{
	// only add cash if we haven't gone past the total amount
	if (!stop_putting_cash || select == GO_BACK_CASE
			|| select == COMPLETE_TRANSCACTION_CASE)
	{
		switch (select)
		{
		case 0:
			cash_given += 0.01;
			given_tracker.cents++;
			break;
		case 1:
			cash_given += 0.05;
			given_tracker.nickels++;
			break;
		case 2:
			cash_given += 0.1;
			given_tracker.dimes++;
			break;
		case 3:
			cash_given += 0.25;
			given_tracker.quaters++;
			break;
		case 4:
			cash_given += 1;
			given_tracker.dollars++;
			break;
		case 5:
			cash_given += 5;
			given_tracker.five_dollars++;
			break;
		case 6:
			cash_given += 10;
			given_tracker.ten_dollars++;
			break;
		case 7:
			cash_given += 20;
			given_tracker.twenty_dollars++;
			break;
		case 8:
			cash_given += 50;
			given_tracker.fifty_dollars++;
			break;
		case 9:
			cash_given += 100;
			given_tracker.hundred_dollars++;
			break;
		case 10:
			// Change State
			menu_state = CHECKOUT;
			// Reset everything that might've changed in this state
			select = 0;
			cash_given = 0;
			given_tracker.cents = 0;
			given_tracker.nickels = 0;
			given_tracker.dimes = 0;
			given_tracker.dollars = 0;
			given_tracker.five_dollars = 0;
			given_tracker.ten_dollars = 0;
			given_tracker.twenty_dollars = 0;
			given_tracker.fifty_dollars = 0;
			given_tracker.hundred_dollars = 0;
			cash_given = 0;
			break;
		case 11:
			if (cash_given >= total_price)
			{
				menu_state = THANK_YOU_SCREEN;
				return;
			}
			else
			{
				// Can't check out if user didn't put enough money in
				printf("Not enough funds\r\n");
			}
			break;
		}

	}
}

void remove_cash_given()
{
	// See if any cash was given
	if (cash_given > 0)
	{
		switch (select)
		{
		case 0:
			// check that the type of cash trying to remove was given
			if (given_tracker.cents > 0)
			{
				cash_given -= 0.01;
				given_tracker.cents--;
			}
			break;
		case 1:
			if (given_tracker.nickels > 0)
			{
				cash_given -= 0.05;
				given_tracker.nickels--;
			}
			break;
		case 2:
			if (given_tracker.dimes > 0)
			{
				cash_given -= 0.1;
				given_tracker.dimes--;
			}
			break;
		case 3:
			if (given_tracker.quaters > 0)
			{
				cash_given -= 0.25;
				given_tracker.quaters--;
			}
			break;
		case 4:
			if (given_tracker.dollars > 0)
			{
				cash_given -= 1;
				given_tracker.dollars--;
			}
			break;
		case 5:
			if (given_tracker.five_dollars > 0)
			{
				cash_given -= 5;
				given_tracker.five_dollars--;
			}
			break;
		case 6:
			if (given_tracker.ten_dollars > 0)
			{
				cash_given -= 10;
				given_tracker.ten_dollars--;
			}
			break;
		case 7:
			if (given_tracker.twenty_dollars > 0)
			{
				cash_given -= 20;
				given_tracker.twenty_dollars--;
			}
			break;
		case 8:
			if (given_tracker.fifty_dollars > 0)
			{
				cash_given -= 50;
				given_tracker.fifty_dollars--;
			}
			break;
		case 9:
			if (given_tracker.hundred_dollars > 0)
			{
				cash_given -= 100;
				given_tracker.hundred_dollars--;
			}
			break;
		default:
			break;
		}
	}
}

