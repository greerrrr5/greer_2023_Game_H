
#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

#define    ADC_DataSize                  2000

extern volatile uint16_t ADC_ConvertedValue[ADC_DataSize];

void ADC_Mode_Init(void);
void SetADCSampleRate(uint32_t SampleRate);

#ifdef __cplusplus
extern "C" {
#endif

#endif

