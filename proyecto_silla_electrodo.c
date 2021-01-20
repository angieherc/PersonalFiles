//Demonstration of the ADC with C18 compiler and PWM
//the ADC is the only source of interrupt in the system
//multiply each volt for 5
//(5-0/1023) 4.8 

#include <p18f4550.h>
#include <xlcd.h>
#include <delays.h>
#include <adc.h>
#include "stdlib.h"
#include "stdio.h"
#include "timers.h"

//no watchdog and internal clock
#pragma config WDT = OFF
#pragma config FOSC = INTOSCIO_EC
#pragma config PBADEN = OFF
#pragma interrupt ISR_function

//unsigned char FASES= 0x06;
void rangos(void);
void Derecha(void);
void Avanza(void);
void Izquierda(void);
void Frena(void);
void Atras(void);
void InitializeBoard(void);
//void adc_isr(void);	


//declaration of the umbrella isr function

#pragma code interrupt_vector = 0x08
void interrupt_vector(){
	_asm GOTO ISR_function _endasm
}
#pragma code /* return to the default code section */

unsigned short dataADC = 0;
unsigned short localADC = 0;
unsigned char adcFlag = 0;
 
unsigned char banderaDerecha=0;
unsigned char banderaIzq=0;
unsigned char banderaAvanza=0;
unsigned char banderaFrena=0;
unsigned char banderaAtras=0;
unsigned char state;
unsigned char rango;

void ISR_function(void)
{
	//as the only source of interrupt is the ADC
	PIR1bits.ADIF = 0;
	//reads the current conversion value 
	dataADC = ReadADC(); //salida del circuito 
	//SetDCPWM1(dataADC);
	//triggers the next conversion
	ConvertADC();	
	//updates user's flag
	//adcFlag = 1;
}

void main (void)
{
  	unsigned char i, n;
  	unsigned char number[5];
  	
	InitializeBoard();

  	//starts the conversion
  	ConvertADC();
	//infinite loop
	while(1)
	{
		//Derecha(velocidad);
		//rango = floor(dataADC*255/5);
		rangos();
	}

	
}

void InitializeBoard(void)
{
	//internal clock set to 4MHz
  	OSCCONbits.IRCF0=0;
  	OSCCONbits.IRCF1=1;
  	OSCCONbits.IRCF2=1;

  	//enables global interrupt bit
  	INTCONbits.GIE = 1;
  	INTCONbits.PEIE = 1;
	TRISD = 0x00;//Port D is the output port
	
  	//initialiazes the ADC
	TRISAbits.TRISA0=1;
	//AN0/RA0 is the analog channel
	//VDD and VSS are the references
	//right justified, 16TAD and FOSC/32
	//ADC interrupt is on
	//number 14 is loaded as ADCON1(3:0)=1110
	OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_16_TAD,
			ADC_CH0 & ADC_INT_ON & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS,
			14);
}


void Avanza(void)
{
	while(banderaAvanza=1);
	PORTD= 0b00000000;//primero el motor de la derecha 
	Delay10TCYx(1); 
	PORTD= 0b11001100;
	Delay10TCYx(1); 
	PORTD= 0b01100110;
	Delay10TCYx(1); 
	PORTD= 0b00110011;
	Delay10TCYx(1); 
	PORTD= 0b10011001;
	Delay10TCYx(1); 
}

 
void Derecha(){ 
	while (banderaDerecha=1)
	{
		PORTD= 0b11001100; //paso 1 
		Delay10TCYx(1); 
		PORTD= 0b01101001;  //paso 2 
		Delay10TCYx(1); 
		PORTD= 0b00110011;  //paso 3 
		Delay10TCYx(1); 
		PORTD= 0b10010110;  // paso 4 
		Delay10TCYx(1);
	}
} 

void Izquierda(void){ 
	while(banderaIzq=1)
	{
		PORTD= 0b11000000; //paso 1 
		Delay10TCYx(1); 
		PORTD= 0b10011100;  //paso 2 
		Delay10TCYx(1); 
		PORTD= 0b00110110;  //paso 3 
		Delay10TCYx(1); 
		PORTD= 0b01101001;  // paso 4 
		Delay10TCYx(1);
	}
} 
void Frena(void)
{
	PORTD=0b00000000; 
}
void Atras (void)
{
	while(banderaAtras=1)
	{	
		PORTD= 0b11001100; //paso 1 
		Delay10TCYx(1); 
		PORTD= 0b10011001;  //paso 2 
		Delay10TCYx(1); 
		PORTD= 0b00110011;  //paso 3 
		Delay10TCYx(1); 
		PORTD= 0b01100110;  // paso 4 
		Delay10TCYx(1);
	}
}
void rangos(void)
{
	switch(rango)
		{
			case 0: 
					if (rango >=20 && rango<=25)//5volt
					{
					banderaAvanza=1;
					state=1;
					}
			
					break;

			case 1: if (rango >=15 && rango<=20)//4volt
					banderaFrena=1;
					state = 2;
					break;

			case 2: if (rango >=0 && rango<=5) //1volt
					banderaIzq=1;
					state = 3;
					break;

			case 3: if (rango >=5 && rango<=10) // 2volt
					banderaDerecha=1;
					state = 4;
					break;
			case 4:	if (rango >=10 && rango<=15) // 3volt
					banderaAtras=1;
					state = 0;
					break;
			default: banderaFrena = 1;
					break;
		}
}
