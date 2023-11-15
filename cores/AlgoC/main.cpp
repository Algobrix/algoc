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
    if (!(PINC & (1 << PC0)))
    { 
		if(chk4TimeoutSYSTIM(g_button_timer,500) == SYSTIM_TIMEOUT)		
		{
			if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_RUN)
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_PAUSE;
				// millis_stop();
			}
			else 
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_RUN;
				// millis_start();
			}
		}
		else
		{
			if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_RUN)
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_HALT;
				PCIFR = 0b00000000; //Clear all flags
				resetAllThreads();
				PCMSK1 &= ~(0x01);
				PCIFR = 0b00000000; //Clear all flags
			}
			else
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
			}
		}
		g_button_timer = getSYSTIM();
    }
	PCIFR = 0b00000000; //Clear all flags
}

int main(void)
{
    init();


    initVariant();
    initALGOBOT();

	// time_t t;
	// srand((unsigned) time(&t));

    PCMSK1 |= 0x00; //PCINT8 PD5 C0
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
	uint8_t prevButtonState = digitalRead(PLAY_BUTTON_PIN);
    uint8_t buttonState = prevButtonState;
    uint8_t batteryError = 0x00;
    uint8_t batteryState = 0x00;
    uint32_t batteryTimer = getSYSTIM();
    g_playTimer = getSYSTIM();

	g_playState = 0x00;
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
    Serial.print(F("Device started. Battery voltage is "));
    Serial.print(g_battery_voltage/1000.);
    Serial.println(" V");
    if(g_battery_voltage < 7000)
    {
        g_upload_flag = 0;
        g_playState = 0x01;
        digitalWrite(PLAY_LED_PIN,g_playState);
    }



    for (;;) 
    {
        g_battery_voltage = getBatteryVoltage();
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
				else
				{
					batteryState ^= 0x01;
					digitalWrite(POWER_LED_PIN,batteryState);
				}
			}
        }

        if(((g_battery_voltage > 7000) && ((buttonState == 0) && (g_runFlag == 1))) || (g_upload_flag != 0x00))
        {
			g_upload_flag = 0;
            g_ALGOBOT_INFO.state = ALGOBOT_STATE_RUN;
			uint8_t p = 0;
			Serial.println("\r\n\r\n");
			for( p = 0; p < 20; p++ )
			{
				Serial.print(F("-"));
			}
			Serial.print(F("\r\nApplication started\r\n"));
			for( p = 0; p < 20; p++ )
			{
				Serial.print(F("-"));
			}
			Serial.println("");
			PCIFR = 0b00000000; //Clear all flags
            PCMSK1 |= 0x01;
            digitalWrite(PLAY_LED_PIN,0);

			chkALGOBOT();
			application(threadAlgoC);
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
			PCMSK1 &= ~(0x01);
			PCIFR = 0b00000000; //Clear all flags
			g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
			Serial.println("Stopping");

			stopALGOBOT();
			g_playButtonTimer = getSYSTIM();
			while(digitalRead(PLAY_BUTTON_PIN) == 0)
			{
				if(chk4TimeoutSYSTIM(g_playButtonTimer,3000) == SYSTIM_TIMEOUT)
				{
					stopALGOBOT();
					pinMode(PLAY_BUTTON_PIN,OUTPUT);
					digitalWrite(PLAY_BUTTON_PIN,HIGH);
					g_runFlag = 0;
					digitalWrite(PLAY_LED_PIN,0x00);
					batteryState == 0x00;
					digitalWrite(POWER_LED_PIN,0x00);
				}
			}
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
    pinMode(PLAY_BUTTON_PIN,OUTPUT);
    digitalWrite(PLAY_BUTTON_PIN,HIGH);
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,5) == SYSTIM_KEEP_ALIVE)
	{

	}
    digitalWrite(PLAY_BUTTON_PIN,LOW);
    pinMode(PLAY_BUTTON_PIN,INPUT);
    vout = analogRead(POWER_METER_PIN);
    vout = (vout * 5000) / 1024;
    vin = vout / (R2 / (R1 + R2));
    return vin;
}

uint8_t chkBTN(void)
{
	switch(g_playButtonState)
	{
		case (PLAY_BUTTON_STATE_IDLE):
		{
			if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_IDLE)
			{
				if(digitalRead(PLAY_BUTTON_PIN) == 0)
				{
					g_playButtonState = PLAY_BUTTON_STATE_LOW;
					g_playButtonTimer = getSYSTIM();
				}
			}
			break;
		}
		case (PLAY_BUTTON_STATE_LOW):
		{
			if(chk4TimeoutSYSTIM(g_playButtonTimer,3000) == SYSTIM_TIMEOUT)
			{
				stopALGOBOT();
				pinMode(PLAY_BUTTON_PIN,OUTPUT);
				digitalWrite(PLAY_BUTTON_PIN,HIGH);
				g_runFlag = 0;
				digitalWrite(PLAY_LED_PIN,0x00);
				digitalWrite(POWER_LED_PIN,0x00);
				while(digitalRead(PLAY_BUTTON_PIN) == 0)
				{

				}
				delay(50);
			}
			if(digitalRead(PLAY_BUTTON_PIN) != 0)
			{
				g_playButtonState = PLAY_BUTTON_STATE_IDLE;
				return 0;
			}
			return 1;
			break;
		}
		case (PLAY_BUTTON_STATE_HIGH):
		{
			break;
		}
		default:
		{
			g_playButtonState = PLAY_BUTTON_STATE_IDLE;
		}
	}
	return 1;
}