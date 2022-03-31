/*
 * fir.c
 *
 *  Created on: 2022年3月25日
 *      Author: Tyler
 */

#include <stdio.h>
#include "audio_common.h"
#include "fir.h"

const Int8 FIR_LPF[ORDER+1] ={      -3,   -2,   -1,    1,    3,    6,    8,   11,   13,   14,   15,   15,
        14,   13,   11,    8,    6,    3,    1,   -1,   -2,   -3};
//const float FIR_LPF[ORDER+1] = {  8.258927119e-05,-0.0008808543207, 0.003858669195,-0.009028311819, 0.008625555784,
 //       0.01996053942,   -0.125576809,   0.6029586196,   0.6029586196,   -0.125576809,
 //       0.01996053942, 0.008625555784,-0.009028311819, 0.003858669195,-0.0008808543207,
  //    8.258927119e-05};
static Uint16 i,j;
/*
C语言实现FIR滤波器-时域暴力卷积方法
y(n) = E(m=0-&gt;N-1) x(m)*h(n-m);
p_input : 待滤波数据
p_output ： 滤波输出数据
data_len ： 数据长度
p_fir_para ： h(t)参数
fir_para_len ： h(t)长度
*/
void c_fir(Int16* p_input, Int16* p_output, Uint16 data_len, Int8* p_fir_para,Uint16 fir_para_len)
{
    //memset(p_output, 0, sizeof(float)  *data_len);  //输出清零
    long Temp;
    for (i = 0; i < data_len; i+=2)
    {
        Temp = 0;
        if (i < fir_para_len*2)
        {
            for (j = 0; j <= i/2; j++)
            {
                Temp += (long)p_fir_para[j]  * p_input[i - j*2];
            }
        }
        else if(i >= fir_para_len*2)
        {
            for (j = 0; j < fir_para_len; j++)
            {
                Temp += (long)p_fir_para[j] * p_input[i - j*2];  //卷积和
            }

        }
        Temp = Temp>>7;
        p_output[i]=Temp;
    }
}

void filter(Int16* p_input, Int16* p_output, Uint16 data_len, Int8* p_fir_para,Uint16 fir_para_len)
{
	Int16 Temp = 0,i,j;
	for(i=0;i<data_len;i+=2)
	{
		Temp = 0;
		for(j=0;(j<fir_para_len)&&(j<i/2);j++)
			Temp += p_fir_para[j]*p_input[i-j*2];
		p_output[i] = Temp;
	}
}

