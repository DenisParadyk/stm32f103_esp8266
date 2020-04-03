/*
 * esp8266.h
 *
 *  Created on: 16 февр. 2020 г.
 *      Author: Denis Paradiuk
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

#include "main.h"
#include "usart.h"
#include "RingFIFO.h"
#include "stm32f1xx.h"

typedef int bool;
#define true 1
#define false 0

bool esp8266_Init();
int  esp8266_ScanAP();
void esp8266_GetAPName(int APnumber, uint8_t* APname);
bool esp8266_ConnectAP(uint8_t* APname, uint8_t* APpassword);
bool esp8266_DisconnectAP();
int  esp8266_CheckConnectionStatus();
void esp8266_GetConnectionStatus(int StatusNumber, uint8_t* StatusName);
bool esp8266_ConnectTCP(uint8_t* ip, int port);
bool esp8266_SendTCP(uint8_t* data, int length);
void Esp8266_setRx(void);

int getTCPbuff(uint8_t *buff);
void clearTCPbuff();

#endif /* INC_ESP8266_H_ */
