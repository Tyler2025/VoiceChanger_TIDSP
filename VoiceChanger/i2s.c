/*
 * i2s.c
 *
 *  Created on: 2022年3月28日
 *      Author: Tyler
 */
#include "i2s.h"
#include "audio_common.h"

int Codec_I2S_Init(void *testArgs)
{
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write(  1, 0x01 );      // Reset codec
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write(  1, 0x08 );      // Disable crude AVDD generation from DVDD
	AIC3206_write(  2, 0x00 );      // Enable Analog Blocks
	// PLL and Clocks config and Power Up
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 27, 0x00 );      // BCLK and WCLK is set as i/p to AIC3206(Slave)
	AIC3206_write(  4, 0x07 );      // PLL setting: PLLCLK <- BCLK and CODEC_CLKIN <-PLL CLK
	AIC3206_write(  6, 0x20 );      // PLL setting: J = 32
	AIC3206_write(  7, 0 );         // PLL setting: HI_BYTE(D)
	AIC3206_write(  8, 0 );         // PLL setting: LO_BYTE(D)
	// For 48 KHz sampling
	AIC3206_write(  5, 0x92 );      // PLL setting: Power up PLL, P=1 and R=2
	AIC3206_write( 13, 0x00 );      // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
	AIC3206_write( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
	AIC3206_write( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	AIC3206_write( 11, 0x84 );      // Power up NDAC and set NDAC value to 4
	AIC3206_write( 12, 0x82 );      // Power up MDAC and set MDAC value to 2
	AIC3206_write( 18, 0x84 );      // Power up NADC and set NADC value to 4
	AIC3206_write( 19, 0x82 );      // Power up MADC and set MADC value to 2
	// DAC ROUTING and Power Up
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 12, 0x08 );      // LDAC AFIR routed to HPL
	AIC3206_write( 13, 0x08 );      // RDAC AFIR routed to HPR
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 64, 0x02 );      // Left vol=right vol
	AIC3206_write( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
	AIC3206_write( 63, 0xd4 );      // Power up left,right data paths and set channel
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 16, 0x06 );      // Unmute HPL , 6dB gain
	AIC3206_write( 17, 0x06 );      // Unmute HPR , 6dB gain
	AIC3206_write(  9, 0x30 );      // Power up HPL,HPR
	AIC3206_write(  0, 0x00 );      // Select page 0
	C55x_delay_msec( 5);            // Wait 5 msec
	// ADC ROUTING and Power Up
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 51, 0x40 );      // SetMICBIAS
	AIC3206_write( 52, 0x30 );      // STEREO 1 Jack
	// IN2_L to LADC_P through 40 kohm
	AIC3206_write( 55, 0x03 );      // IN2_R to RADC_P through 40 kohmm
	AIC3206_write( 54, 0x03 );      // CM_1 (common mode) to LADC_M through 40 kohm
	AIC3206_write( 57, 0xc0 );      // CM_1 (common mode) to RADC_M through 40 kohm
	AIC3206_write( 59, 0x5f );      // MIC_PGA_L unmute
	AIC3206_write( 60, 0x5f );      // MIC_PGA_R unmute
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 81, 0xc0 );      // Powerup Left and Right ADC
	AIC3206_write( 82, 0x00 );      // Unmute Left and Right ADC

	AIC3206_write( 0,  0x00 );
	C55x_delay_msec( 2 );           // Wait 2 msec

	/* I2S settings */
	I2S2_SRGR = 0x0015;		//配置I2SRATE寄存器，配置bit clk,frame clk,010_101,bit 1.536M,frame 48K
	//I2S2_ICMR = 0x0028;    // Enable interrupts XMITST RCVST 立体声发送和接受中断
	I2S2_CR   = 0x8012;    // 16-bit word, Master, enable I2C
	C55x_delay_msec( 100 );
	return 1;
}
