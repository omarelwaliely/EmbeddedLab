#include "TM4C123GH6PM.h"
#include <stdio.h>
void UART0Tx(char const c);
void handleFinish();
int finished = 0;
char userIn[20];
char userInTemp[20];
int i =0;
float duty = 0.5;
int flag = 1;
int kfreq = 30;
#define CLEAR(x) memset(x,'\0',20)


int main(void)
{
	SYSCTL->RCGCUART |= 0x3; /* provide clock to UART0, UART1 */
	SYSCTL->RCGCUART |= 0x20; /* provide clock to UART5 */ 
	SYSCTL->RCGCGPIO |= 1; /* enable clock to PORTA */
	SYSCTL->RCGCGPIO |= 0x10; //e
	SYSCTL->RCGCGPIO |= 0x08; //d

/* UART0 initialization */
	UART0->CTL = 0; /* disable UART0 */
	UART0->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART0->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART0->CC = 0; /* use system clock */
	UART0->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART0->CTL = 0x301; /* enable UART0, TXE, RXE */

/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */
	
/* UART1 initialization, enabling RX interrupt */
	UART5->CTL = 0; /* disable UART0 */
	UART5->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART5->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART5->CC = 0; /* use system clock */
	UART5->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART5->IM |= 0x0010; /* enable RX interrupt */
	UART5->CTL = 0x301; /* enable UART0, TXE, RXE */
	
/* UART1 TX0 and RX0 use PB1 and PB0. Set them up. */
	GPIOE->DEN = 0x30; /* Make PA0 and PA1 as digital */ 
	GPIOE->AFSEL = 0x30; /* Use PA0,PA1 alternate function */
	GPIOE->PCTL = 0x00110000; /* configure PA0 and PA1 for UART */
 
	GPIOD->DIR = 0x40;
	GPIOD->DEN = 0x40;

	
	/* enable interrupt in NVIC and set priority to 3 */
 NVIC->IP[6] = 3 << 5; /* set interrupt no 5 priority to 3 */
  NVIC->IP[61] = 3 << 5; /* set interrupt no 5 priority to 3 */
 NVIC->ISER[1] |= 0x20000000; /* enable IRQ5 for UART0 */ //0100 0000
  NVIC->ISER[0] |= 0x00000040; /* enable IRQ5 for UART0 */ //0100 0000
 
 
 //SysTick->LOAD = (15999/(freq))* duty; /* reload with number of clocks per second */
 SysTick->LOAD = 16000;
  SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */

	GPIOD->DATA =  0x00; 
 
 __enable_irq(); /* global enable IRQs */

 while(1){
	 if(finished){
		 	 handleFinish();
		 finished = 0;
	 }
 };



 return 0;
}
void UART5_Handler(void)
{
volatile int readback;
 char c;
 if (UART5->MIS & 0x0010) /* if a receive interrupt has occurred */
 {
	 c = UART5->DR; /* read the received data */
	 if(c == 'Z'){
		 finished =1;
	 }
	 else{
		 userInTemp[i++] = c;
	 }
	UART5->ICR = 0x0010; /* clear Rx interrupt flag */
	readback = UART5->ICR; /* a read to force clearing of interrupt flag */
 }
 else
 {
 /* should not get here. But if it does, */
 UART5->ICR = UART5->MIS; /* clear all interrupt flags */
 readback = UART5->ICR; /* a read to force clearing of interrupt flag */
 }
}

void handleFinish(){
	int j= 0;
	for(j=0; j<i; j++){
		userIn[j] = userInTemp[j];
	}
	CLEAR(userInTemp);
	for(j=0;j<sizeof(userIn);j++){
		UART0Tx(userIn[j]);
	}
	if(strcmp(userIn, "freq+1") == 0){
		if (kfreq < 16000)
			{
				kfreq += 1;
			}
	}
	if(strcmp(userIn, "freq-1") ==0){
		if (kfreq > 1)
			{
				kfreq -= 1;
			}
	}
	if(strcmp(userIn, "freq+5") == 0){
		if (kfreq < 15995)
			{
				kfreq += 5;
			}
	}
	if(strcmp(userIn, "freq-5") ==0){
			if (kfreq > 5)
			{
				kfreq -= 5;
			}
	}
	if(strcmp(userIn, "duty+5") ==0){
		if (duty < 0.8) //must be this because due to the division the duty cycle of greater than this will not work
			{
				duty += 0.05;
			}
	}
	if(strcmp(userIn, "duty-5") ==0){
		if (duty > 0.2) //must be this because due to the division the duty cycle of greater than this will not work
			{
				duty -= 0.05;
			}
	}
	if(strcmp(userIn, "duty+10") ==0){
		if (duty < 0.7) //must be this because due to the division the duty cycle of greater than this will not work
			{
				duty += 0.1;
			}
	
	}
	if(strcmp(userIn, "duty-10") ==0){
		if (duty > 0.3) //must be this because due to the division the duty cycle of greater than this will not work
			{
				duty -= 0.1;
			}
	}
	i=0;
	CLEAR(userIn);
}




void UART0Tx(char const c)
{
	while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
	UART0->DR = c; // Write byte
}


void SysTick_Handler(void)
{
	if(flag){
		SysTick->LOAD = (15999*(1-duty))/kfreq;
		GPIOD->DATA |= 0x40;
	}
	else{
		SysTick->LOAD = (15999*((duty)))/kfreq;
		GPIOD->DATA &= ~0x40;
	}
	flag =!flag;
}