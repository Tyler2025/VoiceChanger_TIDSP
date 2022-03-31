/*
 * dma.h
 *
 *  Created on: 2022Äê3ÔÂ25ÈÕ
 *      Author: Tyler
 */

#ifndef DMA_H_
#define DMA_H_
#define M 3
#define L 2
int DMA_PingPong_Init(void);
int DMA_Dir_Init(void);
interrupt void dma_isr(void);
void DMA_DIS(void);
#define CSL_DMA_BUFFER_SIZE 512
#define TRANS_TIME			2
#define A					2
#define B					3

#endif /* DMA_H_ */
