#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

/* User Defines */
#define LED2 PA5

void Delay(uint32_t nTime);

int main(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	if (SysTick_Config(SystemCoreClock/1000))
		while(1);

	while(1){
		static int ledval = 0;
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, (ledval) ? Bit_SET : Bit_RESET);
		ledval = 1 - ledval;
		Delay(250);
}
}
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime){
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void SysTick_Handler(void){
	if (TimingDelay != 0x00)
		TimingDelay--;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
	/* Infinite loop */
	/* STDlib parm checking hook, use GDB*/
	while(1);
}
#endif
