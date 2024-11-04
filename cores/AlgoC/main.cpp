/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include <systim.h>
#include <compileTime.h>
#include <../../libraries/Algobrix/algothread.h>
/* #include <../../libraries/Algobrix/algobot.h> */
#include <EEPROM.h>
#include <version.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <../../libraries/Algobrix/softserial.h>


#define ALGOC_VERSION							"v0.5.21"

#define R1 20000.0 // resistance of R1 (20K)
#define R2 10000.0 // resistance of R2 (10K)

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

void chkALGOBOT() __attribute__((weak));
void chkALGOBOT() { }

void stopALGOBOT() __attribute__((weak));
void stopALGOBOT() { }
void initALGOBOT() __attribute__((weak));
void initALGOBOT() { }

void chk4CmdALGOBOT() __attribute__((weak));
void chk4CmdALGOBOT() { }


void setup() __attribute__((weak));
void setup() { }

void loop() __attribute__((weak));
void loop() { }

/* void application(ALGOC) { } */
 /* __attribute__((weak)) */
//void setupUSB() __attribute__((weak));
//void setupUSB() { }

uint16_t getBatteryVoltage(void);


uint8_t g_playState = 0x00;
uint32_t g_playTimer;

enum PLAY_BUTTON_STATE
{
	PLAY_BUTTON_STATE_IDLE = 0x00,
	PLAY_BUTTON_STATE_LOW,
	PLAY_BUTTON_STATE_HIGH,
};

uint8_t g_playButtonState;
uint32_t g_playButtonTimer;
uint32_t g_battery_voltage_print;
uint16_t g_battery_voltage;

uint8_t g_serial_control_flags = 0x00;


volatile uint8_t g_runFlag = 0x00;
volatile uint8_t g_upload_flag = 0x00;
volatile uint32_t g_button_timer;

static uint8_t chkBTN(void);

ISR(PCINT1_vect) 
{
	if(!digitalRead(PLAY_BUTTON_PIN))
    { 
		if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_RUN)
		{
			g_button_timer = getSYSTIM();
			g_ALGOBOT_INFO.state = ALGOBOT_STATE_PAUSE;
			g_ALGOBOT_INFO.stopFlag = 0x01;
		}
		else if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
		{

		}
		else if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_POWER_OFF)
		{
			g_button_timer = getSYSTIM();
		}
		else
		{
			g_button_timer = getSYSTIM();
			g_ALGOBOT_INFO.state = ALGOBOT_STATE_INIT;
		}
    }
	else
	{

	}
	PCIFR = 0x00; //Clear all flags
	// PCIFR |= (1 << PCIF1);
	// PCIFR |= 0xff;
}

int main(void)
{
    init();
    initVariant();

	pinMode(PLAY_BUTTON_PIN,INPUT_PULLUP);
	uint8_t prevButtonState = digitalRead(PLAY_BUTTON_PIN);

	pinMode(PLAY_BUTTON_PIN,OUTPUT);
	digitalWrite(PLAY_BUTTON_PIN,0x01);
	// time_t t;
	// srand((unsigned) time(&t));

    PCMSK1 = 0x00; //PCINT8 PD5 C0
    PCIFR = 0b00000000; //Clear all flags
    PCICR |= 0x02; //Turn on port C PCIE2

#if defined(USBCON)
    USBDevice.attach();
#endif

// #define bleSerial Serial1
    setup();
    Serial.begin(115200);
	SoftSerial bleSerial(11);
	bleSerial.begin(115200);

	delay(100);
	bleSerial.write(0xf0);
	for(uint8_t i=1; i<9; i++) 
	{
		bleSerial.write(i);
	}

	delay(100);
	bleSerial.write(0x0F);
	for(uint8_t i=1; i<9; i++) 
	{
		bleSerial.write(i);
	}
	delay(100);
    uint8_t buttonState = prevButtonState;
    uint8_t batteryError = 0x00;
    uint8_t batteryState = 0x00;
    uint32_t batteryTimer = getSYSTIM();
    g_playTimer = getSYSTIM();

	g_playState = 0x01;
    pinMode(POWER_LED_PIN,OUTPUT);
	digitalWrite(PLAY_LED_PIN,g_playState);


	uint32_t eepromBuildTime;
	eepromBuildTime = EEPROM.read(0);
	eepromBuildTime = (eepromBuildTime << 8) | EEPROM.read(1);
	eepromBuildTime = (eepromBuildTime << 8) | EEPROM.read(2);
	eepromBuildTime = (eepromBuildTime << 8) | EEPROM.read(3);
	/* Serial.print("Build itme: "); */
	/* Serial.println(c_current_build_time,HEX); */
	/* Serial.print("EEPROM Build itme: "); */
	/* Serial.println(eepromBuildTime,HEX); */
    delay(1000);
   	if(eepromBuildTime != c_current_build_time)
	{
		g_upload_flag = 1;
		g_runFlag = 1;
		EEPROM.write(0,(c_current_build_time >> 24) & 0xff);
		EEPROM.write(1,(c_current_build_time >> 16) & 0xff);
		EEPROM.write(2,(c_current_build_time >> 8) & 0xff);
		EEPROM.write(3,c_current_build_time & 0xff);
		/* Serial.println("Save build to EEPROM"); */
	}
    g_battery_voltage = getBatteryVoltage();

	// time_t t;
	srand((unsigned) g_battery_voltage);
#ifdef SERIAL_ENABLE
    Serial.print(F("Device started with firmware version "));
    Serial.print(F(ALGOC_VERSION));
    Serial.print(F(". Battery voltage is "));
    Serial.print(g_battery_voltage/1000.);
    Serial.println(" V");
#endif
    if(g_battery_voltage < 5500)
    {
		Serial.println(F("Make sure battery block is connected and turned on"));
        g_upload_flag = 0;
        g_playState = 0x00;
        digitalWrite(PLAY_LED_PIN,g_playState);
		g_ALGOBOT_INFO.state = ALGOBOT_STATE_POWER_OFF;
	}
	else
	{
		g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
	}
	digitalWrite(PLAY_BUTTON_PIN,0x00);
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,200)==SYSTIM_KEEP_ALIVE);
	pinMode(PLAY_BUTTON_PIN,INPUT);
    initALGOBOT();

	PCIFR = 0b00000000; //Clear all flags
	PCMSK1 |= 0x01;

    for (;;) 
    {
        g_battery_voltage = getBatteryVoltage();
		// Serial.println("Power on: ");
		if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_POWER_OFF)
		{
			g_battery_voltage = getBatteryVoltage();
			if(g_battery_voltage > 5000)
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
				digitalWrite(PLAY_LED_PIN,1);
				delay_ms(200);
			}
			else
			{
				g_runFlag = 0;
				g_upload_flag = 0;
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_POWER_OFF;
			}
		}

        buttonState = g_serial_control_flags & SERIAL_CONTROL_PLAY ? 0 :  chkBTN() ;
		g_serial_control_flags &= ~(SERIAL_CONTROL_PLAY);
        if(chk4TimeoutSYSTIM(batteryTimer,200) == SYSTIM_TIMEOUT)
        {
            batteryTimer = getSYSTIM();
			if(g_runFlag != 0)
			{
				if(g_battery_voltage > 7500)
				{
					if(batteryState != 0x00)
					{
						batteryState == 0x00;
						digitalWrite(POWER_LED_PIN,batteryState);
					}
				}
				else if(g_battery_voltage > 7000)
				{
					if(batteryState != 0x01)
					{
						batteryState = 0x01;
						digitalWrite(POWER_LED_PIN,batteryState);
					}
				}
				else if(g_battery_voltage > 5500)
				{
					batteryState ^= 0x01;
					digitalWrite(POWER_LED_PIN,batteryState);
				}
				else
				{
					batteryState = 0x00;
					digitalWrite(POWER_LED_PIN,batteryState);
				}
			}
        }

        if(((g_battery_voltage > 7000) && ((buttonState == 0) && (g_runFlag == 1))) || (g_upload_flag != 0x00))
        {
			g_upload_flag = 0;
            g_ALGOBOT_INFO.state = ALGOBOT_STATE_RUN;
			uint8_t p = 0;

#ifdef SERIAL_ENABLE
			Serial.println("\r\n\r\n");
			for( p = 0; p < 75; p++ )
			{
				Serial.print(F("-"));
			}
			Serial.print(F("\r\nApplication started with firmware version "));
			Serial.print(F(ALGOC_VERSION));
			Serial.print(F(". Battery voltage is "));
			Serial.print(g_battery_voltage/1000.);
			Serial.println(" V");
			for( p = 0; p < 75; p++ )
			{
				Serial.print(F("-"));
			}
			Serial.println("");
#endif
            digitalWrite(PLAY_LED_PIN,0);

			chkALGOBOT();

            threadAlgoC.state = ALGOTHREAD_STATE_RUNNING;
            do
            {
			    application(threadAlgoC);
                threadAlgoC.state = ALGOTHREAD_STATE_IDLE;
                yield();
            } while(areAllThreadsCompleted() == false);
			// loop();
			if(chk4TimeoutSYSTIM(g_playTimer,200) == SYSTIM_TIMEOUT)
			{
				g_playState ^= 0x01;
				g_playTimer = getSYSTIM();
				digitalWrite(PLAY_LED_PIN,g_playState);
			}
			if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_RUN)
			{
				// g_ALGOBOT_INFO.state = ALGOBOT_STATE_WAIT4STOP;
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
				g_playState = 0x01;
				pinMode(PLAY_LED_PIN,OUTPUT);
				digitalWrite(PLAY_LED_PIN,g_playState);
				// while(g_ALGOBOT_INFO.state != ALGOBOT_STATE_IDLE)
				// {

				// }
			}
			g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
			PCIFR = 0x00; //Clear all flags

			stopALGOBOT();
			g_playButtonTimer = getSYSTIM();
			// while(digitalRead(PLAY_BUTTON_PIN) == 0)
			// {
			// 	if(chk4TimeoutSYSTIM(g_playButtonTimer,3000) == SYSTIM_TIMEOUT)
			// 	{
			// 		stopALGOBOT();
			// 		pinMode(PLAY_BUTTON_PIN,OUTPUT);
			// 		digitalWrite(PLAY_BUTTON_PIN,HIGH);
			// 		g_runFlag = 0;
			// 		digitalWrite(PLAY_LED_PIN,0x00);
			// 		batteryState == 0x00;
			// 		digitalWrite(POWER_LED_PIN,0x00);
			// 	}
			// }
			delay(50);
			if(g_runFlag)
			{
				g_playState = 0x01;
				digitalWrite(PLAY_LED_PIN,g_playState);
			}
		}
		if((g_battery_voltage > 7000) && (buttonState == 0) && (g_runFlag == 0))
		{
			g_runFlag = 1;
			digitalWrite(PLAY_LED_PIN,0x01);
			while(digitalRead(PLAY_BUTTON_PIN) == 0)
			{

			}
		}
        else if((buttonState == 0) && (g_battery_voltage <= 7000) && (g_runFlag == 1))
        {
            if(chk4TimeoutSYSTIM(g_battery_voltage_print,2000) == SYSTIM_TIMEOUT)
            {
                g_battery_voltage_print = getSYSTIM();
                Serial.print(F("Button press detected, but voltage is to low: "));
                Serial.print(g_battery_voltage/1000.);
				Serial.println(" V");
			}
		}
			

		chk4CmdALGOBOT();

        // if (serialEventRun) serialEventRun();
    }

    return 0;
}

uint16_t getBatteryVoltage(void)
{
    uint32_t vout = 0.0;
    uint32_t vin = 0.0;
    // pinMode(PLAY_BUTTON_PIN,OUTPUT);
    // digitalWrite(PLAY_BUTTON_PIN,HIGH);
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,5) == SYSTIM_KEEP_ALIVE)
	{

	}
    // digitalWrite(PLAY_BUTTON_PIN,LOW);
    // pinMode(PLAY_BUTTON_PIN,INPUT);
    vout = analogRead(POWER_METER_PIN);
    vout = (vout * 5000) / 1024;
    vin = vout / (R2 / (R1 + R2));
    return vin;
}

uint8_t chkBTN(void)
{
	if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_INIT)
	{
		uint32_t timer = getSYSTIM();
		while(digitalRead(PLAY_BUTTON_PIN) == 0)
		{
			g_battery_voltage = getBatteryVoltage();
			if(g_battery_voltage < 5000)
			{
				if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_POWER_OFF)
				{
					g_ALGOBOT_INFO.state = ALGOBOT_STATE_POWER_OFF;
					digitalWrite(PLAY_BUTTON_PIN,HIGH);
					pinMode(PLAY_BUTTON_PIN,LOW);
					digitalWrite(PLAY_LED_PIN,0);
					break;
				}
			}
		}
		if(g_battery_voltage > 7000)
		{
			return 0;
		}
	}

	return 1;
}
