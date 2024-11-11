/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEFINES_H
#define __DEFINES_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Macro Defines -------------------------------------------------------------*/
#define BUTTON_PIN			(GPIO_Pin_0)
#define BUTTON_GPIO			(GPIOA)

#define LED_ALL_PINS		(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
#define LED_ALL_GPIO		(GPIOD)

#define LED_GREEN_PIN		(GPIO_Pin_12)
#define LED_GREEN_GPIO	(GPIOD)

#define LED_ORANGE_PIN	(GPIO_Pin_13)
#define LED_ORANGE_GPIO	(GPIOD)

#define LED_RED_PIN			(GPIO_Pin_14)
#define LED_RED_GPIO		(GPIOD)

#define LED_BLUE_PIN		(GPIO_Pin_15)
#define LED_BLUE_GPIO		(GPIOD)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif /* __DEFINES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
