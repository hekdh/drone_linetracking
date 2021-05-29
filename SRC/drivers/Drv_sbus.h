#ifndef _DRV_SBUS_H_
#define _DRV_SBUS_H_

#include "stm32f4xx.h"
extern u16 Rc_Sbus_In[16];

//
void Drv_SbusInit(void);
void Sbus_IRQ(void);

#endif


