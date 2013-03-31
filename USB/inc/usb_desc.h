/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Version            : V3.2.1
* Date               : 07/05/2010
* Description        : Descriptor Header for Mass Storage Device
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define SIZ_DEVICE_DESC		0x12
#define SIZ_CONFIG_DESC		0x19
#define SIZ_STRING_LANGID	0x04
#define SIZ_STRING_VENDOR	0x10
#define SIZ_STRING_PRODUCT	0x10
#define SIZ_STRING_SERIAL	0x12

/* Exported functions ------------------------------------------------------- */
extern const uint8_t USB_DeviceDescriptor[];
extern const uint8_t USB_ConfigDescriptor[];
extern const uint8_t USB_StringDescriptor[];

extern const uint8_t USB_StringLangID[SIZ_STRING_LANGID];
extern const uint8_t USB_StringVendor[SIZ_STRING_VENDOR];
extern const uint8_t USB_StringProduct[SIZ_STRING_PRODUCT];
extern const uint8_t USB_StringSerial[SIZ_STRING_SERIAL];


#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/


