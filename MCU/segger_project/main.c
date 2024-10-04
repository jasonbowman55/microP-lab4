// Jason Bowman
// jbowman@hmc.edu
// 10-2-24


#include "STM32L432KC_RCC.h"
#include <stdint.h>

//TIM16 base register decleration
#define TIM16_BASE (0x40014400UL)

#define TIM16_CCMR1offset (0x16)
#define TIM16_CCMR1 (*(uint32_t *)(TIM16_BASE + TIM16_CCMR1offset)) // --- WHY DOESNT IT LIKE IT WHEN I TAKE OUT THE FIRST *

//TIM16 control register
#define TIM16_CR1offset (0x00)
#define TIM16_CR1 (*(uint32_t*)(TIM16_BASE + TIM16_CR1offset))

//TIM16 prescalar register
#define TIM16_PSCoffset (0x28)
#define TIM16_PSC (*(uint32_t*)(TIM16_BASE + TIM16_PSCoffset))

//TIM16 prescalar register
#define TIM16_PSCoffset (0x28)
#define TIM16_PSC (*(uint32_t*)(TIM16_BASE + TIM16_PSCoffset))

// Pitch in Hz, duration in ms
const int notes[][2] = {
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{416,	125},
{494,	125},
{523,	250},
{  0,	125},
{330,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{523,	125},
{494,	125},
{440,	250},
{  0,	125},
{494,	125},
{523,	125},
{587,	125},
{659,	375},
{392,	125},
{699,	125},
{659,	125},
{587,	375},
{349,	125},
{659,	125},
{587,	125},
{523,	375},
{330,	125},
{587,	125},
{523,	125},
{494,	250},
{  0,	125},
{330,	125},
{659,	125},
{  0,	250},
{659,	125},
{1319,	125},
{  0,	250},
{623,	125},
{659,	125},
{  0,	250},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{416,	125},
{494,	125},
{523,	250},
{  0,	125},
{330,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{523,	125},
{494,	125},
{440,	500},
{  0,	0}};

int main(void) {
//TIM16 prescalar register
  TIM16_PSC |= (0b1110110010111110 << 0);

//CLEAR / enable up counter in TIM16 block
  TIM16_CR1 &= ~(1 << 0);

//enable preload ARR
  TIM16_CR1 |= (1 << 7);

//set ARR based on a frequency devision

// setting main DTG block output as TIM16ch1
  

//set SYSCLK as PLL

  //set SW as 11
  RCC->CFGR |= (0b11 << 0);

  //channel-mode 1 PWM output
  TIM16_CCMR1 |= (0b0110 << 4);
	
}