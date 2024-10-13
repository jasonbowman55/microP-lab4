// Jason Bowman
// jbowman@hmc.edu
// 10-2-24


#include "STM32L432KC_RCC.h"
#include <stdint.h>

// TIM16
#define TIM16_BASE (0x40014400) // TIM16 BASE register
#define TIM16_CR1 (*(uint16_t*)(TIM16_BASE + 0x00)) // control register
#define TIM16_CCMR1 (*(uint32_t *)(TIM16_BASE + 0x18)) // capture compare MODE regiter
#define TIM16_CCER (*(uint16_t*)(TIM16_BASE + 0x20)) // capture compare enable register
#define TIM16_PSC (*(uint16_t*)(TIM16_BASE + 0x28)) // prescalar register
#define TIM16_ARR (*(uint16_t*)(TIM16_BASE + 0x2C)) // auto reload register
#define TIM16_EGR (*(uint16_t*)(TIM16_BASE + 0x14)) // event generation register
#define TIM16_CCR1 (*(uint16_t*)(TIM16_BASE + 0x34)) // capture compare register
#define TIM16_BDTR (*(uint32_t*)(TIM16_BASE + 0x44)) // break & deadtime state register

// GPIO
#define GPIOA_BASE (0x48000000)
#define GPIOA_MODER (*(uint32_t*)(GPIOA_BASE + 0x00)) // GPIO mode register
#define GPIOA_OSPEEDR (*(uint32_t*)(GPIOA_BASE + 0x08)) // GPIO register speed select
#define GPIOA_AFRL (*(uint32_t*)(GPIOA_BASE + 0x20)) // GPIO alternate function control register

// Access RCC rubregisters via .h file

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


// DELAY
void delay(int ms) { // take input ms from any delay calls
  while (ms-- > 0) { // while ms > 0, continue. decreament each ms, once ms=0, leave loop
    volatile int x=887; // x clk cycles = 1ms
    while (x-- > 0) // while x > 0, continue. decreament x each clk. once x=0, leave loop
      __asm__("NOP"); // nothing happens when decreamenting 0
  }
}
////////

// We must enable the GPIO path and TIM16 in main so that our path is redefined everytime we reset the system in between each played note

void GPIOinit() {
  RCC->AHB2ENR |= (1 << 0); // follow clk tree, en GPIOA clk input from SYSCLK (PLL)

  GPIOA_AFRL &= ~(0b1111 << 24); // clear AF14 in GPIO alternate function
  GPIOA_AFRL |= (0b1110 << 24); // set GPIO connect to AF14

  GPIOA_OSPEEDR |= (0b11 << 12); // set pin PA6 as q fast output

  GPIOA_MODER &= ~(0b11 << 12); // reset pin PA6 MODER
  GPIOA_MODER |= (0b10 << 12); // se pin PA6 to alternate function
}

void TIMinit() {
  RCC->APB2ENR |= (1 << 17); // enable peripheral clk to be TIM16

  TIM16_CCMR1 &= ~(0b111 << 4); // clear OC1M PWM output mode
  TIM16_CCMR1 |= (0b110 << 4); // set ch1 to PWM out

  TIM16_CCMR1 |= (1 << 3); // enable preload register

  TIM16_CR1 |= (1 << 7); // enable auto-load register as "buffered"

  TIM16_EGR |= (1 << 0); // counter block enabled

  TIM16_CCR1 |= (32767); // set duty cycle to be half the limit of the counter (50%ds)

  TIM16_PSC &= ~(65535); // clear PSC
  TIM16_PSC |= (60606); // set prescale such that max period length corresponds to lowest freq for 1 period (80MHz / (660Hz*2))

  TIM16_BDTR |= (1 << 15); // en Main Output Enable (MOE) in Dead Time Register (DTR)

  TIM16_CCER |= (1 << 0); // requirement for MOE to effectively output 
}

/////////////////////////////////////////////////////////////////////////////////////

// Function that converts frequency input into 
void FREQset(int freq) { // take a desired input frequency "freq"
  int ARRnew = (1320/freq);
  TIM16_ARR &= ~(65535);
  TIM16_ARR |= ARRnew;
  TIM16_CCR1 |= (ARRnew/2);
}
///////////////////////////

// enable counter block in TIM16 function
void CNTen() {
  TIM16_CR1 |= (1 << 0);
}
////////////////////////

// disable counter block in TIM16 function
void CNTdisable() {
  TIM16_CR1 &= ~(1 << 0);
}
////////////////////////

// play notes as long as there is a frequency input > 0
void playNote(int freq, int time) {
  if (freq != 0) {
    FREQset(freq);
    CNTen();
    delay(time);
    CNTdisable();
  } else {
    delay(time);
  }
}

int main(void) {
  configureClock();
  GPIOinit();
  TIMinit();

  delay(1000);

  for (int i = 0; i < (sizeof(notes) / sizeof(notes[0])); i++) {
    playNote(notes[i][0], notes[i][1]);
  }

  CNTen();
  CNTdisable();
  
}