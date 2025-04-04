/*
 * processor.h
 *
 *  Created on: Mar 26, 2025
 *      Author: ivanc
 *  @brief: Gives abstraction to the main part of the application, the processor command line.
 *  Uses all other files in the project in order to properly parse, send, and communicate data throught a message
 *  buffer, and output and receive through USART
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "usart.h"
#include "led.h"
#include "ctype.h"
#include <string.h>


/*
 * @brief: Function used to clean up and build up text from the USART terminal used to build up the line
 * for command processing. This formats the line so there are all capital letters.
 * Calls process_command function to process the messages received in this formated form.
 *
 */
void process_readline();
/*
 * @brief: Function used to analyze the line formated from process_readline. Will store located seperated words
 * into the argv parameter, and the word oount increases for each word located. Will have ';;' as an escape
 * character
 */
void process_command(char *input);

#endif /* PROCESSOR_H_ */
