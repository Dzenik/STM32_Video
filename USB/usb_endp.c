/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.1.0RC1
* Date               : 09/28/2009
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "usb_lib.h"
//#include "usb_desc.h"
//#include "usb_mem.h"
//#include "hw_config.h"
#include "ssd1289.h"
#include "usb_istr.h"
#include "usb_conf.h"
#include "usb_regs.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buffer[VIRTUAL_COM_PORT_DATA_SIZE];
__IO uint32_t count_out = 0;
uint32_t count_in = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//void EP1_IN_Callback(void)
//{
//  count_in = 0;
//}
void EP1_OUT_Callback(void)
{
    uint16_t dataLen = GetEPRxCount(EP1_OUT & 0x7F);
    uint16_t offset=0;
    if(GraphicsState == NOT_ADDRESSED)
    {
        if(dataLen<=8)
        {
            SetEPRxStatus(ENDP1, EP_RX_VALID);
            return;
        }
        PMAToUserBufferCopy(buffer, GetEPRxAddr(EP1_OUT & 0x7F), 8);
        uint16_t horz = *((uint16_t*)(buffer));
        uint16_t vert = *(uint16_t*)(buffer+2);
        dataTotal = *(uint32_t*)(buffer+4);
        LCD_WriteReg(LCD_DIR_HORIZONTAL,vert); //экран повернут
        LCD_WriteReg(LCD_DIR_VERTICAL,horz);
        offset=16;
        dataTransfered = 0x00;
        GraphicsState = TRANSFER_IN_PROGRESS;
        dataLen-=8;
    }

    LCD_WriteRAM_Prepare;
    PMAToLCDBufferCopy(GetEPRxAddr(EP1_OUT & 0x7F), offset, dataLen);
    dataTransfered+=(dataLen)>>1;
    if(dataTransfered >= dataTotal)
        GraphicsState = NOT_ADDRESSED;
    SetEPRxStatus(ENDP1, EP_RX_VALID);
}
/*******************************************************************************
* Function Name  : EP3_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  /* Get the received data buffer and update the counter */
//  count_out = USB_SIL_Read(EP3_OUT, buffer_out);
    
#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

