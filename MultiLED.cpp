/* 
* MultiLED.cpp
*
* Created: 30.11.2020
* Author: Kofler Max
*/

#define __MULTILED_CPP__
#include "MultiLED.h"


void MultiLED::addStrips(uint8_t ammount, uint16_t maxLEDCount){
	ledArray = (RGB**)malloc(ammount*3);
	ledCounts = (uint16_t*)malloc(ammount*2);
	stripCount = ammount;
	cLeds = maxLEDCount;
	stepC = (cLeds*8*3)+8;
	steps = (uint8_t*)malloc(stepC);
}

MultiLED::MultiLED(uint16_t port_address){
	portAddr = port_address;
}

MultiLED::~MultiLED(){
	free(ledArray);
	free(ledCounts);
	free((void*)steps);
}

bool MultiLED::addRGBArray(uint8_t stripID, uint16_t len, RGB* array){
	//First check if the array length is not longer than the maxLEDCount provided in addStrips
	if (len <= cLeds){
		//Then check if the stripID is not larger than there are strips registered!
		if (stripID < stripCount){
			ledArray[stripID] = array;
			ledCounts[stripID] = len;
			return true;
		}else{ return false; }
	}else{ return false; }
	return false;
}

void MultiLED::prepareSteps(){
	
	
	
	for (uint16_t i = 0; i < stepC; i++){
		steps[i] = 0;
	}

	for (uint16_t ledArrayID = 0; ledArrayID < stripCount; ledArrayID++){
		uint16_t ledC = ledCounts[ledArrayID];
		RGB *leds = ledArray[ledArrayID];
		
		
		uint16_t stepIndex = 0;
		uint16_t led = 0;
		//Transmission in GRB
		while(led < ledC){
			//Cycle trough G
			//Serial.print("G=");
			//Serial.println((int)leds[led].green);
			for (uint8_t i = 8; i > 0; i--){
				steps[stepIndex] |= ((leds[led].green >> (i-1)) & 1) << ledArrayID;
				stepIndex++;
			}
			//Cycle trough R
			//Serial.print("R=");
			//Serial.println((int)leds[led].red);
			for (uint8_t i = 8; i > 0; i--){
				steps[stepIndex] |= ((leds[led].red >> (i-1)) & 1) << ledArrayID;
				stepIndex++;
			}
			//Cycle trough B
			//Serial.print("B=");
			//Serial.println((int)leds[led].blue);
			for (uint8_t i = 8; i > 0; i--){
				steps[stepIndex] |= ((leds[led].blue >> (i-1)) & 1) << ledArrayID;
				stepIndex++;
			}
			led ++;
		}
	}
}

void MultiLED::writeToStrip(){
	//Disable interrupts
	cli();
	volatile uint8_t *reg16 = (volatile uint8_t *)0x10;
	volatile uint8_t *reg17 = (volatile uint8_t *)0x11;
	volatile uint8_t *reg18 = (volatile uint8_t *)0x12;
	volatile uint8_t *reg19 = (volatile uint8_t *)0x13;
	
	
	
	volatile uint16_t portAddress = portAddr;
	
	volatile uint16_t stepsAddr = (uint16_t)steps;
	//volatile uint16_t stepsAddr = (volatile uint16_t)steps;
	uint16_t stepCount = this->stepC;
	//r18 = low byte for counting down
	//r19 = high byte for counting down
	*reg18 = (uint8_t)stepCount;
	*reg19 = (stepCount >> 8 );

	//Low byte
	*reg16 = stepsAddr & 0x00FF;
	//High byte
	*reg17 = (stepsAddr >> 8) & 0xFF;
	
	//Copy the steps address to the Z register
	asm volatile ("mov ZL, r16");
	asm volatile ("mov ZH, r17");
	
	//Load the port address into Y register
	asm volatile ("clr r16");
	asm volatile ("clr r17");
	*reg16 = portAddress & 0x00FF;			//LOW
	*reg17 = (portAddress >> 8) & 0xFF;		//HIGH
	asm volatile("mov YL, r16");
	asm volatile("mov YH, r17");
	
	
	asm volatile ("ldi r16, 0xFF");
	
	//The loop label:
	asm volatile ("loop:");
	
	//asm volatile ("nop");
	
	//Set the port to HIGH (r16):
	asm volatile ("st  Y, r16");
	
	//Load the step into r20 (uses 2 cycles!)
	asm volatile ("ld r20, Z+");
	
	//Binary and it with 0xFF
	//asm volatile ("andi r20, 0xFF");
	asm volatile("nop");
	
	asm volatile ("nop");
	asm volatile ("mov r16, r20");
	//asm volatile ("out 0x05, r16");
	asm volatile ("st  Y, r16");
	
	//Count down
	asm volatile ("dec r18");
	asm volatile ("cpi r18, 0");
	asm volatile ("breq zeror18");
	asm volatile ("nop");
	asm volatile ("ldi r16, 0x00");
	//asm volatile ("out 0x05, r16");
	asm volatile ("st  Y, r16");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("rjmp weiter");
	
	asm volatile ("zeror18:");
	asm volatile ("nop");
	asm volatile ("ldi r16, 0x00");
	//asm volatile ("out 0x05, r16");
	asm volatile ("st  Y, r16");
	asm volatile ("cpi r19, 0x00");
	asm volatile ("breq ende");
	asm volatile ("dec r19");
	asm volatile ("ldi r18, 0xFF");
	asm volatile ("rjmp weiter");
	
	//Go on to the next step
	asm volatile ("weiter:");
	asm volatile ("ldi r16, 0xFF");
	asm volatile ("rjmp loop");
	
	//End of this operation
	asm volatile ("ende:");
	asm volatile ("ldi r16, 0x00");
	//asm volatile ("out 0x05, r16");
	asm volatile ("st  Y, r16");
	asm volatile ("nop");
	
	//Reenable interrupts
	sei();

	//Waiting for reset trigger
	_delay_us(55);
}

void MultiLED::show(){
	prepareSteps();
	writeToStrip();
}