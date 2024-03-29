#include "TM4C123GH6PM.h"
void delayFreq(int n);

int main(void)
{
	SYSCTL->RCGCGPIO |= 0x28;
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR = 0b1;
	GPIOF->DIR = 0x0e;
	GPIOF->DEN = 0x1f;
	GPIOF->PUR = 0b10001;

	GPIOD->DIR = 0b01000000;
	GPIOD->DEN = 0b01000000;
	GPIOD->PUR = 0b0;
	float duty = 0.5;
	int flag = 0;
	int flag2 = 0;
	while (1)
	{
		if ((GPIOF->DATA & 0b10000) == 0)
		{
			if (duty < 1 && !flag)
			{
				flag =1;
				duty += 0.05;
			}
		}
		else{
			flag =0;
		}
		if ((GPIOF->DATA & 0b00001) == 0)
		{
			if (duty > 0 && !flag2)
			{
				duty -= 0.05;
				flag2 =1;
			}
		}
		else{
			flag2 =0;
		}
		delayFreq(30 * duty);
		GPIOD->DATA |= 0x40;
		delayFreq(30 * (1 - duty));
		GPIOD->DATA &= ~0x40;
	}
}

// delay in milliseconds (16 MHz CPU clock)
void delayFreq(int n) // n is in khz
{
		SysTick->LOAD = 15999/n;
		SysTick->CTRL = 0x5;
		while ((SysTick->CTRL & 0x10000) == 0)
		{
		}
		SysTick->CTRL = 0;
}