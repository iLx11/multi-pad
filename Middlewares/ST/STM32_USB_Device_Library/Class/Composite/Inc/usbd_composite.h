/**
  ******************************************************************************
  * @file    usbd_composite.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_composite.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_COMPOSITE_H
#define __USB_COMPOSITE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_COMPOSITE
  * @brief This file is the header file for usbd_composite_core.c
  * @{
  */


/** @defgroup USBD_COMPOSITE_Exported_Defines
  * @{
  */
#define USBD_IAD_DESC_SIZE                        0x08
#define USBD_IAD_DESCRIPTOR_TYPE                  0x0B

#define COMPOSITE_EPIN_ADDR                 0x81
#define COMPOSITE_EPIN_SIZE                 0x10

#define USB_COMPOSITE_CONFIG_DESC_SIZ       107

/**
  * @}
  */
enum
{
  USBD_INTERFACE_CDC_CMD = 0X00,
  USBD_INTERFACE_CDC,
  USBD_INTERFACE_HID,
  USBD_INTERFACE_NUM
};

extern void *pClassDataCDC;
extern void *pClassDataHID;
/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef  USBD_COMPOSITE_ClassDriver;
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_COMPOSITE_CORE_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
