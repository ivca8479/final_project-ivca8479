/*
 * processor.c
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 */

#include "processor.h"

#define BUFFER_SIZE	(1000)
#define MAX_ARGC		(10)
// To account for the '<<' characters, always need to consider them
#define MIN_CHARS		(2)
// led arguments can only be on or off
#define MAX_LED_PARAMS (2)

#define DELETE_CHAR (127)


typedef void (*command_handler_t)(int i, char *argv[]);

/*
 *Echos the input parameters given after the echo command, prints them out in all capital letters
 *Will print out an error message if no
 */
void echo(int argc, char *argv[])
{
	if(argc == 1)
	{
		printf("Error, no arguments given to Echo\r\n");
		return;
	}
	for (int i = 1; i < argc; i++)
	{
			printf("%s ", argv[i]);
	}
	printf("\n\r");
}


/*
 * @brief: Command used to turn on and off the user on board led. The user must only write "led on" in any
 * variation of capitial letters. Anything else will print as invalid argument
 * @parameters:
 * argc - total count of inputs given to the led command
 * argv - string array that holds the parameters needed for the led command
 */
void led(int argc, char *argv[]) {
    if (strcmp(argv[1], "ON") == 0 && argc < MAX_LED_PARAMS)
    {
        led_on();
    }
    else if (strcmp(argv[1], "OFF") == 0 && argc < MAX_LED_PARAMS)
    {
        led_off();
    }
    else
    {
    	printf("Invalid argument for the LED on/off function, can only take in input on or off anything else is invalid\r\n");
    }
}

/*
 * @brief: Prints author of the program to the USART terminal
 */
void author(int argc, char *argv[]) {
	if(argc > 0)
	{
		printf("Invalid argument for author, author takes no argument\r\n");
		return;
	}
	printf("Ivan Calderon\n\r");
}



/*
 * Works on putty terminal, clear message used in putty to clear the screen to refresh and make things less messy
 */
void clear(int argc, char *argv[])
{
	printf("\x1b[H\x1b[2J");
}

/*
 * Struct used to build the command table and map names of commands to their respective command handler
 */

typedef struct {
	const char *name;
	command_handler_t handler;
}command_table_t;






// comands used in this program as well as their
static const command_table_t commands[] = {
		{"ECHO", echo},
		{"LED", led},
		{"AUTHOR",author},
		{"CLEAR", clear}
};


static const int num_commands = sizeof(commands) / sizeof(command_table_t);


/*
 * @brief Command handler function used to map commands to their respective handler function
 */
void command_handler(int argc, char *argv[])
{
	for (int i=0; i < num_commands; i++) {
		// see if the argument matches with one of the command names
		if (strcmp(argv[0], commands[i].name) == 0) {
			// give the updated count to the command handler
			int left = argc - i;
			// call the command
			commands[i].handler(left, argv);
			// exit
			return;
		}
	}
	// if no command function was called print out what was written
	printf("Unknown command (%s)\r\n", argv[0]);
}


/*
 * 	Accumulates the line from the getchar function and builds a line made up of only alphabetical letters
 */
void process_readline()
{
	char line[BUFFER_SIZE];
	int index = MIN_CHARS;
	int i = 0;
	int c;
	int spaces = 1;
	printf("<< ");
	// build up the line until user presses the return/enter
	while(((c = getchar()) != '\r') && (i < (BUFFER_SIZE - 1)))
	{
		// user deletes a character, doesn't remove the << indicator
		if(c == '\b' || c == DELETE_CHAR)
		{
			if(i > 0)
			{
				// move back 1 index
				i--;
			}
			// make sure it doesnt cross '<<' region
			if(index > MIN_CHARS)
			{
				//'delete the character'
				index--;
				printf("\b \b");
			}
		}
		else
		{
			// echo the character
			printf("%c", c);
			// skip spaces when building the line to send to the command processor
			if(c == ' ')
			{
				if(spaces == 0)
				{
					// use space as a delimiter
					line[i++] = ' ';
					// set spaces = 1 to avoid multiple spaces in a row
					spaces = 1;
				}
				// keep track of the printed characters still
				index++;
			}
			// for other characters
			if(c != ' ')
			{
				// convert all letters to upper case
				if(isalpha(c))
				{
					c = toupper(c);
				}
				// add to the line buffer
				line[i++] = c;
				index++;
				spaces = 0;
			}
		}
	}
	// use a space as a delimiter
	line[i++] = ' ';
	line[i] = '\0';
	printf("\n\r");
	// process the formated built up line
	process_command(line);
}

/*
 *  Lexical Analysis
 */
void process_command(char *input)
{
	char *p = input;
	char *end;
	// find end of string
	for (end = input; *end != '\0'; end++);
	// Tokenize input in place
	char *argv[MAX_ARGC];
	int argc = 0;
	char word[BUFFER_SIZE];
	int index = 0;
	memset(argv, 0, sizeof(argv));
	memset(word, 0, sizeof(word));
	for (p = input; p < end; p++)
	{
	// TODO: Your code here!

		//reached the end of the token with a delimiter ' '
		if(*p == ' ')
		{
			if(index > 0)
			{
				// null terminate
				word[index] = '\0';
				// add to array of tokens
				argv[argc++] = strdup(word);;
				index = 0;
				// reset the current word
				memset(word, 0, sizeof(word));
				// move on to the next character
				p++;
			}
		}
		// caught the ';' delimiter used to seperate commands
		if(*p == ';')
		{
			p++;
			// doesn't seperate command, instead is an escape character to actually print the character ';'
			if(*p != ';')
			{
				if(index > 0 && word != '\0')
				{
					// null terminate
					word[index] = '\0';
					//process the command and its arguments
					argv[argc++] = strdup(word);
					argv[argc] = NULL;
					command_handler(argc, argv);
					// reset argv
					memset(argv, 0, sizeof(argv));
					// reset the current word
					memset(word, 0, sizeof(word));
					argc = 0;
					index = 0;
					// ignore any spaces and ';' characters
					while(*p == ' ' || *p == ';')
					{
						p++;
					}
				}
			}
		}
		word[index++] = *p;
	}
	// no command
	if (argc == 0){
		return;
	}
	argv[argc] = NULL;

	// TODO: Dispatch argc/argv to handler
	command_handler(argc, argv);
}
