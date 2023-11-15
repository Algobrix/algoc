/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/


/* 
* Algobrix Brain Block 
* based on Atmega328PB
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define NUM_DIGITAL_PINS            24
#define NUM_ANALOG_INPUTS           8


/*pins which can deliver PWM */
//#define digitalPinHasPWM(p)   (((p) >= 0 && (p) <= 3) || ((p) >= 10 && (p)<= 13) || (p) == 6)
# define digitalPinHasPWM(p)        ((p) == 0 || (p) == 1 || (p) == 2 || (p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10 || (p) == 11)

/* First SPI */
static const uint8_t SS   = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK  = 13;

/* 2nd SPI */
//static const uint8_t SS1   = 16;
//static const uint8_t MOSI1 = 17;
//static const uint8_t MISO1 = 18;
//static const uint8_t SCK1  = 19;


/* Default I2C is I2C1, because I2C0 is at the additionally pin header */
static const uint8_t SDA = 18;	// PC4
static const uint8_t SCL = 19;	// PC5

// IGONRE FOR NOW ....
//static const uint8_t SDA0 = 21;	//PC4
//static const uint8_t SCL0 = 20; //PC5

/* Second I2C */
//static const uint8_t SDA1 = 8;	// PE0
//static const uint8_t SCL1 = 7;	// PE1

#undef LED_BUILTIN
#define LED_BUILTIN 	21


/*Pin Change Interrupt Control Register */
#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 23) ? (&PCICR) : ((uint8_t *)0))

#define digitalPinToPCICRbit(p) (((p) <= 5) ? 0 : (((p) <= 11) ? 1 : (((p) <= 19) ? 2 : 3)))


/* Pin Change Mask Register 0*/
// Pin Change interrupts
// PCINT0 - PCINT5 = PB0-PB5	PCMSK0
// PCINT8 - PCINT13 = PC0-PC5	PCMSK1
// PCINT16 - PCINT23 = PD0-PD7	PCMSK2
// PCINT24 - PCINT27 = PE0-PE3	PCMSK3

/* map digital pin to pin change mask register */
#define digitalPinToPCMSK(p)    (((p) <= 5) ? (&PCMSK0) : (((p) <= 11) ? (&PCMSK1) : (((p) <= 19) ? (&PCMSK2) : (((p) <= 23) ? (&PCMSK3) : ((uint8_t *)0)))))


/* map arduino pin to bit of the PCMSK Register */
#define digitalPinToPCMSKbit(p) \
	( \
		(p) == 0 ? 0 : \
		((p) == 1 ? 1 : \
	 	((p) == 2 ? 2 : \
		((p) == 3 ? 3 : \
		((p) == 4 ? 4 : \
		((p) == 5 ? 5 : \
		((p) == 6 ? 0 : \
		((p) == 7 ? 1 : \
		((p) == 8 ? 2 : \
		((p) == 9 ? 3 : \
		((p) == 10 ? 4 : \
		((p) == 11 ? 5 : \
		((p) == 12 ? 0 : \
		((p) == 13 ? 1 : \
		((p) == 14 ? 2 : \
		((p) == 15 ? 3 : \
		((p) == 16 ? 4 : \
		((p) == 17 ? 5 : \
		((p) == 18 ? 6 : \
		((p) == 19 ? 7 : \
		((p) == 20 ? 0 : \
		((p) == 21 ? 1 : \
		((p) == 22 ? 2 : \
		((p) == 23 ? 3 : 0) \
	)))))))))))))))))))))))


/* map arduino pins to external interrupt */

// Interrupts
#define EXTERNAL_NUM_INTERRUPTS     (2)

#define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT))


/* Analog Pins*/
#define analogInputToDigitalPin(p)  (((p - 14) < NUM_ANALOG_INPUTS) ? (p) - 14 : -1)

static const uint8_t A0 = 14;
static const uint8_t A1 = 15;
static const uint8_t A2 = 16;
static const uint8_t A3 = 17;
static const uint8_t A4 = 18;
static const uint8_t A5 = 19;
static const uint8_t A6 = 20;
static const uint8_t A7 = 21;

/* Analog Channel Pin Assignment */
//extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
//#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	(uint16_t) &DDRE,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	(uint16_t) &PINE,
};

/* Port Settings per externel IO Pin*/
const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PD, /* 0 */
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PB, /* 8 */
	PB,
	PB,
	PB,
	PB,
	PB,
	PC, /* 14 */
	PC,
	PC,
	PC,
	PC,
	PC,
	PE, /* 20 */
	PE,
	PE,
	PE
};

/* Pin - Port Setting*/
const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(0), /* 0, port D */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 8, port B */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(0), /* 14, port C */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(2), /* 20, port E */
	_BV(3),
	_BV(0),
	_BV(1)
};

/* Pin-Timer Assignment */
const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	TIMER3A,        /* 0 - port D */
	TIMER4A,
	TIMER4B, // TIMER3B or TIMER4B
	TIMER2B,
	NOT_ON_TIMER,
	TIMER0B,
	TIMER0A,
	NOT_ON_TIMER,
	NOT_ON_TIMER,   /* 8 - port B */
	TIMER1A,
	TIMER1B,
	TIMER2A,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,   /* 14 - port C */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,   /* 20 - port E */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
};

/*
const uint8_t PROGMEM analog_pin_to_channel_PGM[] = {
	0,  // A0               PC0                 ADC0
    1,  // A1               PC1                 ADC1    
    2,  // A2               PC2                 ADC2    
    3,  // A3               PC3                 ADC3
    4,  // A4               PC4                 ADC4    
    5,  // A5               PC5                 ADC5    
    6,  // A6               PE2                 ADC6
    7,  // A7               PE3                 ADC7
};
*/
#endif /* ARDUINO_MAIN */

/*
 * Pinout Map:
 * | Name | Arduino Pin # |
 * | PD0  |      0        |
 * | PD1  |      1        |
 * | PD2  |      2        |
 * | PD3  |      3        |
 * | PD4  |      4        |
 * | PD5  |      5        |
 * | PD6  |      6        |
 * | PD7  |      7        |
 * | PB0  |      8        |
 * | PB1  |      9        |
 * | PB2  |      10       |
 * | PB3  |      11       |
 * | PB4  |      12       |
 * | PB5  |      13       |
 * | PC0  |      14\A0    |
 * | PC1  |      15\A1    |
 * | PC2  |      16\A2    |
 * | PC3  |      17\A3    |
 * | PC4  |      18\A4    |
 * | PC5  |      19\A5    |
 * | PE2  |      20\A6    |
 * | PE3  |      21\A7    |
 * | PE0  |      22       |
 * | PE1  |      23       |
 */
#define PLAY_BUTTON_PIN 14
#define PLAY_LED_PIN 4
#define POWER_LED_PIN 8
#define POWER_METER_PIN A6

// BLE Pins
#define BLE_EN_PIN 15

// LED Pins
#define LED_A_PIN 19
#define LED_B_PIN 18

// Motor Pins
#define MOTOR_A_DIR 10
#define MOTOR_A_PWM 9
#define MOTOR_A_ENCODER 21
#define MOTOR_B_DIR 2
#define MOTOR_B_PWM 3
#define MOTOR_B_ENCODER 5
#define MOTOR_C_DIR 13
#define MOTOR_C_PWM 6
#define MOTOR_C_ENCODER 23

// Sensor Pins
#define SENSOR_A_PIN A3
#define SENSOR_B_PIN A2

// Sound Module Pins
#define SOUND_TX_PIN 22
#define SOUND_STATE_PIN 7


#define SERIAL_PORT_MONITOR   		Serial
#define SERIAL_PORT_HARDWARE  		Serial

#define SERIAL_PORT_HARDWARE1       Serial1
#define SERIAL_PORT_HARDWARE_OPEN   Serial1

#endif
