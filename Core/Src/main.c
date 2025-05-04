/* USER CODE BEGIN Header */
/**
  ******************************************************************************
* @file         : main.c
*@brief         : Main program body
  
  ******************************************************************************
* @attention
*
*Copyright (c) 2024 STMicroelectronics.
* All rights reserved.
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software, it is provided AS-IS.
*
 ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern I2C_HandleTypeDef hi2c1;

#include "fonts.h"
#include "ssd1306.h"
#include <stdio.h>
#include <stdarg.h> //for va_list var arg functions
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
float temperature;
char string[64];
void myprintf(const char *fmt, ...);
uint16_t gas=0;
char msg[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void myprintf(const char *fmt, ...) {
  static char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
  HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, -1);

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);

#define DHT22_PORT GPIOB
#define DHT22_PIN GPIO_PIN_14

uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
uint32_t pMillis, cMillis;
float temp_Celsius = 0;
float Humidity = 0;

uint8_t hum_integral, hum_decimal, tempC_integral, tempC_decimal;
char string[15];

void microDelay (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

uint8_t DHT22_Start (void)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT22_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 0);
  microDelay (1300);
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1);
  microDelay (30);
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  microDelay (40);
  if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))
  {
    microDelay (80);
    if ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) Response = 1;
  }
  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

uint8_t DHT22_Read (void)
{
  uint8_t x,y;
  for (x=0;x<8;x++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {
      cMillis = HAL_GetTick();
    }
    microDelay (40);
    if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))   // if the pin is low
      y&= ~(1<<(7-x));
    else
      y|= (1<<(7-x));
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go low
      cMillis = HAL_GetTick();
    }
  }
  return y;
}
  uint8_t res = SSD1306_Init();

  uint8_t rxBuffer[512] = {0};
          uint8_t ATisOK;
          char ATcommand[512];
          char ATcommandB[1024];
          char ATcommandT[512];
          char tempStr[512];
		 char humStr[512];
		 char gasLStr[512];
		 char gasStr[512];


          sprintf(ATcommandB, "<!DOCTYPE html><html>\n<head>\n\
                  <meta charset=\"UTF-8\">\n\
                  <title>Environmental Monitoring</title>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
                  <style>\n\
                  body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; margin: 0; padding: 0; }\n\
                  header { background-color: #0066cc; color: white; padding: 20px 10px; }\n\
                  h1 { margin: 0; font-size: 26px; }\n\
                  .info-card { background: white; border-radius: 10px; margin: 20px auto; padding: 20px; \n\
                  box-shadow: 0 4px 8px rgba(0,0,0,0.1); width: 90%%; max-width: 400px; }\n\
                  .reading { font-size: 22px; color: #333; margin: 10px 0; }\n\
                  .status { font-size: 18px; color: #888; }\n\
                  footer { margin-top: 30px; color: #666; }\n\
                  </style>\n</head>\n<body>\n\
                  <header><h1>Environmental Monitoring System</h1></header>\n\
                  <div class=\"info-card\">\n");

          sprintf(ATcommandT,
          "</div>\n\
          <footer style=\"background-color: #0066cc; color: white; padding: 15px 10px; font-size: 14px; margin-top: 40px;\">\n\
          <p>&copy; 2025 Environmental Monitoring Project<br>Developed by Nwofor Amuche Prince & Ugama Andrew Ebili</p>\n\
          </footer>\n\
          </body></html>");

//       Establishing connection between esp8266 and the client
          sprintf(ATcommand,"AT+RST\r\n");
          memset(rxBuffer,0,sizeof(rxBuffer));
          HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
          HAL_UART_Receive (&huart1, rxBuffer, 512, 100);
          HAL_Delay(500);

          ATisOK = 0;
          while(!ATisOK){
            sprintf(ATcommand,"AT+CWMODE_CUR=2\r\n");
              memset(rxBuffer,0,sizeof(rxBuffer));
              HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
              HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
            if(strstr((char *)rxBuffer,"OK")){
              ATisOK = 1;
            }
            HAL_Delay(500);
          }
// Setting the WIFI details
          ATisOK = 0;
          while(!ATisOK){
            sprintf(ATcommand,"AT+CWSAP_CUR=\"IoT-SYSTEM\",\"20252025\",1,3,4,0\r\n");
              memset(rxBuffer,0,sizeof(rxBuffer));
              HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
              HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
            if(strstr((char *)rxBuffer,"OK")){
              ATisOK = 1;
            }
            HAL_Delay(500);
          }

          ATisOK = 0;
          while(!ATisOK){
            sprintf(ATcommand,"AT+CIPAP_CUR=\"192.168.72.1\"\r\n");
            memset(rxBuffer,0,sizeof(rxBuffer));
            HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
            HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
            if(strstr((char *)rxBuffer,"OK")){
              ATisOK = 1;
            }
            HAL_Delay(500);
          }

          ATisOK = 0;
          while(!ATisOK){
            sprintf(ATcommand,"AT+CIPMUX=1\r\n");
              memset(rxBuffer,0,sizeof(rxBuffer));
              HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
              HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
              if(strstr((char *)rxBuffer,"OK")){
                ATisOK = 1;
              }
              HAL_Delay(500);
          }

          ATisOK = 0;
          while(!ATisOK){
            sprintf(ATcommand,"AT+CIPSERVER=1,80\r\n");
            memset(rxBuffer,0,sizeof(rxBuffer));
            HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
            HAL_UART_Receive (&huart1, rxBuffer, 512, 1000);
            if(strstr((char *)rxBuffer,"OK")){
                ATisOK = 1;
            }
            HAL_Delay(500);
          }
		RTC_DateTypeDef sDate;
		RTC_TimeTypeDef sTime;

		myprintf("\r\n~ SD card Project ~\r\n\r\n");

			HAL_Delay(1000); // Let the SD card settle

			// FatFs variables
			FATFS FatFs;   // Filesystem handle
			FRESULT fres;  // Result after operations

			// Mount the filesystem
			fres = f_mount(&FatFs, "", 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	  FIL fil;
	  UINT bytesWrote;
	  BYTE writeBuf[100];
	  FRESULT fres;

	  if(DHT22_Start())
	  	     {
	  	       hum1 = DHT22_Read();
	  	       hum2 = DHT22_Read();
	  	       tempC1 = DHT22_Read();
	  	       tempC2 = DHT22_Read();
	  	       SUM = DHT22_Read();
	  	       CHECK = hum1 + hum2 + tempC1 + tempC2;
	  	       if (CHECK == SUM)
	  	       {
	  	         if (tempC1>127)
	  	         {
	  	           temp_Celsius = (float)tempC2/10*(-1);
	  	         }
	  	         else
	  	         {
	  	           temp_Celsius = (float)((tempC1<<8)|tempC2)/10;
	  	         }

	  	         Humidity = (float) ((hum1<<8)|hum2)/10;

	  	         SSD1306_GotoXY (0, 0);
	  	         hum_integral = Humidity;
	  	         hum_decimal = Humidity*10-hum_integral*10;
	  	         sprintf(string,"Hum:%d.%d %% ", hum_integral, hum_decimal);
	  	         SSD1306_Puts (string, &Font_11x18, 1);

	  	         SSD1306_GotoXY (0, 20);
	  	         if (temp_Celsius < 0)
	  	         {
	  	           tempC_integral = temp_Celsius *(-1);
	  	           tempC_decimal = temp_Celsius*(-10)-tempC_integral*10;
	  	           sprintf(string,"Temp:-%d.%d C", tempC_integral, tempC_decimal);
	  	         }
	  	         else
	  	         {
	  	           tempC_integral = temp_Celsius;
	  	           tempC_decimal = temp_Celsius*10-tempC_integral*10;
	  	           sprintf(string,"Temp:%d.%d C", tempC_integral, tempC_decimal);
	  	         }
	  	         SSD1306_Puts (string, &Font_11x18, 1);

	  	         SSD1306_GotoXY (0, 40);



	  	         // Gas Reading code
	  	         HAL_ADC_Start(&hadc1);
	  	         HAL_ADC_PollForConversion(&hadc1, 20);
	  	         gas = HAL_ADC_GetValue(&hadc1);

	  	       // If gas value is less than 1010,  turn off the alarm systems and Display the readings
	  	         if (gas <= 1010)
					 {
						 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Turn off LED
						 sprintf(string,"Normal Air Quality");
						 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Turrn off buzzer
						// Display the reading on the Oled
						 SSD1306_Puts (string, &Font_7x10, 1);
						 SSD1306_UpdateScreen();
						 //Display the reading on a webpage
						 sprintf(tempStr, "<div class=\"reading\">Temperature: <strong>%d.%d &deg;C</strong></div>\n",tempC_integral, tempC_decimal);
						 sprintf(humStr, "<div class=\"reading\">Humidity: <strong>%d.%d %%</strong></div>\n",hum_integral, hum_decimal);
						 sprintf(gasLStr, "<div class=\"reading\">LPG Level: <strong>%hu</strong></div>\n",gas);
						 sprintf(gasLStr, "<div class=\"reading\">LPG Level: <strong>%hu</strong></div>\n",gas);
						 sprintf(gasStr, "<div class=\"reading\" style=\"font-weight: bold;\">No Toxic Gas Detected!</div>\n");
							  memset(rxBuffer, 0, sizeof(rxBuffer));
							  HAL_UART_Receive(&huart1, rxBuffer, 512, 1000);

							  // Check if we have an incoming request for the page
							  if(strstr((char *)rxBuffer, "GET /"))
							  {
								  // Prepare and send the HTML page with the current temperature
									  sprintf(ATcommand, "AT+CIPSEND=0,%d\r\n", strlen(ATcommandB)  + strlen(tempStr)  + strlen(humStr) + strlen(gasLStr)+ strlen(gasStr)+ strlen(ATcommandT));
									  memset(rxBuffer, 0, sizeof(rxBuffer));
									  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 1000);
									  HAL_UART_Receive(&huart1, rxBuffer, 512, 100);

								  if(strstr((char *)rxBuffer, ">"))
								  {
									  memset(rxBuffer, 0, sizeof(rxBuffer));
									  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommandB, strlen(ATcommandB), 1000);
									  HAL_UART_Transmit(&huart1, (uint8_t *)tempStr, strlen(tempStr), 1000);
									  HAL_UART_Transmit(&huart1, (uint8_t *)humStr, strlen(humStr), 1000);
									  HAL_UART_Transmit(&huart1, (uint8_t *)gasLStr, strlen(gasLStr), 1000);
									  HAL_UART_Transmit(&huart1, (uint8_t *)gasStr, strlen(gasStr), 1000);
									  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommandT, strlen(ATcommandT), 1000);
									  HAL_UART_Receive(&huart1, rxBuffer, 512, 100);
								  }

								  // Close the connection after sending the response
								  sprintf(ATcommand, "AT+CIPCLOSE=0\r\n");
								  memset(rxBuffer, 0, sizeof(rxBuffer));
								  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 1000);
								  HAL_UART_Receive(&huart1, rxBuffer, 512, 100);

					 }


								  // Data Storage code
					  fres = f_open(&fil, "data.txt", FA_WRITE | FA_OPEN_ALWAYS);
						  if (fres == FR_OK) {
							myprintf("Opened 'data.txt' for appending\r\n");

							// Move the file pointer to the end to append
							f_lseek(&fil, f_size(&fil));

							// Append a new line
							f_write(&fil, "\r\n", 2, &bytesWrote);

							// Write message
							snprintf(writeBuf, sizeof(writeBuf),
									 " Date&Time: %04d-%02d-%02d %02d:%02d:%02d  Temp:%d.%d C  Hum:%d.%d %%  LPG Level:%hu Air Quality:Normal", 2025 + sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes,      								sTime.Seconds,tempC_integral, tempC_decimal,hum_integral, hum_decimal,gas);
							fres = f_write(&fil, writeBuf, strlen((char*)writeBuf), &bytesWrote);
							if (fres == FR_OK) {
							  myprintf("Wrote %i bytes: %s\r\n", bytesWrote, writeBuf);
							} else {
							  myprintf("f_write error (%i)\r\n", fres);
							}

							f_close(&fil);
						  } else {
							myprintf("Failed to open 'data.txt' (%i)\r\n", fres);
						  }




	  	      	         }
	  	         	 //	if the gas reading is above the threshold, activate the alert system and display the reading
	  	      	         else
	  	      	         {


	  	      	     	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Turn On LED
						 sprintf(string,"Toxic Gas Detected");
						 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Turn On  the Buzzer
					//	 Display the reading on OLed
						 SSD1306_Puts (string, &Font_7x10, 1);
						 SSD1306_UpdateScreen();
                   //		Send the reading to a web server
    	      	         sprintf(tempStr, "<div class=\"reading\">Temperature: <strong>%d.%d &deg;C</strong></div>\n",tempC_integral, tempC_decimal);
	  	      	         sprintf(humStr, "<div class=\"reading\">Humidity: <strong>%d.%d %%</strong></div>\n",hum_integral, hum_decimal);
	  	      	         sprintf(gasLStr, "<div class=\"reading\">LPG Level: <strong>%hu</strong></div>\n",gas);
						  sprintf(gasStr, "<div class=\"reading\" style=\"color: red; font-weight: bold;\">⚠️ Toxic Gas Detected!</div>\n");

						  memset(rxBuffer, 0, sizeof(rxBuffer));
						  HAL_UART_Receive(&huart1, rxBuffer, 512, 1000);


						  // Check if we have an incoming request for the page
						  if(strstr((char *)rxBuffer, "GET /"))
						  {
							  // Prepare and send the HTML page with the current temperature
							  sprintf(ATcommand, "AT+CIPSEND=0,%d\r\n", strlen(ATcommandB)  + strlen(tempStr) + strlen(humStr) + strlen(gasLStr)+ strlen(gasStr)+ strlen(ATcommandT));
							  memset(rxBuffer, 0, sizeof(rxBuffer));
							  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 1000);
							  HAL_UART_Receive(&huart1, rxBuffer, 512, 100);

							  if(strstr((char *)rxBuffer, ">"))
							  {
								  memset(rxBuffer, 0, sizeof(rxBuffer));
								  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommandB, strlen(ATcommandB), 1000);
								  HAL_UART_Transmit(&huart1, (uint8_t *)tempStr, strlen(tempStr), 1000);
								  HAL_UART_Transmit(&huart1, (uint8_t *)humStr, strlen(humStr), 1000);
								  HAL_UART_Transmit(&huart1, (uint8_t *)gasLStr, strlen(gasLStr), 1000);
								  HAL_UART_Transmit(&huart1, (uint8_t *)gasStr, strlen(gasStr), 1000);
//								  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommandBTN, strlen(ATcommandBTN), 1000);
								  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommandT, strlen(ATcommandT), 1000);
								  HAL_UART_Receive(&huart1, rxBuffer, 512, 100);
							  }

							  // Close the connection after sending the response
							  sprintf(ATcommand, "AT+CIPCLOSE=0\r\n");
							  memset(rxBuffer, 0, sizeof(rxBuffer));
							  HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 1000);
							 HAL_UART_Receive(&huart1, rxBuffer, 512, 100);



							  }

//						  SD Card code
						  fres = f_open(&fil, "data.txt", FA_WRITE | FA_OPEN_ALWAYS);
							  if (fres == FR_OK) {
								myprintf("Opened 'data.txt' for appending\r\n");

								// Move the file pointer to the end to append
								f_lseek(&fil, f_size(&fil));

								// Append a new line
								f_write(&fil, "\r\n", 2, &bytesWrote);

								// Write message
								snprintf(writeBuf, sizeof(writeBuf), " Date&Time: %04d-%02d-%02d %02d:%02d:%02d  Temp:%d.%d C  Hum:%d.%d %%  LPG Level:%hu Air Quality:Toxic", 2025 + sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds,tempC_integral, tempC_decimal,hum_integral, hum_decimal,gas);
								fres = f_write(&fil, writeBuf, strlen((char*)writeBuf), &bytesWrote);
								if (fres == FR_OK) {
								  myprintf("Wrote %i bytes: %s\r\n", bytesWrote, writeBuf);
								} else {
								  myprintf("f_write error (%i)\r\n", fres);
								}

								f_close(&fil);
							  } else {
								myprintf("Failed to open 'data.txt' (%i)\r\n", fres);
							  }



	  	       }
//	  	Checking if the gas reading is above the threshold
	  	       if ( tempC_integral > 28 & tempC_decimal >=1)
				 {
					 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  // Turn on Led
					 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Turn On Buzzer
					 HAL_Delay(500);
					 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Turn off Buzzer
					 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
					 HAL_Delay(500);

					 }
	  	     }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */
  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2) {
    // RTC not yet initialized: set default time and date

    sTime.Hours = 0x12;
    sTime.Minutes = 0x22;
    sTime.Seconds = 0x00;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
    sDate.Month = RTC_MONTH_APRIL;
    sDate.Date = 0x28;
    sDate.Year = 0x25; // 2025

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }

    // Mark RTC as initialized
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
  }
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */

  /* USER CODE BEGIN RTC_Init 2 */
  // You can place code here to read current time/date if needed
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pins : PA1 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
