#include "Board/board.h"
#include "hw_config.h"
//#include "usb_lib.h"
#include "usb_pwr.h"


volatile uint32_t Tick = 0x00000000;
void SysTick_Handler()
{
    if(Tick>0)
        Tick--;
}

void SysTickDelay(uint32_t msDelay)
{
    Tick=msDelay;
    while(Tick);
}

int main(void)
{
    Set_System();
	/* Set the Vector Table base location at 0x08000000 */
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

    LCD_Setup();
	LCD_Clear(0x0000);

	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	Device_init();
	Device_Reset();
	while(bDeviceState != CONFIGURED);



	GPIO_InitTypeDef GPIO_InitStructure;

	// запускаем тактирование GPIO порта F
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

	// Устанавливаем пины PF6, PF7, PF8, PF9 как выходные
	// использовать будем PF6 и PF7, но уж проинициализируем все четыре LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
	                | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);


    while(1)
    {
    	GPIO_SetBits(GPIOF, GPIO_Pin_6);
    	SysTickDelay(500);
    	GPIO_ResetBits(GPIOF, GPIO_Pin_6);
    	SysTickDelay(500);
    }
}
