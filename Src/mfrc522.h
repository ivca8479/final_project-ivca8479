/*
 * RC522.h
 *
 *  Created on: Apr 23, 2025
 *      Author: ivanc
 */

#ifndef RC522_H_
#define RC522_H_

#include "spi.h"
#include "usart.h"

/*
 * @brief:
 * This file is used to access the mfrc522 code sensor and be able to integrate
 * a card polling method with SPI
 */

/*
 *  @brief: Sends a command to the MFRC522 and handles response exchange.
 *
 * @parameters
 * command - The command to send (e.g., PCD_AUTHENT, PCD_TRANSCEIVE).
 * sendData - Pointer to data to be written to the FIFO.
 * sendLen - Length of the data to send.
 * backData - Pointer to buffer to store received data.
 * backBits - Pointer to store number of received bits.
 * @return MI_OK if successful, MI_ERR if unsuccesful
 */
uint8_t mfrc522_to_card(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backBits);

/*
 * @breif: writes to a register on the mfrc522 using spi communication
 *
 * @parameters
 * reg - register from the MFRC we wish to send
 * value - data we wish to write
 */

void mfrc522_write(uint8_t reg, uint8_t value);

/*
 * @brief: reads a value at a register in the MFRC522
 *
 * @parameters:
 * reg - register we wish to read from
 *
 * @return:
 * returns value read from the register
 */
uint8_t mfrc522_read(uint8_t reg);

/**
 * @brief Sends a REQA or WUPA request command to detect nearby RFID tags.
 *
 * @parameter
 *  reqMode Mode to request (PICC_REQIDL or PICC_REQALL).
 *  tagType Pointer to store the type of tag found.
 *
 * @return MI_OK if a tag was detected, MI_ERR otherwise.
 */
uint8_t mfrc522_request(uint8_t reqMode, uint8_t *tagType);

/**
 * @brief Checks if an RFID card is present.
 *
 * @return 1 if a card is detected, 0 otherwise.
 */
uint8_t mfrc522_is_card_present();
/**
 * @brief Turns on the MFRC522 antenna by enabling the Tx1 and Tx2 control bits.
 */
void mfrc522_antenna_on();
/**
 * @brief Sends the reset command to the MFRC522 to return it to default state.
 */
void mfrc522_reset();

/**
 * @brief Initializes the MFRC522 with standard configuration values.
 * Sets timer settings, modulation mode, and turns on the antenna.
 */
void mfrc522_init();

/**
 * @brief Sets specific bits in a given register.
 *
 * @param reg Register address.
 * @param mask Bitmask specifying bits to set.
 */
void mfrc522_set_bitmask(uint8_t reg, uint8_t mask);

/**
 * @brief
 * Clears specific bits in a given register.
 *
 * @parameters:
 *  reg - Register address.
 *  mask - Bitmask specifying bits to clear.
 */
void mfrc522_clear_bitmask(uint8_t reg, uint8_t mask);

#endif /* RC522_H_ */
