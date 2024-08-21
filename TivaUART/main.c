#include "TM4C123.h"
#include <stdint.h>
#include <stdlib.h>

void Delay(unsigned long counter);
void UART5_Transmitter(unsigned char data);

int main(void)
{
	/***** Enable CLK for UART5 & PORTE *****/
	SYSCTL->RCGCUART |= 0x20;
	SYSCTL->RCGCGPIO |= 0x10;
	Delay(1);
	
	/***** UART5 initialization *****/
	UART5->CTL = 0;         // Disable UART5
		
	// Set baudrate
	UART5->IBRD = 104;
	UART5->FBRD = 11;       
	
	UART5->CC = 0;          // Use system CLK
	UART5->LCRH = 0x60;     // 8-bit length, no parity bit, 1 stop bit
	UART5->CTL = 0x301;     // Re-enable UART5

	GPIOE->DEN = 0x30;        // Set Rx & Tx pins to digital
	GPIOE->AFSEL = 0x30;      // Use alternative function
	GPIOE->AMSEL = 0;    		  // Disable analog functions
	GPIOE->PCTL = 0x00110000; // Use PE4 & PE5 for UART
	  
	/***** Enable interrupt *****/ 
	UART5->ICR &= ~(0x010);     // Clear receive interrupt
	UART5->IM  = 0x0010;
	NVIC->ISER[1] |= 0x20000000; // Enable interrupt for UART5

	Delay(10); 
	while(1);
}


// Interrupt function at IRQ61
void UART5_Handler( void )
{
	unsigned char rx_data = 0;
	
	UART5->ICR &= ~(0x010); // Clear receive interrupt
	rx_data = UART5->DR ;   // get the received data byte
	
	// Send capitalized letter
	if(rx_data >= 'a' && rx_data <= 'z')
	{
		rx_data -= 32;
	}
	// Send successive character
	else if (rx_data >= 'A' && rx_data <= 'Z')
	{
		if (rx_data == 'Z') rx_data = 'A';
		else rx_data++;
	}
	// Send 0 if not a letter
	else
	{
		rx_data = '0';
	}
	
	// Send reply
	UART5_Transmitter(rx_data);
}

// Sending data
void UART5_Transmitter(unsigned char data)  
{
    while((UART5->FR & (1<<5)) != 0); // Wait for empty buffer
    UART5->DR = data;               
}


void Delay(unsigned long counter)
{
	unsigned long i = 0;
	for(i=0; i< counter; i++);
}