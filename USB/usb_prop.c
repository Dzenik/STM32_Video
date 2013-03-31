/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V3.1.0RC1
* Date               : 09/28/2009
* Description        : All processing related to Virtual Com Port Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Device_Status_In 		NOP_Process
#define Device_Status_Out		NOP_Process

#define Device_GetConfiguration			NOP_Process
#define Device_SetConfiguration			NOP_Process
#define Device_GetInterface				NOP_Process
#define Device_SetInterface				NOP_Process
#define Device_GetStatus				NOP_Process
#define Device_ClearFeature				NOP_Process
#define Device_SetEndPointFeature		NOP_Process
#define Device_SetDeviceFeature			NOP_Process
#define Device_SetDeviceAddress			NOP_Process
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t wIstr;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    Device_init,
    Device_Reset,
    Device_Status_In,
    Device_Status_Out,
    Device_Data_Setup,
    Device_NoData_Setup,
    Device_Get_Interface_Setting,
    Device_GetDeviceDescriptor,
    Device_GetConfigDescriptor,
    Device_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Device_GetConfiguration,
    Device_SetConfiguration,
    Device_GetInterface,
    Device_SetInterface,
    Device_GetStatus,
    Device_ClearFeature,
    Device_SetEndPointFeature,
    Device_SetDeviceFeature,
    Device_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)USB_DeviceDescriptor,
    SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)USB_ConfigDescriptor,
    SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)USB_StringLangID, SIZ_STRING_LANGID},
    {(uint8_t*)USB_StringVendor, SIZ_STRING_VENDOR},
    {(uint8_t*)USB_StringProduct, SIZ_STRING_PRODUCT},
    {(uint8_t*)USB_StringSerial, SIZ_STRING_SERIAL}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Device_init()
{
     DEVICE_INFO *pInfo = &Device_Info;
     pInfo->Current_Configuration = 0;

     _SetCNTR(CNTR_FRES);		//Reset USB block
     _SetCNTR(0);				//Deassert reset signal
     _SetISTR(0);				//Clear pending interrupts
     USB_SIL_Init();
     GPIO_ResetBits(GPIOC, GPIO_Pin_13); //Enable pull-up
}

void Device_Reset()
{
     //Set device as not configured
      pInformation->Current_Configuration = 0;
      pInformation->Current_Interface = 0;		//the default Interface
      /* Current Feature initialization */
      pInformation->Current_Feature = USB_ConfigDescriptor[7];

      SetBTABLE(BTABLE_ADDRESS);

      /* Initialize Endpoint 0 */
      SetEPType(ENDP0, EP_CONTROL);
      SetEPTxStatus(ENDP0, EP_TX_STALL);
      SetEPRxAddr(ENDP0, ENDP0_RXADDR);
      SetEPTxAddr(ENDP0, ENDP0_TXADDR);
      Clear_Status_Out(ENDP0);
      SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
      SetEPRxValid(ENDP0);

      SetEPType(ENDP1, EP_BULK);
      SetEPRxAddr(ENDP1, ENDP1_RXADDR);
      SetEPRxCount(ENDP1, 0x40);
      SetEPRxStatus(ENDP1, EP_RX_VALID);
      SetEPTxStatus(ENDP1, EP_TX_DIS);

      /* Set this device to response on default address */
      SetDeviceAddress(0);
}

void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Set_USBClock()
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
  CTR_HP();
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  wIstr = _GetISTR();
    #if (IMR_MSK & ISTR_CTR)
      if (wIstr & ISTR_CTR & wInterrupt_Mask)
      {
        CTR_LP();
      }
    #endif
    #if (IMR_MSK & ISTR_RESET)
      if (wIstr & ISTR_RESET & wInterrupt_Mask)
      {
        _SetISTR((uint16_t)CLR_RESET);
        Device_Property.Reset();
      }
    #endif
}

///*******************************************************************************
//* Function Name  : Virtual_Com_Port_init.
//* Description    : Virtual COM Port Mouse init routine.
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//void Device_init(void)
//{
//
//  /* Update the serial number string descriptor with the data from the unique
//  ID*/
//  Get_SerialNum();
//
//  pInformation->Current_Configuration = 0;
//
//  /* Connect the device */
//  PowerOn();
//
//  /* Perform basic device initialization operations */
//  USB_SIL_Init();
//
//  /* configure the USART to the default settings */
//  USART_Config_Default();
//
//  bDeviceState = UNCONNECTED;
//}
//
///*******************************************************************************
//* Function Name  : Virtual_Com_Port_Reset
//* Description    : Virtual_Com_Port Mouse reset routine
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//void Device_Reset(void)
//{
//  /* Set Virtual_Com_Port DEVICE as not configured */
//  pInformation->Current_Configuration = 0;
//
//  /* Current Feature initialization */
//  pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];
//
//  /* Set Virtual_Com_Port DEVICE with the default Interface*/
//  pInformation->Current_Interface = 0;
//
//#ifdef STM32F10X_CL
//  /* EP0 is already configured by USB_SIL_Init() function */
//
//  /* Init EP1 IN as Bulk endpoint */
//  OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, VIRTUAL_COM_PORT_DATA_SIZE);
//
//  /* Init EP2 IN as Interrupt endpoint */
//  OTG_DEV_EP_Init(EP2_IN, OTG_DEV_EP_TYPE_INT, VIRTUAL_COM_PORT_INT_SIZE);
//
//  /* Init EP3 OUT as Bulk endpoint */
//  OTG_DEV_EP_Init(EP3_OUT, OTG_DEV_EP_TYPE_BULK, VIRTUAL_COM_PORT_DATA_SIZE);
//#else
//
//  SetBTABLE(BTABLE_ADDRESS);
//
//  /* Initialize Endpoint 0 */
//  SetEPType(ENDP0, EP_CONTROL);
//  SetEPTxStatus(ENDP0, EP_TX_STALL);
//  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
//  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
//  Clear_Status_Out(ENDP0);
//  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
//  SetEPRxValid(ENDP0);
//
//  /* Initialize Endpoint 1 */
//  SetEPType(ENDP1, EP_BULK);
//  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
//  SetEPTxStatus(ENDP1, EP_TX_NAK);
//  SetEPRxStatus(ENDP1, EP_RX_DIS);
//
//  /* Initialize Endpoint 2 */
//  SetEPType(ENDP2, EP_INTERRUPT);
//  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
//  SetEPRxStatus(ENDP2, EP_RX_DIS);
//  SetEPTxStatus(ENDP2, EP_TX_NAK);
//
//  /* Initialize Endpoint 3 */
//  SetEPType(ENDP3, EP_BULK);
//  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
//  SetEPRxCount(ENDP3, VIRTUAL_COM_PORT_DATA_SIZE);
//  SetEPRxStatus(ENDP3, EP_RX_VALID);
//  SetEPTxStatus(ENDP3, EP_TX_DIS);
//
//  /* Set this device to response on default address */
//  SetDeviceAddress(0);
//#endif /* STM32F10X_CL */
//
//  bDeviceState = ATTACHED;
//}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetConfiguration.
* Description    : Udpade the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//void Device_SetConfiguration(void)
//{
//  DEVICE_INFO *pInfo = &Device_Info;
//
//  if (pInfo->Current_Configuration != 0)
//  {
//    /* Device configured */
//    bDeviceState = CONFIGURED;
//  }
//}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//void Device_SetDeviceAddress (void)
//{
//  bDeviceState = ADDRESSED;
//}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//void Device_Status_In(void)
//{
//  if (Request == SET_LINE_CODING)
//  {
//    USART_Config();
//    Request = 0;
//  }
//}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Device_Status_Out(void)
{}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Device_Data_Setup(uint8_t RequestNo)
{
//  uint8_t    *(*CopyRoutine)(uint16_t);
//
//  CopyRoutine = NULL;
//
//  if (RequestNo == GET_LINE_CODING)
//  {
//    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
//    {
//      CopyRoutine = Virtual_Com_Port_GetLineCoding;
//    }
//  }
//  else if (RequestNo == SET_LINE_CODING)
//  {
//    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
//    {
//      CopyRoutine = Virtual_Com_Port_SetLineCoding;
//    }
//    Request = SET_LINE_CODING;
//  }
//
//  if (CopyRoutine == NULL)
//  {
    return USB_UNSUPPORT;
//  }
//
//  pInformation->Ctrl_Info.CopyData = CopyRoutine;
//  pInformation->Ctrl_Info.Usb_wOffset = 0;
//  (*CopyRoutine)(0);
//  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Device_NoData_Setup(uint8_t RequestNo)
{
//
//  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
//  {
//    if (RequestNo == SET_COMM_FEATURE)
//    {
//      return USB_SUCCESS;
//    }
//    else if (RequestNo == SET_CONTROL_LINE_STATE)
//    {
//      return USB_SUCCESS;
//    }
//  }
//
  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *Device_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *Device_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Device_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Device_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Inecoding structure base address.
*******************************************************************************/
//uint8_t *Device_GetLineCoding(uint16_t Length)
//{
//  if (Length == 0)
//  {
//    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
//    return NULL;
//  }
//  return(uint8_t *)&linecoding;
//}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
//uint8_t *Device_SetLineCoding(uint16_t Length)
//{
//  if (Length == 0)
//  {
//    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
//    return NULL;
//  }
//  return(uint8_t *)&linecoding;
//}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

