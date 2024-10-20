// Jason Bowman
// jbowman@hmc.edu
// 10-2-24


#include "STM32L432KC_RCC.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_GPIO.h"
#include <stdint.h>

// TIM16
#define TIM16_BASE (0x40014400) // TIM16 BASE register
#define TIM16_CR1 (*(volatile uint16_t*)(TIM16_BASE + 0x00)) // control register
#define TIM16_CCMR1 (*(volatile uint32_t*)(TIM16_BASE + 0x18)) // capture compare MODE regiter
#define TIM16_CCER (*(volatile uint16_t*)(TIM16_BASE + 0x20)) // capture compare enable register
#define TIM16_PSC (*(volatile uint16_t*)(TIM16_BASE + 0x28)) // prescalar register
#define TIM16_ARR (*(volatile uint16_t*)(TIM16_BASE + 0x2C)) // auto reload register
#define TIM16_EGR (*(volatile uint16_t*)(TIM16_BASE + 0x14)) // event generation register
#define TIM16_CCR1 (*(volatile uint16_t*)(TIM16_BASE + 0x34)) // capture compare register
#define TIM16_BDTR (*(volatile uint32_t*)(TIM16_BASE + 0x44)) // break & deadtime state register

// GPIO
#define GPIOA_BASE (0x48000000)
#define GPIOA_MODER (*(volatile uint32_t*)(GPIOA_BASE + 0x00)) // GPIO mode register
#define GPIOA_OSPEEDR (*(volatile uint32_t*)(GPIOA_BASE + 0x08)) // GPIO register speed select
#define GPIOA_AFRL (*(volatile uint32_t*)(GPIOA_BASE + 0x20)) // GPIO alternate function control register

// Access RCC rubregisters via .h file

// Pitch in Hz, duration in ms
//const int notes[][2] = {
//{659,	125},
//{623,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{494,	125},
//{587,	125},
//{523,	125},
//{440,	250},
//{  0,	125},
//{262,	125},
//{330,	125},
//{440,	125},
//{494,	250},
//{  0,	125},
//{330,	125},
//{416,	125},
//{494,	125},
//{523,	250},
//{  0,	125},
//{330,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{494,	125},
//{587,	125},
//{523,	125},
//{440,	250},
//{  0,	125},
//{262,	125},
//{330,	125},
//{440,	125},
//{494,	250},
//{  0,	125},
//{330,	125},
//{523,	125},
//{494,	125},
//{440,	250},
//{  0,	125},
//{494,	125},
//{523,	125},
//{587,	125},
//{659,	375},
//{392,	125},
//{699,	125},
//{659,	125},
//{587,	375},
//{349,	125},
//{659,	125},
//{587,	125},
//{523,	375},
//{330,	125},
//{587,	125},
//{523,	125},
//{494,	250},
//{  0,	125},
//{330,	125},
//{659,	125},
//{  0,	250},
//{659,	125},
//{1319,	125},
//{  0,	250},
//{623,	125},
//{659,	125},
//{  0,	250},
//{623,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{494,	125},
//{587,	125},
//{523,	125},
//{440,	250},
//{  0,	125},
//{262,	125},
//{330,	125},
//{440,	125},
//{494,	250},
//{  0,	125},
//{330,	125},
//{416,	125},
//{494,	125},
//{523,	250},
//{  0,	125},
//{330,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{623,	125},
//{659,	125},
//{494,	125},
//{587,	125},
//{523,	125},
//{440,	250},
//{  0,	125},
//{262,	125},
//{330,	125},
//{440,	125},
//{494,	250},
//{  0,	125},
//{330,	125},
//{523,	125},
//{494,	125},
//{440,	500},
//{  0,	0}};

const int notes[][2] = {
    {392, 250},  // G
    {392, 250},  // G
    {440, 500},  // A
    {392, 500},  // G
    {523, 500},  // C
    {494, 1000}, // B

    {392, 250},  // G
    {392, 250},  // G
    {440, 500},  // A
    {392, 500},  // G
    {587, 500},  // D
    {523, 1000}, // C

    {392, 250},  // G
    {392, 250},  // G
    {659, 500},  // E
    {587, 500},  // D
    {523, 500},  // C
    {494, 1000}, // B

    {440, 250},  // A
    {440, 250},  // A
    {494, 500},  // B
    {523, 500},  // C
    {587, 500},  // D
    {523, 1000}, // C

    {0, 0}       // End of song
};



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
  TIM16_PSC |= (255); // set prescale such that max period length corresponds to lowest freq for 1 period (80MHz / (660Hz*2))

  TIM16_BDTR |= (1 << 15); // en Main Output Enable (MOE) in Dead Time Register (DTR)

  TIM16_CCER |= (1 << 0); // requirement for MOE to effectively output 
}

/////////////////////////////////////////////////////////////////////////////////////

// Function that converts frequency input into 
void FREQset(int freq) { // take a desired input frequency "freq"
  int ARRnew = (31250/freq);
  TIM16_ARR &= ~(65535);
  TIM16_ARR |= ARRnew;
  TIM16_CCR1 &= ~(65535);
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
  configureFlash();
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