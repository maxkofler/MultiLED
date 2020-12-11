/* 
* MultiLED.h
*
* Created: 30.11.2020 07:44:14
* Author: Kofler Max


--------------------------------------------

Currently supported PORTS:
	-PORTB
	-PORTD
	
Warning: currently it is not possible to use multiple instances of this Library! Port usage
has to be done with "#define MULTILED_PORTx", x = B or D.


*/

#define MULTILED_PORTA 0x0022
#define MULTILED_PORTB 0x0025
#define MULTILED_PORTC 0x0028
#define MULTILED_PORTD 0x002B
#define MULTILED_PORTE 0x002E
#define MULTILED_PORTF 0x0031
#define MULTILED_PORTG 0x0034
#define MULTILED_PORTH 0x0102
#define MULTILED_PORTJ 0x0105
#define MULTILED_PORTK 0x0108
#define MULTILED_PORTL 0x010B

#ifdef __MULTILED_CPP__

	#ifndef MULTILED_COMPILER_OK
		#warning "The steps array can not be detected by the compiler! Size calculation: LEDCount * 3 * 8 (to hide this, type #define MULTILED_COMPILER_OK)"
	#endif

	#ifndef F_CPU
		#error "F_CPU not defined! Please define the Frequency of your CPU!"
	#endif
#endif



#ifndef __MULTILED_H__
#define __MULTILED_H__

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

struct RGB{
	volatile uint8_t red;
	volatile uint8_t green;
	volatile uint8_t blue;
};

class MultiLED
{
public:
	MultiLED(uint16_t port_address);
	~MultiLED();

	//Preparation:
	void addStrips(uint8_t ammount, uint16_t maxLEDCount);
	bool addRGBArray(uint8_t stripID, uint16_t len, RGB* array);

	//Refresh the Strip
	void prepareSteps();
	void writeToStrip();
	void show();
	
private:
	//LED Strip RGB array:
	RGB** ledArray;
	uint16_t* ledCounts;
	
	uint8_t stripCount;
	
	uint16_t cLeds;
	uint16_t stepC;
	
	uint16_t portAddr;
	
	volatile uint8_t* steps;

	

}; //MultiLED

#endif //__MULTILED_H__
