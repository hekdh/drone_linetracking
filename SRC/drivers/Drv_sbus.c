/*==========================================================================
 * 描述    ：SBUS驱动、数据解析
 * 更新时间：2018-11-11 
 * 作者		 ：匿名科创-茶不思、Jyoun
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 交流Q群 ：190169595
 * 技术Q群 ：194080682
============================================================================
 * 匿名科创团队感谢大家的支持，欢迎大家进群互相交流、讨论、学习。
 * 若您觉得匿名有不好的地方，欢迎您拍砖提意见。
 * 若您觉得匿名好，请多多帮我们推荐，支持我们。
 * 匿名开源程序代码欢迎您的引用、延伸和拓展，不过在希望您在使用时能注明出处。
 * 开源不易，利润微薄，希望大家积极抵制山寨产品。
 * 只有您的支持，匿名才能做得更好。 
============================================================================
 * 本篇相关资料：https://www.digiwang.com/zatansbus.html/
===========================================================================*/

#include "Drv_sbus.h"
#include "include.h"

void Drv_SbusInit(void)
{
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_StructInit(&GPIO_InitStructure);

	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_USART6, ENABLE ); 
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOC, ENABLE );
	
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART6_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_UART6_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
	
	GPIO_PinAFConfig ( GPIOC, GPIO_PinSource7, GPIO_AF_USART6 );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//GPIO_PuPd_UP;//
    GPIO_Init ( GPIOC, &GPIO_InitStructure );
	
	USART_InitStructure.USART_BaudRate = 100000;       
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_2;   
    USART_InitStructure.USART_Parity = USART_Parity_Even;    
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
    USART_InitStructure.USART_Mode = USART_Mode_Rx; 
    USART_Init ( USART6, &USART_InitStructure );

    USART_ITConfig ( USART6, USART_IT_RXNE, ENABLE );

    USART_Cmd ( USART6, ENABLE );
}

#include "Ano_RC.h"
u16 Rc_Sbus_In[16];
u8 sbus_flag;

/*
sbus flags的结构如下所示：
flags：
bit7 = ch17 = digital channel (0x80)
bit6 = ch18 = digital channel (0x40)
bit5 = Frame lost, equivalent red LED on receiver (0x20)
bit4 = failsafe activated (0x10) b: 0001 0000
bit3 = n/a
bit2 = n/a
bit1 = n/a
bit0 = n/a

*/
void Sbus_GetByte(u8 data)
{
	static u8 datatmp[25];
	static u8 cnt = 0;
	
	datatmp[cnt++] = data;
	
	if(cnt == 25)
	{
		if(datatmp[0] == 0x0F && datatmp[24] == 0x00)
		{
			cnt = 0;
			Rc_Sbus_In[0] = (s16)(datatmp[2] & 0x07) << 8 | datatmp[1];
			Rc_Sbus_In[1] = (s16)(datatmp[3] & 0x3f) << 5 | (datatmp[2] >> 3);
			Rc_Sbus_In[2] = (s16)(datatmp[5] & 0x01) << 10 | ((s16)datatmp[4] << 2) | (datatmp[3] >> 6);
			Rc_Sbus_In[3] = (s16)(datatmp[6] & 0x0F) << 7 | (datatmp[5] >> 1);
			Rc_Sbus_In[4] = (s16)(datatmp[7] & 0x7F) << 4 | (datatmp[6] >> 4);
			Rc_Sbus_In[5] = (s16)(datatmp[9] & 0x03) << 9 | ((s16)datatmp[8] << 1) | (datatmp[7] >> 7);
			Rc_Sbus_In[6] = (s16)(datatmp[10] & 0x1F) << 6 | (datatmp[9] >> 2);
			Rc_Sbus_In[7] = (s16)datatmp[11] << 3 | (datatmp[10] >> 5);
			
			Rc_Sbus_In[8] = (s16)(datatmp[13] & 0x07) << 8 | datatmp[12];
			Rc_Sbus_In[9] = (s16)(datatmp[14] & 0x3f) << 5 | (datatmp[13] >> 3);
			Rc_Sbus_In[10] = (s16)(datatmp[16] & 0x01) << 10 | ((s16)datatmp[15] << 2) | (datatmp[14] >> 6);
			Rc_Sbus_In[11] = (s16)(datatmp[17] & 0x0F) << 7 | (datatmp[16] >> 1);
			Rc_Sbus_In[12] = (s16)(datatmp[18] & 0x7F) << 4 | (datatmp[17] >> 4);
			Rc_Sbus_In[13] = (s16)(datatmp[20] & 0x03) << 9 | ((s16)datatmp[19] << 1) | (datatmp[18] >> 7);
			Rc_Sbus_In[14] = (s16)(datatmp[21] & 0x1F) << 6 | (datatmp[20] >> 2);
			Rc_Sbus_In[15] = (s16)datatmp[22] << 3 | (datatmp[21] >> 5);
			sbus_flag = datatmp[23];
			
			//user
			//
			if(sbus_flag & 0x10)
			{
				//如果有数据且能接收到有失控标记，则不处理，转嫁成无数据失控。
			}
			else
			{
				//否则有数据就喂狗
				for(u8 i = 0;i < 8;i++)//原RC接收程序只设计了8个通道
				{
					ch_watch_dog_feed(i);
				}
			}
		}
		else
		{
			for(u8 i=0; i<24;i++)
				datatmp[i] = datatmp[i+1];
			cnt = 24;
		}
	}
}

void Sbus_IRQ(void)
{
	u8 com_data;
	
	if ( USART_GetITStatus ( USART6, USART_IT_RXNE ) )
    {
        USART_ClearITPendingBit ( USART6, USART_IT_RXNE ); 

        com_data = USART6->DR;
			Sbus_GetByte(com_data);
    }
}

