/*******************************************************************************

 M95M01 SPI EEPROM library
 -------------------------
 
 M95M01.cpp - M95M01 class implementation file
 
 Code written by Stefan Dzisiewski-Smith.
 
 Copyright (c) 2016, Stefan Dzisiewski-Smith
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*******************************************************************************/

#include "M95M01.h"
#include <Arduino.h>

const uint8_t CMD_WRSR  = 0x01;  // write status register
const uint8_t CMD_WRITE = 0x02;  // write to EEPROM
const uint8_t CMD_READ  = 0x03;  // read from EEPROM
const uint8_t CMD_WRDI  = 0x04;  // write disable
const uint8_t CMD_RDSR  = 0x05;  // read status register
const uint8_t CMD_WREN  = 0x06;  // write enable

const uint8_t BIT_WIP   = 0;	 // write in progress
const uint8_t BIT_WEL   = 1;	 // write enable latch
const uint8_t BIT_BP0   = 2;	 // block protect 0
const uint8_t BIT_BP1   = 3;	 // block protect 1
const uint8_t BIT_SRWD  = 7;	 // status register write disable

const uint8_t WRITE_TIMEOUT_MS = 10; // a write should only ever take 5 ms max

M95M01_t::M95M01_t(){
}

void M95M01_t::begin(uint8_t CS_pin, uint32_t speed_Hz){
	this->CS_pin = CS_pin;
	pinMode(this->CS_pin, OUTPUT);
	digitalWrite(this->CS_pin, HIGH);
	SPI.begin();
	// SPI_MODE0 and SPI_MODE3 are both valid - SCK idles low with 0, HIGH with 3
	// idling high reduces power consumption if using a pullup on SCK
	SPI.beginTransaction(SPISettings(speed_Hz, MSBFIRST, SPI_MODE3)); 
}

void M95M01_t::end(uint8_t CS_pin, uint32_t speed_Hz){
	this->CS_pin = CS_pin;
	pinMode(this->CS_pin, OUTPUT);
	digitalWrite(this->CS_pin, HIGH);
	SPI.begin();
	// SPI_MODE0 and SPI_MODE3 are both valid - SCK idles low with 0, HIGH with 3
	// idling high reduces power consumption if using a pullup on SCK
	SPI.beginTransaction(SPISettings(speed_Hz, MSBFIRST, SPI_MODE3)); 
}


uint8_t M95M01_t::write_byte(uint32_t address, uint8_t value){

	// returns 0 is the write was successful
	// returns 1 if the write timed out and hence was not successful

	unsigned long start_time_ms;
	uint8_t scratch;

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer(CMD_RDSR);
	start_time_ms = millis();	

	do{
		scratch = SPI.transfer(0); // dummy to clock out data
	} while ((bitRead(scratch, BIT_WIP) == 1) && (millis() < start_time_ms + WRITE_TIMEOUT_MS));
	digitalWrite(this->CS_pin, HIGH); 

	if(bitRead(scratch, BIT_WIP) == 1){ // if we're still busy writing, something has gone wrong;
		return(1); // timeout
	}

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer(CMD_WREN);
	digitalWrite(this->CS_pin, HIGH); 
	digitalWrite(this->CS_pin, LOW);
	SPI.transfer16((CMD_WRITE<<8) | (0x01 & (address >> 16))); // write instruction + top bit of 17-bit address
	SPI.transfer16(address & 0xFFFF); // bottom 16 bits of 17-bit address
	SPI.transfer(value);
	digitalWrite(this->CS_pin, HIGH);

	return(0); // success
}

uint8_t M95M01_t::read_byte(uint32_t address){

	// returns the value of the requested byte if the read was successful
	// returns 0 if the read timed out and was not successful (ambiguous)

	unsigned long start_time_ms;
	uint8_t scratch;

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer(CMD_RDSR);
	start_time_ms = millis();	

	do{
		scratch = SPI.transfer(0); // dummy to clock out data
	} while ((bitRead(scratch, BIT_WIP) == 1) && (millis() < start_time_ms + WRITE_TIMEOUT_MS));
	digitalWrite(this->CS_pin, HIGH); 

	if(bitRead(scratch, BIT_WIP) == 1){ // if we're still busy writing, something has gone wrong;
		return(0); // timeout
	}

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer16((CMD_READ<<8) | (0x01 & (address >> 16))); // write instruction + top bit of 17-bit address
	SPI.transfer16(address & 0xFFFF); // bottom 16 bits of 8-bit address
	scratch = SPI.transfer(0x00); // dummy payload to clock data out
	digitalWrite(this->CS_pin, HIGH);

	return(scratch);
}

uint8_t M95M01_t::write_array(uint32_t address, uint8_t value_array[], const uint32_t array_length){

	// returns 0 is the write was successful
	// returns 1 if the write timed out and hence was not successful

	unsigned long start_time_ms;
	uint8_t scratch;
	uint32_t i; // page counter
	uint32_t j = address; // byte address counter
	uint32_t page_start_address;

	for(i=page(address); i<=page(address+array_length-1); i++){	

		if(i == page(address)){
			page_start_address = address; // for the first page, start at the given address in case we're dropping in to the middle of a page
		} else {
			page_start_address = i*page_size; // else we start at the beginning of the current page
		}

		digitalWrite(this->CS_pin, LOW);
		SPI.transfer(CMD_RDSR);
		start_time_ms = millis();	

		do{
			scratch = SPI.transfer(0); // dummy to clock out data
		} while ((bitRead(scratch, BIT_WIP) == 1) && (millis() < start_time_ms + WRITE_TIMEOUT_MS));
		digitalWrite(this->CS_pin, HIGH); 

		if(bitRead(scratch, BIT_WIP) == 1){ // if we're still busy writing, something has gone wrong;
			return(1); // timeout
		}

		digitalWrite(this->CS_pin, LOW);
		SPI.transfer(CMD_WREN);
		digitalWrite(this->CS_pin, HIGH); 
		digitalWrite(this->CS_pin, LOW);
		SPI.transfer16((CMD_WRITE<<8) | (0x01 & (page_start_address >> 16))); // write instruction + top bit of 17-bit address
		SPI.transfer16(page_start_address & 0xFFFF); // bottom 16 bits of 17-bit address
		do{
			SPI.transfer(value_array[j-address]);
		} while(page_address(++j)!=0);
		digitalWrite(this->CS_pin, HIGH);
	}

	return(0); // success
}

uint8_t M95M01_t::read_array(uint32_t address, uint8_t value_array[], const uint32_t array_length){

	// returns 0 is the read was successful
	// returns 1 if the read timed out and hence was not successful

	unsigned long start_time_ms = millis();
	uint8_t scratch;
	uint32_t j; // byte address counter

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer(CMD_RDSR);
	start_time_ms = millis();	

	do{
		scratch = SPI.transfer(0); // dummy to clock out data
	} while ((bitRead(scratch, BIT_WIP) == 1) && (millis() < start_time_ms + WRITE_TIMEOUT_MS));
	digitalWrite(this->CS_pin, HIGH); 

	if(bitRead(scratch, BIT_WIP) == 1){ // if we're still busy writing, something has gone wrong;
		return(1); // timeout
	}

	digitalWrite(this->CS_pin, LOW);
	SPI.transfer16((CMD_READ<<8) | (0x01 & (address >> 16))); // write instruction + top bit of 17-bit address
	SPI.transfer16(address & 0xFFFF); // bottom 16 bits of 17-bit address
	for(j=address; j<address+array_length; j++){
		value_array[j-address] = SPI.transfer(0x00); // dummy payload to clock data out
	}
	digitalWrite(this->CS_pin, HIGH);

	return(0);
}

uint32_t M95M01_t::page(uint32_t address){
	if(address < (uint32_t)page_size){
		return(0);
	} else {
		return(address / page_size);
	}
}

uint8_t M95M01_t::page_address(uint32_t address){

	return(address % page_size);

}


M95M01_t M95M01 = M95M01_t();
