// This will work to fully enable and connect the tim2 periferal

// all of the registers I am using
#define TIM2_BASE (0x40000000)
#define TIM2_CR1 (*(uint32_t*)(TIM2_BASE + 0x00))

void configureTIM2() {
  TIM2_CR1 = |= (1 << 0); //enable clk into TIM2


}