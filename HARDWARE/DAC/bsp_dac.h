
#ifndef __BSP_DAC_H
#define __BSP_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

#define   DAC_DHR12R1_ADDR  0x40007408                           // DMA writes into this reg on every request               
#define   OUT_FREQ          1000                                 // Output waveform frequency
#define   SINE_RES          128                                  // Waveform resolution
#define   CNT_FREQ          84000000                             // TIM6 counter clock (prescaled APB1)
#define   TIM_PERIOD        ((CNT_FREQ)/((SINE_RES)*(OUT_FREQ))) // Autoreload reg value

void DAC_Mode_Init(void);
uint32_t SetDACFreq(uint32_t Freq);


#ifdef __cplusplus
extern "C" {
#endif

#endif

