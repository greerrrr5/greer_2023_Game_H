
#include "dso.h"
#include "math.h"
#include "usart.h"
#include "arm_math.h"
#include "adc/bsp_adc.h"
#include "log/bsp_log.h"

uint16_t WaveBuffer[WAVE_BUFF_SIZE];    //�������ݻ���
float fft_inputbuf[WAVE_BUFF_SIZE];	    //FFT��������
float fft_outputbuf[WAVE_BUFF_SIZE/2];	//FFT�������
static uint16_t ADC_DMA_Pos;            //��ȡDMA��ǰλ��
static uint16_t DMA_Pos_In_Array;       //��¼���������±�λ��
arm_cfft_radix4_instance_f32 scfft;

WAVEPARAMS WaveParams ={0, 4095, 0, 0, 0};
const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1] =
{
    //5usΪ��λ1
    {1000000, 5}, //0  ,50us
    {500000, 10},
    {200000, 20},
    {100000, 50},
    {50000, 100},
    {20000, 200}, //5
    {10000, 500},
    {5000, 1000},
    {2000, 2000},
    {1000, 5000},
    {500, 10000}
};

/******************************************************
 * @description		: �������Ƶ�ʴ�
 * @param - SPS	    : ADC������
 * @return 			: ��
 ******************************************************/
void FFT_GetFreq(signed long SPS)
{
    int i;
    int pos;
    float max;
    float ADC_temp;
    
    //FFT���ݴ���
    for(i=0; i<SAMPLE_DEPTH; i++)
    {
        ADC_temp = ((float)WaveBuffer[i]*(3.3f/4095.0f));
        fft_inputbuf[2*i] = ADC_temp;
		fft_inputbuf[2*i+1] = 0;
    }
    
    arm_cfft_radix4_f32(&scfft, fft_inputbuf);	                    //FFT���㣨��4��
    arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf, SAMPLE_DEPTH);	//��������������ģ�÷�ֵ 
    
    for(i=0; i < SAMPLE_DEPTH/2+1; i++)    //��������Ӧ�����±�
    {
        if(max < fft_outputbuf[i+2])	
        {
            max = fft_outputbuf[i+2];	
            pos = i+2;
        }
    }
    
    WaveParams.Freq = (double)SPS / 2048 * pos;
    LOG_INFO("======pos: %d fre: %f\r\n", pos, WaveParams.Freq);
}

/******************************************************
 * @description		: ����ADC����
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void CopyDataToWaveBuff(void)
{
    uint16_t i, j;
    /* �������ݵ�WaveBuffer��ͬʱ����DMA���츲���������� */
    /* ��ȡADC_DMA��ǰλ�� */
    ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(DMA2_Stream0);

    LOG_DEBUG("ADC_DMA��ǰλ��1 = %d\n",ADC_DMA_Pos);

    if (ADC_DMA_Pos < SAMPLE_DEPTH)
    {
        /* ����У׼��������ʱ��λ�� */
        DMA_Pos_In_Array = SAMPLE_DEPTH - ADC_DMA_Pos;

        LOG_DEBUG("pos = %d\n",(ADC_DataSize - DMA_Pos_In_Array));
        for (j = 0, i = ADC_DataSize - DMA_Pos_In_Array; i < ADC_DataSize; i++, j++)
        {
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
        for (i = 0; i < ADC_DMA_Pos; i++, j++)
        {
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
    }
    else
    {
        for (j = 0, i = ADC_DMA_Pos - SAMPLE_DEPTH; i < ADC_DMA_Pos; i++, j++)
        {  
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
    }

    /* ��WaveBuffer��һЩ����----���ֵ */
    WaveParams.MaxValue = 0;
    WaveParams.MinValue = 4096;
    WaveParams.PPValue = 0;
    for (i = 0; i < WAVE_BUFF_SIZE/2; i++)
    {
        WaveParams.MaxValue = (WaveBuffer[i] > WaveParams.MaxValue) ? WaveBuffer[i] : WaveParams.MaxValue;
        WaveParams.MinValue = (WaveBuffer[i] < WaveParams.MinValue) ? WaveBuffer[i] : WaveParams.MinValue;
    }
    WaveParams.PPValue = WaveParams.MaxValue - WaveParams.MinValue;
    LOG_INFO("WaveParams.PPValue = %f\r\n", WaveParams.PPValue*(3.3f/4095.0f));
}
