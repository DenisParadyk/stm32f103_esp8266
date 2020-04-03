/*
 * RingFIFO.c
 *
 *  Created on: 14 џэт. 2020 у.
 *      Author: Denis Paradiuk
 */

#include "RingFIFO.h"

static void RingIncr(uint16_t *val, uint16_t max)
{
  (*val)++;

  if((*val) >= max)
    (*val) = 0;
}

static void Incr(uint16_t *val, uint16_t max)
{
  if((*val) < max)
    (*val)++;
}


static void Decr(uint16_t *val)
{
  if((*val) != 0)
    (*val)--;
}

/******************************************************************************/

void RingBuffInit(RingBuff_t *buff, uint8_t *Mem, uint16_t Len)
{
  buff->Head = 0;
  buff->NumOfItems = 0;

  buff->Len = Len;

  buff->Buff = Mem;
}

void RingBuffPut(RingBuff_t *buff, uint8_t val)
{
  buff->Buff[buff->Head] = val;
  RingIncr(&buff->Head, buff->Len);
  Incr(&buff->NumOfItems, buff->Len);
}

void RingBuffPutString(RingBuff_t* buff, uint8_t* data, uint16_t Len)
{
	for(int i = 0; i < Len; i++)
	{
		buff->Buff[buff->Head] = data[i];
		RingIncr(&buff->Head, buff->Len);
		Incr(&buff->NumOfItems, buff->Len);
	}
}

int16_t RingBuffGet(RingBuff_t *buff)
{
  uint8_t ret;
  int16_t tail;

  if(buff->NumOfItems == 0)
    return -1;

  tail = buff->Head - buff->NumOfItems;

  if(tail < 0)
    tail = buff->Len + tail;

  ret = buff->Buff[tail];

  Decr(&buff->NumOfItems);

  return ret;
}

uint16_t RingBuffNumOfItems(RingBuff_t *buff)
{
  return buff->NumOfItems;
}

void RingBuffClear(RingBuff_t *buff)
{
  buff->NumOfItems = 0;
}
