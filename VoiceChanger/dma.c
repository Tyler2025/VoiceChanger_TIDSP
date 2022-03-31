/*
 * dma.c
 *
 *  Created on: 2022年3月25日
 *      Author: Tyler
 */
#include "dma.h"
#include "csl_dma.h"
#include "csl_intc.h"
#include <stdio.h>
#include "fir.h"
#include "audio_common.h"
#include <string.h>
#include "gpio.h"
unsigned int RX_PP_Flag = 0;
unsigned int TX_PP_Flag = 0;
//fir滤波后
Int16 FIRBuf[CSL_DMA_BUFFER_SIZE];
Int16 Out[CSL_DMA_BUFFER_SIZE];
//Int16 Deci[];
int loop,m;
extern Int8 FIR_LPF[];
extern Uint16 flag[];
extern void VECSTART(void);
#pragma DATA_ALIGN (RXdmaPingDstBuf, 4)
Int16 RXdmaPingDstBuf[CSL_DMA_BUFFER_SIZE];
#pragma DATA_ALIGN (RXdmaPongDstBuf, 4)
Int16 RXdmaPongDstBuf[CSL_DMA_BUFFER_SIZE];

#pragma DATA_ALIGN (TXdmaPingDstBuf, 4)
Int16 TXdmaPingDstBuf[CSL_DMA_BUFFER_SIZE];
#pragma DATA_ALIGN (TXdmaPongDstBuf, 4)
Int16 TXdmaPongDstBuf[CSL_DMA_BUFFER_SIZE];

int count=0 ;
int sample=0;
CSL_DMA_Handle 		dmaHandle;
CSL_DMA_Config 		dmaConfig;
CSL_DMA_ChannelObj  dmaObj;
Uint16   			chanNumber=4;//DMA1_Channel0
CSL_Status 			csl_status;

CSL_DMA_Handle 		dir_dmaHandle;
CSL_DMA_Config 		dir_dmaConfig;
CSL_DMA_ChannelObj  dir_dmaObj;
Uint16   			dir_chanNumber=5;//DMA1_Channel1
CSL_Status 			dir_csl_status;

int DMA_PingPong_Init(void)
{
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S2_RX;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = CSL_DMA_BUFFER_SIZE*4;
	dmaConfig.srcAddr      = 0x00002A28;
	dmaConfig.destAddr     = (Uint32)RXdmaPingDstBuf;

	csl_status = DMA_init();
    if (csl_status != CSL_SOK)
    {
        printf("DMA_init() Failed \n");
    }
	dmaHandle = DMA_open((CSL_DMAChanNum)chanNumber,&dmaObj, &csl_status);
    if (dmaHandle == NULL)
    {
        printf("DMA_open() Failed \n");
    }

    csl_status = DMA_config(dmaHandle, &dmaConfig);
    if (csl_status != CSL_SOK)
    {
        printf("DMA_config() Failed \n");
    }
    csl_status = DMA_start(dmaHandle);
    if (csl_status != CSL_SOK)
    {
        printf("DMA_start() Failed \n");
    }

    return 1;
}

int DMA_Dir_Init(void)
{
	dir_dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
	dir_dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
	dir_dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dir_dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dir_dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S2_TX;
	dir_dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
	dir_dmaConfig.chanDir      = CSL_DMA_WRITE;
	dir_dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dir_dmaConfig.dataLen      = CSL_DMA_BUFFER_SIZE*4;
	dir_dmaConfig.srcAddr      = (Uint32)TXdmaPingDstBuf;
	dir_dmaConfig.destAddr     = 0x00002A08;


    dir_dmaHandle = DMA_open((CSL_DMAChanNum)dir_chanNumber,&dir_dmaObj, &dir_csl_status);
    if (dir_dmaHandle == NULL)
    {
        printf("DMA_open() Failed \n");
    }

    dir_csl_status = DMA_config(dir_dmaHandle, &dir_dmaConfig);
    if (dir_csl_status != CSL_SOK)
    {
        printf("DMA_config() Failed \n");
    }
    dir_csl_status = DMA_start(dir_dmaHandle);
    if (dir_csl_status != CSL_SOK)
    {
        printf("DMA_start() Failed \n");
    }
    return 1;
}

void DMA_DIS(void)
{
	csl_status = DMA_close(dmaHandle);
	if (csl_status != CSL_SOK)
	{
		printf("DMA_close() Failed \n");
	}

	csl_status = DMA_reset(dmaHandle);
	if (csl_status != CSL_SOK)
	{
		printf("DMA_reset() Failed \n");
	}
	dir_csl_status = DMA_close(dir_dmaHandle);
    if (dir_csl_status != CSL_SOK)
    {
        printf("DMA_close() Failed \n");
    }

    dir_csl_status = DMA_reset(dir_dmaHandle);
    if (dir_csl_status != CSL_SOK)
    {
        printf("DMA_reset() Failed \n");
    }
}
interrupt void dma_isr(void)
{
    Uint16 ifrValue;

  	ifrValue = CSL_SYSCTRL_REGS->DMAIFR;//处理中断标志位
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

	if ((ifrValue&0x0010) == 16)//接收中断
	{
		if (flag[1]==0)//变声
		{
			if ((DMA_getLastTransferType (dmaHandle, &csl_status)) == 1)
			{
				if (flag[2]==0)//女变男
				{
					for (loop=0;loop<CSL_DMA_BUFFER_SIZE;loop+=2)
					{
						m=loop/B*A;
						*(Out+loop)=*(RXdmaPongDstBuf+m);
					}
					//c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
				}
				else if(flag[2]==1)//男边女
				{
				//C55x_delay_msec( 1000 );
                    //c_fir(RXdmaPongDstBuf,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
					for (loop=0;loop<CSL_DMA_BUFFER_SIZE*A/B;loop+=2)
					{
						m=loop/A*B;
						*(Out+loop)=*(RXdmaPongDstBuf+m);
					}
					for (loop=CSL_DMA_BUFFER_SIZE*A/B;loop<CSL_DMA_BUFFER_SIZE;loop++)
					{
						*(Out+loop)=0;
					}
                    c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
					//c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
				}
			}
			else
			{
				if (flag[2]==0)//女边男
				{
					for (loop=0;loop<CSL_DMA_BUFFER_SIZE;loop+=2)
					{
						m=loop/B*A;
						*(Out+loop)=*(RXdmaPingDstBuf+m);
					}
					//c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
				}
				else if(flag[2]==1)//男边女
				{
				//C55x_delay_msec( 1000 );
                    //c_fir(RXdmaPingDstBuf,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
					for (loop=0;loop<CSL_DMA_BUFFER_SIZE*A/B;loop+=2)
					{
						m=loop/A*B;
						*(Out+loop)=*(RXdmaPingDstBuf+m);
					}
					for (loop=CSL_DMA_BUFFER_SIZE*A/B;loop<CSL_DMA_BUFFER_SIZE;loop++)
					{
						*(Out+loop)=0;
					}
                    c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
				}/*
				for (loop=0;loop<CSL_DMA_BUFFER_SIZE;loop+=2)
				{
					m=loop/3*2;
					*(Out+loop)=*(RXdmaPingDstBuf+m);
				}
				c_fir(Out,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);*/
			}
		}
		else if (flag[1]==1)//不变声
		{/*
		    if ((DMA_getLastTransferType (dmaHandle, &csl_status)) == 1)
		    {
		        c_fir(RXdmaPongDstBuf,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
		    }
		    else
		    {
                c_fir(RXdmaPingDstBuf,FIRBuf,CSL_DMA_BUFFER_SIZE,FIR_LPF,ORDER+1);
		    }*/
			//C55x_delay_msec( 1000 );
			/*for (loop=0;loop<1536;loop++)
			{
				m=4*loop/3;
				*(Out+loop)=*(RXdmaPongDstBuf+m);
			}
			for (loop=1536;loop<CSL_DMA_BUFFER_SIZE;loop++)
			{
				//m=4*loop/3;
				*(Out+loop)=0;
			}
			*/
		}
	}
	if ((ifrValue&0x0020) == 32)//发送中断
	{
		//if (flag[0]==0)//播放
		//{
			if (flag[1]==0)//变声
			{
				if ((DMA_getLastTransferType (dir_dmaHandle, &dir_csl_status)) == 1)
				{
				    if(flag[2]==0)//女变男
				    {
                        for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++) {
                            TXdmaPongDstBuf[loop] = Out[loop];
                        }
				    }
				    else if(flag[2]==1)
				    {
                        for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++) {
                            TXdmaPongDstBuf[loop] = FIRBuf[loop];
                        }
				    }
				}
				else
				{
                    if(flag[2]==0)
                    {
                        for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++) {
                            TXdmaPingDstBuf[loop] = Out[loop];
                        }
                    }
                    else if(flag[2]==1)
                    {
                        for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++) {
                            TXdmaPingDstBuf[loop] = FIRBuf[loop];
                        }
                    }
				}
			}
			else if (flag[1]==1)//不变生
			{
				if ((DMA_getLastTransferType (dir_dmaHandle, &dir_csl_status)) == 1)
				{
					for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++)
					{
						TXdmaPongDstBuf[loop] = RXdmaPongDstBuf[loop];
					}
				}
				else
				{
					for(loop = 0; loop < CSL_DMA_BUFFER_SIZE; loop++)
					{
						TXdmaPingDstBuf[loop] = RXdmaPingDstBuf[loop];
					}
				}
			}
		}
		//else if(flag[0]==1)//暂停播放
		//{

		//}


	++count;
}
