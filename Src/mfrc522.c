/*
 * RC522.c
 *
 *  Created on: Apr 23, 2025
 *      Author: ivanc
 *
 *  @brief file used to be able to access and read registers from the
 *  mfrc522 sensor.
 *  Used this github from arduino as reference:
 *  https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp
 *
 */

#include "mfrc522.h"

/*
 *  Macro Functions used to control the SS pin in order to transfer and send Data
 */
#define SS_PIN   (15)
#define SS_PORT  (GPIOA)
#define NSS_LOW()   (SS_PORT->BSRR |= (1 << (16 + SS_PIN)))
#define NSS_HIGH()  (SS_PORT->BSRR |= (1 << SS_PIN))
// Status codes
#define MI_OK         (0)
#define MI_NOTAGERR   (1)
#define MI_ERR        (2)

// MFRC522 Commands
#define PCD_IDLE         (0x00)
#define PCD_TRANSCEIVE   (0x0C)
#define PCD_AUTHENT      (0x0E)
#define PCD_RESETPHASE   (0x0F)

// MIFARE Commands
#define PICC_REQIDL      (0x26)

// MFRC522 Registers Used
#define CommandReg       (0x01)
#define CommIEnReg       (0x02)
#define CommIrqReg       (0x04)
#define ErrorReg         (0x06)
#define FIFODataReg      (0x09)
#define FIFOLevelReg     (0x0A)
#define ControlReg       (0x0C)
#define BitFramingReg    (0x0D)
#define ModeReg          (0x11)
#define TxControlReg     (0x14)
#define TxASKReg         (0x15)
#define TModeReg         (0x2A)
#define TPrescalerReg    (0x2B)
#define TReloadRegH      (0x2C)
#define TReloadRegL      (0x2D)
#define VersionReg       (0x37)

// Buffer and bit masks
#define MAX_LEN               (16)
#define MFRC522_WRITE_MASK    (0x7E)
#define MFRC522_READ_MASK     (0x80)

#define MFRC522_REQUEST_BITS     (0x07)
#define MFRC522_ATQA_EXPECTED    (0x10)

#define MFRC522_ANTENNA_MASK       (0x03)

// MFRC522 initialization, used the Arduino Libraries for MFRC to help with the values
// These seem pretty standard and worked well for my project
#define MFRC522_INIT_TMODE_VALUE        (0x8D)
#define MFRC522_INIT_TPRESCALER_VALUE   (0x3E)
#define MFRC522_INIT_TRELOADL_VALUE     (30)
#define MFRC522_INIT_TRELOADH_VALUE     (0)
#define MFRC522_INIT_TXASK_VALUE        (0x40)
#define MFRC522_INIT_MODE_VALUE         (0x3D)

// IRQ masks
#define MFRC522_IRQEN_AUTH           (0x12)
#define MFRC522_IRQEN_TRANSCEIVE     (0x77)
#define MFRC522_IRQ_WAIT_AUTH        (0x10)
#define MFRC522_IRQ_WAIT_TRANSCEIVE  (0x30)

#define MFRC522_TIMEOUT	(2000)

#define BUFFER_LEN	(2)
#define DATA_BITS	(8)

#define MFRC522_ERROR	(0x1B)

void mfrc522_write(uint8_t reg, uint8_t value)
{
	// From the Datasheet the NSS must stay low while it sends or recieves
	NSS_LOW();
	SPI_Send_Receive_Byte((reg << 1) & MFRC522_WRITE_MASK);
	SPI_Send_Receive_Byte(value);
	NSS_HIGH();
}

uint8_t mfrc522_read(uint8_t reg)
{
	NSS_LOW();
	SPI_Send_Receive_Byte(
			((reg << 1) & MFRC522_WRITE_MASK) | MFRC522_READ_MASK);
	// 0 is dummy data used to receive a byte
	uint8_t data = SPI_Send_Receive_Byte(0);
	NSS_HIGH();
	return data;
}

void mfrc522_set_bitmask(uint8_t reg, uint8_t mask)
{
	// read current register value
	uint8_t tmp = mfrc522_read(reg);
	// write to the register this value
	mfrc522_write(reg, tmp | mask);
}

void mfrc522_clear_bitmask(uint8_t reg, uint8_t mask)
{
	// read current register value
	uint8_t tmp = mfrc522_read(reg);
	// clear it
	mfrc522_write(reg, tmp & (~mask));
}

void mfrc522_reset()
{
	// Send command to reset the mfrc522
	mfrc522_write(CommandReg, PCD_RESETPHASE);
}

void mfrc522_antenna_on()
{
	// Read from the TxControl Registste
	uint8_t temp = mfrc522_read(TxControlReg);
	if (!(temp & MFRC522_ANTENNA_MASK))
	{
		mfrc522_set_bitmask(TxControlReg, MFRC522_ANTENNA_MASK);
	}
}

void mfrc522_init()
{
	// Make sure the MFRC522 is ready for initialization by reseting it
	mfrc522_reset();
	// Configure modes and registers
	mfrc522_write(TModeReg, MFRC522_INIT_TMODE_VALUE);
	mfrc522_write(TPrescalerReg, MFRC522_INIT_TPRESCALER_VALUE);
	mfrc522_write(TReloadRegL, MFRC522_INIT_TRELOADL_VALUE);
	mfrc522_write(TReloadRegH, MFRC522_INIT_TRELOADH_VALUE);
	mfrc522_write(TxASKReg, MFRC522_INIT_TXASK_VALUE);
	mfrc522_write(ModeReg, MFRC522_INIT_MODE_VALUE);
	// Turn the antenna on so it can be ready for card detections
	mfrc522_antenna_on();
}

uint8_t mfrc522_to_card(uint8_t command, uint8_t *sendData, uint8_t sendLen,
		uint8_t *backData, uint16_t *backBits)
{
	uint8_t irqEnable = 0, waitIRQ = 0;
	uint8_t status = MI_ERR;
	uint8_t irqFlags, errorFlags, receivedBytes, controlBits;
	uint16_t timeout = MFRC522_TIMEOUT;

	// Determine IRQ masks based on the command
	if (command == PCD_AUTHENT)
	{
		irqEnable = 0x12;
		waitIRQ = 0x10;
	}
	else if (command == PCD_TRANSCEIVE)
	{
		irqEnable = 0x77;
		waitIRQ = 0x30;
	}

	// Enable interrupt
	mfrc522_write(CommIEnReg, irqEnable | MFRC522_READ_MASK);
	// Clear interrupt flags
	mfrc522_clear_bitmask(CommIrqReg, MFRC522_READ_MASK);
	// Reset FIFO pointer
	mfrc522_set_bitmask(FIFOLevelReg, MFRC522_READ_MASK);

	// Stop any running command
	mfrc522_write(CommandReg, PCD_IDLE);

	// Write data to FIFO
	for (uint8_t i = 0; i < sendLen; i++)
	{
		mfrc522_write(FIFODataReg, sendData[i]);
	}

	// Start the desired command
	mfrc522_write(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	{
		mfrc522_set_bitmask(BitFramingReg, MFRC522_READ_MASK);
	}

	// Wait for command completion
	do
	{
		irqFlags = mfrc522_read(CommIrqReg);
		timeout--;
	} while (timeout && !(irqFlags & (waitIRQ | 1))); // Wait for interrupt or timeout

	// Stop transmission if started
	mfrc522_clear_bitmask(BitFramingReg, MFRC522_READ_MASK); // StartSend = 0

	// Check for errors
	errorFlags = mfrc522_read(ErrorReg);
	if (timeout && !(errorFlags & MFRC522_ERROR)) // No buffer overflow, CRC, or protocol error
	{
		status = MI_OK;

		// If timeout interrupt was set, treat as no tag present
		if (irqFlags & 1)
		{
			status = MI_NOTAGERR;
		}

		// Handle received data
		if (command == PCD_TRANSCEIVE)
		{
			receivedBytes = mfrc522_read(FIFOLevelReg);
			controlBits = mfrc522_read(ControlReg) & MFRC522_REQUEST_BITS;

			if (controlBits)
			{
				*backBits = (receivedBytes - 1) * DATA_BITS + controlBits;
			}
			else
			{
				*backBits = receivedBytes * DATA_BITS;
			}

			// Limit output size
			if (receivedBytes > MAX_LEN)
			{
				receivedBytes = MAX_LEN;
			}
			// Copy response to output buffer
			for (uint8_t i = 0; i < receivedBytes; i++)
			{
				backData[i] = mfrc522_read(FIFODataReg);
			}
		}
	}

	return status;
}

uint8_t mfrc522_request(uint8_t reqMode, uint8_t *tagType)
{
	uint8_t status;
	uint16_t backBits;

	mfrc522_write(BitFramingReg, MFRC522_REQUEST_BITS);

	tagType[0] = reqMode;

	status = mfrc522_to_card(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
	// Check things returned as expected
	if ((status != MI_OK) || (backBits != MFRC522_ATQA_EXPECTED))
	{
		status = MI_ERR;
	}

	return status;
}

uint8_t mfrc522_is_card_present()
{
	uint8_t buffer[BUFFER_LEN];
	uint8_t status = mfrc522_request(PICC_REQIDL, buffer);
	// Return if request was sucesful for card is ok
	return (status == MI_OK);
}
