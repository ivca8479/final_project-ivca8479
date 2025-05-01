[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/JZWYWevW)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=18822983&assignment_repo_type=AssignmentRepo)
# ecen_5813_Final_Project

# Description
In this Project I simulated a Cash register for a movie theater. 
I implemented this as a state machine which uses usart for command processing,
The MFRC522 senosr,
SPI to send and recieve signals to the MFRC522 sensor
Systick for precised delays

You can add movie items suchs as tickets at given times, popcorn, soda, and nachos and then when
ready you go to the checkout
In the checkout you select your method of payment with either a card payment or cash payment
and once processed  it prints out a reciept on the screen which includes movie tickets and instructions
if movie tickets where bought as well as any additional food items custumer may have purchased. 
After a few seconds it restarts, if any movies where picked by the last custumer, the tickets available
for the section is updated

# How to use
The arrows in putty will help guide the user where they stand on the menu, you can change this by using
the up and down arrows on the screen. Some menues such as the checkout menu and the movie menu will
allow you to use the left and right arrows.

In the movie select menu you press up and down to select which movie you would like to see,
pressing the left and right arrows will change what time you would see the movie indicated with brackets []

In the checkout menu, moving the left and right arrow will allow you to change which payment method
will be selected

Pressing enter will either select the item you are currently highlighting 
or change the state of the menu to the next phase when redy

Pressing the delete key will remove items only if they have been added 

When selecting a card payment, the mfrc522 will poll until it detects the card which you put in close
proximity with it.

For proper interaction use the putty serial terminal as I used a specific clear command
which I am not sure will work on other serial terminals, and set it up in the following manner

# Testing
In order to test this project, I dividided multiple peripherals up before I used them in the main cash register.c file. My way to test the MFRC522 was I created a function called detected_card(). I would run this in my Main.c file just initializing the SPI and mfrc522 and doing a polling test to see if when I had the card close to the sensor it would detect it and output "Card detected" onto the serial terminal. After I felt confident about the precision I implemented it in my cash_register.c file which worked great. 

I used my assignment #6 USART configuration, but changed it so the USART is at no parity and 9600 as that is the default settings in putty and this made it faster for me to test my code. I know that this worked well based on assignment #6 feedback given to us, and used the queue I made in assignment #2 which had extensive testing done to it.

To test my keyboard button presses, the serial terminal helped with that as I could easily detect with the arrow on the screen went based on my keyboard presses. Using the insert key to add items, I outputed on the main menu how many items their were, how many of each specific item there was, and how much the cost of each item alligned to. Removing items also comfirmed my item tracker was working as those items would indeed be deleted and the cost would reduce. 

To test my state machine, I just focused on one state at a time before I implemented all of them together, doing that by not being able to change states when printing a menu. Once I knew each one worked individually based on the tests from earlier I could then add them together and had a reliable state machine.  

# USART Setup:

Set baud Rate/Speed to 9600

Set data to 8 bits

Set Parity to Non

Flow Controle XON/XOFF

# SPI MFRC522 Setup:
Using the STM32F4091RC Microcontroller, put the 3.3 volts to the power rail on a breadboard
Put the microcontroller ground to the ground rail on the same breadboard

Using Wires connect the MFRC522 ground to the breadboard ground, and the 3.3 pin on the MFRC522 
to the power rail you had on the breadboard

CS pin goes to PA15 on the microcontroller
SCK pin goes to D3 female header pin on microcontroller
MISO Pin goes to D4 female header pin
MOSI Pin goes to D5 female header pin
Reset pin goes to the RESET female header pin which is one above the 3.3 V header pin




# References 
Dean Chapter 8

https://github.com/miguelbalboa/rfid 

Assignment #6 

# Important Data Sheets:
https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf






