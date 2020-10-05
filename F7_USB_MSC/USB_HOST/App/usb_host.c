/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_audio.h"
#include "usbh_cdc.h"
#include "usbh_msc.h"
#include "usbh_hid.h"
#include "usbh_mtp.h"

/* USER CODE BEGIN Includes */
#include "ff.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

FATFS USBH_fatfs;
FIL MyFile;
FRESULT res;
uint32_t bytesWritten;
uint8_t rtext[200];
uint8_t wtext[] = "Test USB Host on STM32F7";
uint8_t name[10];//name of the file
uint16_t counter=0;
uint32_t i=0;
extern char USBHPath [];  /* USBH logical drive path */
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
void UserFunction(void)
{
	uint16_t bytesread;
	if(Appli_state == APPLICATION_READY)
	{
		sprintf(name,"%d.txt",counter++);
		if(f_open(&MyFile,name,FA_CREATE_ALWAYS |FA_WRITE) != FR_OK )
		{
			/* Creation failed */
			printf("Cannot open %s file \n", name);
		}
		else
		{
			printf("file %s created \n");
			/*write message to the file. Use variable wtext, bytesWritten*/
			res =f_write(&MyFile,wtext,sizeof(wtext),&bytesWritten);

			/*close the file*/
			f_close(&MyFile);

			/*check number of written bytes*/
			if ((bytesWritten == 0) || (res != FR_OK))
			{
				/*error during writing*/
				printf("write error \n");
			}
			else
			{
				/*open file to verification*/
				if (f_open(&MyFile,name,FA_READ) != FR_OK )
				{
					/*file open failure*/
					printf("Cannot open %s file for verify \n");
				}
				else
				{
					/*Read file content. Use variable : rtext, bytesread*/
					res= f_read(&MyFile,rtext,sizeof(rtext), &bytesread);

					if ((bytesread == 0) || (res != FR_OK)) {
						/*read fail*/
						printf("Cannot read file for verification \n");
					}
					else
					{
						/*read success*/
					}

					/*close the file*/
					if (f_close(&MyFile) != FR_OK) {
						/*check number of written bytes*/
						printf("fclose fail \n");
						while (1){}
					}
				}

				/* Compare read data with the expected data */
				if ((bytesread == bytesWritten))
				{
					/*verification success full - number of written bytes is equal to number of read bytes*/
					printf("verification OK - read number of bytes is equal to written number of bytes \n");

				}
				else
				{
					/*verification failed - number of written bytes is not equal to number of read bytes*/
					printf("verify fail \n");
				}
			}
		}
		i++;
	}
}
/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */
  
  /* USER CODE END USB_HOST_Init_PreTreatment */
  
  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_AUDIO_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_CDC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MTP_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
  
  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  printf("USB stick disconnected\n");
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  break;

  case HOST_USER_CONNECTION:
  printf("USB stick connected\n");
  Appli_state = APPLICATION_START;
  if (f_mount(&USBH_fatfs,USBHPath,0) != FR_OK)
  {
	 printf("fmount failed \n");
  }
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
