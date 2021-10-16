
#ifndef __DSO_H
#define __DSO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
#include "arm_math.h"

#define     SAMPLE_DEPTH              1024                  //�洢���512 * 2�ֽڣ�short��
#define     WAVE_BUFF_SIZE            (SAMPLE_DEPTH*2) 

#define     SPSMAXGRADE                10                   // �����ʵ�λ 0-10,    ��5usΪ��λ1

typedef struct {                
    uint16_t MaxValue;
    uint16_t MinValue;
    uint16_t PPValue;
    double Freq;
    signed long Period;
}WAVEPARAMS;

typedef struct {                
    signed long SPS;
    signed long TIMEBASE;
}DSO_MATCH_2PARAMS;

extern arm_cfft_radix4_instance_f32 scfft;
extern const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1];

void CopyDataToWaveBuff(void);
void FFT_GetFreq(signed long SPS);

#ifdef __cplusplus
}
#endif

#endif


