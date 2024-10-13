// This will work to fully enable and connect the tim2 periferal
#include <stdint.h>
// all of the registers I am using
#define TIM2_BASE (0x40000000)
#define TIM2_CR1 (*(uint16_t*)(TIM2_BASE + 0x00)) //TIM2 control register
#define TIM2_SR (*(uint16_t*)(TIM2_BASE + 0x10)) //TIM2 status register
#define TIM2_EGR (*(uint16_t*)(TIM2_BASE + 0x14)) //TIM2 event generation register

void configureTIM2() {
  TIM2_CR1 &= ~(65535); //clear CR1 register
  TIM2_CR1 |= (1 << 0); //enable clk into TIM2
  TIM2_CR1 |= (0b00 << 8); //multiply SYS clk by 1
  TIM2_CR1 |= (1 << 7); //auto reload enable
  TIM2_CR1 &= ~(1 << 4); //set TIM2 as an upcounter
  TIM2_CR1 &= ~(1 << 1); //requirement for SR interupt flag function
  TIM2_CR1 &= ~(1 << 2); //enable update request source URS

  TIM2_SR &= ~(65535); //clear TIM2 status register
  TIM2_SR |= (1 << 0); //update interupt flag pending
  TIM2_SR |= (1 << 6); //trigger interupt flag enable

  TIM2_EGR &= ~(65535); //clear TIM2 even generation register
  TIM2_EGR |= (1 << 0); //enable update generations
  TIM2_EGR |= (1 << 6); //enable trigger generation for trigger interrupt flag in SR

}