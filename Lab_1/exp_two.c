#include "TM4C123GH6PM.h"
void delayMs(int n);
int changeColor(int color);
void blink(int color, int ison);
void checkBlink(int check);
int main(void)
{
	int curr = 0;
	SYSCTL->RCGCGPIO |= 0x20;
		GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR = 0b1;
	GPIOF->DIR = 0x0e;
	GPIOF->DEN = 0x1f;
	GPIOF->PUR = 0b10001;

	int useCurr = 0;
	int isOn = 0;
	while(1)
	{	
			int s1 = 0;
			int s2 = 0;
			for(int i = 0; i<2; i++){
				if((GPIOF->DATA & 0b10000) == 0){
					s1 = 1;
					blink(useCurr, isOn);
				}
				else{
					s1 = 0;
					break;
				}
			}
			for(int i = 0; i<2; i++){
				if((GPIOF->DATA & 0b00001) == 0){
					s2 = 1;
					blink(useCurr, isOn);
				}
				else{
					s2 = 0;
					break;
				}
			}
			if (s1){
				useCurr = changeColor(useCurr);
			}
			else{
				changeColor(useCurr);
			}
			checkBlink(isOn);
			if (s2){
				isOn = isOn?0:1;
			}
	}
}
void blink(color, isOn){
	delayMs(500);
	changeColor(color);
  delayMs(500);
	if(!isOn){
		GPIOF->DATA = 0;	
	}
}

void checkBlink(int isOn){
			delayMs(500);
			if(!isOn){
					GPIOF->DATA = 0;	
			}
			delayMs(500);
}
int changeColor(int color){
	if (color == 0){
		GPIOF->DATA = 0x08;
		return 1;
	}
	else if (color == 1){
		GPIOF->DATA = 0x02;
		return 2;
	}
	else{
		GPIOF->DATA = 0x04;
		return 0;
	}
}
void delayMs(int n)
{
	for(int i =0; i< n; i++){
		SysTick->LOAD= 15999;
		SysTick->CTRL = 0x5;
		while((SysTick->CTRL & 0x10000) == 0) 
		{ }
		SysTick->CTRL = 0;
	}
}