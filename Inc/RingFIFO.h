/*
 * RingFIFO.h
 *
 *  Created on: 14 џэт. 2020 у.
 *      Author: Denis Paradiuk
 */

#ifndef CODE_INCLUDE_RINGFIFO_H_
#define CODE_INCLUDE_RINGFIFO_H_

#include "stm32f1xx.h"

typedef struct
{
  uint16_t Len;
  uint16_t Head;
  uint16_t NumOfItems;
  uint8_t *Buff;
} RingBuff_t;

#define UART_TXBUFF_LENGHT     8
#define UART_RXBUFF_LENGHT     8

void RingBuffInit(RingBuff_t *buff, uint8_t *Mem, uint16_t Len);
void RingBuffPut(RingBuff_t *buff, uint8_t val);
void RingBuffPutString(RingBuff_t* buff, uint8_t* data, uint16_t Len);
int16_t RingBuffGet(RingBuff_t *buff);
uint16_t RingBuffNumOfItems(RingBuff_t *buff);
void RingBuffClear(RingBuff_t *buff);

#endif /* CODE_INCLUDE_RINGFIFO_H_ */
