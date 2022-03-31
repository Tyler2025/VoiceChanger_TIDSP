/*
 * fir.h
 *
 *  Created on: 2022Äê3ÔÂ25ÈÕ
 *      Author: Tyler
 */

#ifndef FIR_H_
#define FIR_H_

#define ORDER 21
void c_fir(Int16* p_input, Int16* p_output, Uint16 data_len, Int8* p_fir_para,Uint16 fir_para_len);
void filter(Int16* p_input, Int16* p_output, Uint16 data_len, Int8* p_fir_para,Uint16 fir_para_len);



#endif /* FIR_H_ */
