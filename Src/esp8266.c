/*
 * esp8266.c
 *
 *  Created on: 16 февр. 2020 г.
 *      Author: Denis Paradiuk
 */

#include "esp8266.h"

#define AT_TEST				 "AT\r\n"
#define AT_RESET   		 	 "AT+RST\r\n"
#define AT_SCAN_AP 			 "AT+CWLAP\r\n"
#define AT_CONNECT_AP        "AT+CWJAP_CUR="
#define AT_DISCONNECT_AP	 "AT+CWQAP\r\n"
#define AT_CONNECTION_STATUS "AT+CIPSTATUS\r\n"
#define AT_CONNECT_TCP 		 "AT+CIPSTART=\"TCP\",\""
#define AT_TCP_SEND 		 "AT+CIPSEND="

static char buffAP[200];
static bool tcp_recv = false;

bool esp8266_Init()
{
	uint8_t tmp[11];

	memset(tmp, 0, sizeof(tmp));

	if(HAL_UART_Transmit(&huart2, AT_TEST, sizeof(AT_TEST) - 1, HAL_MAX_DELAY) == HAL_OK)
	{
		if(HAL_UART_Receive(&huart2, (uint8_t*)tmp, sizeof(tmp), HAL_MAX_DELAY) == HAL_OK)
		{
				if(tmp[7] == 'O' && tmp[8] == 'K')
				{
					return true;
				}
		}
	}
	return false;
}

int esp8266_ScanAP()
{
	int APcounter;
	uint8_t tmp[300];

	memset(tmp, 0, sizeof(tmp));

	if(HAL_UART_Transmit(&huart2, AT_SCAN_AP, sizeof(AT_SCAN_AP) - 1, HAL_MAX_DELAY) == HAL_OK)
	{
		if(HAL_UART_Receive_IT(&huart2, (uint8_t*)tmp, sizeof(tmp)) == HAL_OK)
		{
			int buffIndex = 0;
			for(uint32_t i = 0; i < sizeof(tmp); i++)
			{
				if(tmp[i]     == 'C' &&
				   tmp[i + 1] == 'W' &&
				   tmp[i + 2] == 'L' &&
				   tmp[i + 3] == 'A' &&
				   tmp[i + 4] == 'P' &&
				   tmp[i + 5] == ':')
				{
					uint32_t ssidByte = i + 10;

					buffAP[buffIndex] = tmp[ssidByte];

					while(tmp[ssidByte] != '"')
					{
						buffIndex++;
						ssidByte++;
						buffAP[buffIndex] = tmp[ssidByte];
					}

					if(tmp[ssidByte] == '"')
					{
						APcounter++;
						buffAP[buffIndex] = ';';
						return APcounter;
					}
				}
			}
		}
	}
}

void esp8266_GetAPName(int APnumber, uint8_t* APname)
{
	if(APnumber == 1)
	{
		int i = 0;

		while(buffAP[i] != ';')
		{
			APname[i] = buffAP[i];
			i++;
		}
	}
	else
	{
		int tmp = 0;

		for(int i = 0; i < sizeof(buffAP); i++)
		{
			if(buffAP[i] == ';')
			{
				if(tmp + 1 == APnumber)
				{
					int ssidByte = i + 1;

					for(int j = 0; j < 50; j++)
					{
						APname[j] = buffAP[ssidByte];
						ssidByte++;
						if(buffAP[ssidByte] == ';') {break;}
					}
				}
				else
				{
					tmp++;
				}
			}
		}
	}
}

bool esp8266_ConnectAP(uint8_t* APname, uint8_t* APpassword)
{
	char tmp[100];
	char ConnectAP[100];

	memset(tmp, 0, sizeof(tmp));
	memset(ConnectAP, 0, sizeof(ConnectAP));

	sprintf(ConnectAP, "%s\"%s\",\"%s\"\r\n",AT_CONNECT_AP,APname,APpassword);

	if(HAL_UART_Transmit_IT(&huart2, ConnectAP, strlen(ConnectAP)) == HAL_OK)
	{
		if(HAL_UART_Receive_IT(&huart2, tmp, sizeof(tmp)) == HAL_OK)
		{
			for(int j = 0; j < 1000; j++)
			{
				HAL_Delay(10);
				for(int i = 0; i < sizeof(tmp); i++)
				{
					if(tmp[i]     == 'C' &&
					   tmp[i + 1] == 'O' &&
					   tmp[i + 2] == 'N' &&
					   tmp[i + 3] == 'N' &&
					   tmp[i + 4] == 'E' &&
					   tmp[i + 5] == 'C' &&
					   tmp[i + 6] == 'T' &&
					   tmp[i + 7] == 'E' &&
					   tmp[i + 8] == 'D' )
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool esp8266_DisconnectAP()
{
	char tmp[100];

	memset(tmp, 0, sizeof(tmp));

	if(HAL_UART_Transmit_IT(&huart2, AT_DISCONNECT_AP, sizeof(AT_DISCONNECT_AP) - 1) == HAL_OK)
	{
		if(HAL_UART_Receive_IT(&huart2, tmp, sizeof(tmp) - 1) == HAL_OK)
		{
			for(int i = 0; i < sizeof(tmp); i++)
			{
				if(tmp[i] == 'O' && tmp[i + 1] == 'K')
				{
					return true;
				}
			}
		}
	}
	return false;
}

int esp8266_CheckConnectionStatus()
{
	char tmp[100];

	memset(tmp, 0, sizeof(tmp));

	if(HAL_UART_Transmit_IT(&huart2, AT_CONNECTION_STATUS, sizeof(AT_CONNECTION_STATUS) - 1) == HAL_OK)
	{
		if(HAL_UART_Receive_IT(&huart2, tmp, sizeof(tmp) - 1) == HAL_OK)
		{
			for(int i = 0; i < sizeof(tmp); i++)
			{
				if(tmp[i]     == 'S' &&
				   tmp[i + 1] == 'T' &&
				   tmp[i + 2] == 'A' &&
				   tmp[i + 3] == 'T' &&
				   tmp[i + 4] == 'U' &&
				   tmp[i + 5] == 'S' )
				{
					switch(tmp[i + 7])
					{
					case '2':
						return 2;
						break;
					case '3':
						return 3;
						break;
					case '4':
						return 4;
						break;
					case '5':
						return 5;
						break;
					default:
						return false;
					}
				}
			}
		}
	}
}

void esp8266_GetConnectionStatus(int StatusNumber, uint8_t* StatusName)
{
	switch(StatusNumber)
	{
	case 2:
		StatusName = "WIFI CONNECT";
		break;
	case 3:
		StatusName = "TCP CONNECT";
		break;
	case 4:
		StatusName = "TCP DISCONNECT";
		break;
	case 5:
		StatusName = "WIFI DISCONNECT";
		break;
	}
}

bool esp8266_ConnectTCP(uint8_t* ip, int port)
{
	char tmp[100];
	char ConnectTCP[100];

	memset(tmp, 0, sizeof(tmp));
	memset(ConnectTCP, 0, sizeof(ConnectTCP));

	char* m_port = itoa(port, tmp, 10);

	sprintf(ConnectTCP, "%s%s%s%s%s", AT_CONNECT_TCP, ip, "\",", m_port, "\r\n");

	if(HAL_UART_Transmit_IT(&huart2, ConnectTCP, strlen(ConnectTCP)) == HAL_OK)
	{
		if(HAL_UART_Receive_IT(&huart2, tmp, sizeof(tmp) - 1) == HAL_OK)
		{
			for(int j = 0; j < 1000; j++)
			{
				HAL_Delay(1);
				for(int i = 0; i < sizeof(tmp); i++)
				{
					if(tmp[i]     == 'C' &&
					   tmp[i + 1] == 'O' &&
					   tmp[i + 2] == 'N' &&
					   tmp[i + 3] == 'N' &&
					   tmp[i + 4] == 'E' &&
					   tmp[i + 5] == 'C' &&
					   tmp[i + 6] == 'T')
					{
						tcp_recv = true;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool esp8266_SendTCP(uint8_t* data, int length)
{
	if(length != 0)
	{
		char tmp[100];
		char tmp2[100];
		char SendTCP[100];
		char SendData[100];

		memset(tmp, 0, sizeof(tmp));
		memset(tmp2, 0, sizeof(tmp2));
		memset(SendTCP, 0, sizeof(SendTCP));
		memset(SendData, 0, sizeof(SendData));

		char* m_length = itoa(length, tmp, 10);

		sprintf(SendTCP, "%s%s%s", AT_TCP_SEND, m_length, "\r\n");

		sprintf(SendData, "%s\r\n", data);

		if(HAL_UART_Transmit_IT(&huart2, SendTCP, strlen(SendTCP)) == HAL_OK)
		{
			HAL_Delay(100);
			if(HAL_UART_Transmit_IT(&huart2, SendData, strlen(SendData)) == HAL_OK)
			{
				HAL_Delay(100);
				if(HAL_UART_Receive_IT(&huart2, tmp2, sizeof(tmp2)) == HAL_OK)
				{
					for(int k = 0; k < 100; k++)
					{
						HAL_Delay(1);
						for(int i = 0; i < sizeof(tmp2); i++)
						{
							if(tmp2[i]     == 'S' &&
							   tmp2[i + 1] == 'E' &&
							   tmp2[i + 2] == 'N' &&
							   tmp2[i + 3] == 'D' &&
							   tmp2[i + 4] == ' ' &&
							   tmp2[i + 5] == 'O' &&
							   tmp2[i + 6] == 'K')
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

#define RXNE(UARTx)     (UARTx->SR & USART_SR_RXNE)
#define RXNEIE(UARTx)   (UARTx->CR1 & USART_CR1_RXNEIE)

volatile uint8_t temp;

static uint8_t buff[5];
static bool readyReadBytesNumber = false;
static uint16_t number = 0;
static bool getBytes = false;
static int num = 0;
static uint8_t mem[100];

int getTCPbuff(uint8_t *buff)
{
	for(int i = 0; i < number; i++)
	{
		buff[i] = mem[i];
	}
	return number;
}

void clearTCPbuff()
{
	number = 0;
}

void Esp8266_setRx(void)
{
	HAL_UART_Receive_IT(&huart2, temp, 1);
}

/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART2)
	  {
		  Esp8266_setRx();
		  counter++;
		  if (counter == 3) {
			  temp = 2;
		  }


	  }


	// HAL_UART_Receive_IT(&huart2, temp, 1);
	 //temp = USART1->DR;

/*

	   	if (readyReadBytesNumber)
	   	{
	   		if (isdigit(temp))
	   		{
	   			number *= 10;
	   			number += temp - 0x30;
	   			return;
	   		}

	   		readyReadBytesNumber = false;
	   		getBytes = true;

	   		return;
	   	}
	   	else if (getBytes)
	   	{
	   		while(num < number)
	   		{
	   			mem[num] = temp;
	   			num++;

	   			return;
	   		}

	   		getBytes = false;
	   	}

	   	buff[4]  = temp;

	   	if (!strcmp(buff, "+IPD,", 5))
	   	{
	   		readyReadBytesNumber = true;
	   	}
	   	buff[0] = buff[1];
	   	buff[1] = buff[2];
	   	buff[2] = buff[3];
	   	buff[3] = buff[4];
	}
}*/

void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	temp = USART2->DR;

	if (readyReadBytesNumber)
	{
		if (isdigit(temp))
		{
			number *= 10;
			number += temp - 0x30;
			return;
		}

		readyReadBytesNumber = false;
		getBytes = true;

		return;
	}
	else if (getBytes)
	{
		while(num < number)
		{
			mem[num] = temp;
			num++;

			return;
		}

		getBytes = false;
	}

	buff[4]  = temp;

	if (!strcmp(buff, "+IPD,", 5))
	{
		readyReadBytesNumber = true;
	}
	buff[0] = buff[1];
	buff[1] = buff[2];
	buff[2] = buff[3];
	buff[3] = buff[4];
  /* USER CODE END USART2_IRQn 1 */
}
