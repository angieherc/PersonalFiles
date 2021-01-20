#include "p18f4455.h"
#include "portb.h"
#include "timers.h"
#include "usart.h"
#include "stdlib.h"
#include "xlcd.h"
#include "delays.h"
#include "stdio.h"
#include "adc.h"
#include "i2c.h"

#pragma config WDT = OFF, FOSC = INTOSCIO_EC, PBADEN = OFF
#pragma config PLLDIV   = 1  
#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config LPT1OSC  = OFF
#pragma config LVP      = OFF
#pragma config XINST    = OFF       
#pragma config CP0      = OFF
#pragma config CP1      = OFF
#pragma config CPB      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
#pragma config WRTB     = OFF       
#pragma config WRTC     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
#pragma config EBTRB    = OFF
//#pragma interrupt ISR_function
int i;
int data;
int reg;
unsigned char buffer[40];

void InitializeBoard(void);
void InitI2C(void);



void InitializeBoard(){
	//Clock Configuration (OSCCONbits = 01100000)
  	OSCCONbits.IRCF0=0; 
  	OSCCONbits.IRCF1=1;
  	OSCCONbits.IRCF2=1; 
	
	//PortC configuration
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC7 = 0;
	TRISCbits.TRISC6 = 1;
	
	
	
	TRISD = 0x00;
		

	//Enabling Global and Peripherial interrupts
	INTCONbits.GIE = 0;
	INTCONbits.PEIE = 0;
	//Enabling LCD
	//Display Configuration
	OpenXLCD(FOUR_BIT & LINES_5X7);
	while(BusyXLCD());
	WriteCmdXLCD(DON & CURSOR_OFF & BLINK_OFF);
	while(BusyXLCD());
	WriteCmdXLCD(SHIFT_DISP_LEFT);
	//Initial Conditions
	while(BusyXLCD());
	SetDDRamAddr(0x00);
	while(BusyXLCD());
	putrsXLCD("LCD funcionando");
	while(BusyXLCD());
	SetDDRamAddr(0x40);
	while(BusyXLCD());
	putrsXLCD(" :D ");
	for(i = 0;i < 20; i++){
		Delay100TCYx(0);
	}	
	
	InitI2C();
	}

/****************MAIN FUNCTION***************************/


void main (){

	InitializeBoard();
	
	while(1){
		
	
			sprintf(buffer, (const far rom char*) " %d. cm.      " , (int)data);
			while(BusyXLCD());
			SetDDRamAddr(0x40);
			while(BusyXLCD());
			putsXLCD(buffer);
		
	
	
	}

}

/*Inicializa el I2C*/

void InitI2C(void)
{
  	//here is the I2C setup from the Seeval32 code.
	
	TRISBbits.TRISB0 = 1; //Configure SCL
                	  //as Input
	TRISBbits.TRISB1 = 1; //Configure SDA
            	      //as Input  	

 	SSPSTAT = 0x80;   //Disable SMBus &
        	          //Slew Rate Control
  	SSPCON1 = 0x28;   //Enable MSSP Master
  	SSPADD = 0x09;    //Should be 0x09
    	              //for 100kHz
  	SSPCON2 = 0x00;   //Clear MSSP Conrol Bits
}


//------------------------------
// Inicializacion Acelerometro
//------------------------------
void  accel_init(){
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(0x3A); //0x1D desplazado con un 0
    WriteI2C(0x16); //Registro MODE CONTROL
    WriteI2C(0x05); //Medición contínua
    StopI2C();
    IdleI2C();
}

//-----------------------------------
// Escritura 1 registro
//-----------------------------------
void  accel_write(int reg, int data){
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(0x3A); //0x1D desplazado con un  0
    WriteI2C(reg);  //Registro a escribir
    WriteI2C(data); //Dato a escribir en registro
    StopI2C();
    IdleI2C();
}

//-----------------------------------
// Lectura 1 registro
//-----------------------------------
int accel_read(int data){
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(0x3A); //0x1D desplazado con un  0
    WriteI2C(reg);  //Registro a leer
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(0x3B); //0x1D desplazado con un  1
    data=ReadI2C(); //Dato leido->data
    StopI2C();
    IdleI2C();
    return(data);
}

//18 instruction cycles delay using the delays library
void DelayFor18TCY(void)
{
	int i;
	for(i=0; i <18; i++) Delay1TCY();
}

//15ms-delay using the delays libraries, assuming a 1us-instruction cycle
//with a 4-MHz clock
void DelayPORXLCD()
{
	Delay1KTCYx(150); //it had to be 15
}

//5ms-delay using the delays libraries, assuming a 1us-instruction cycle
//with a 4-MHz clock
void DelayXLCD()
{
	Delay1KTCYx(50); //it had to be 5
}

