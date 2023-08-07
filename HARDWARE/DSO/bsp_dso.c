#include "math.h"
#include "stdbool.h"
#include "usart.h"
#include "arm_math.h"
#include "adc/bsp_adc.h"
#include "log/bsp_log.h"
#include "dso/bsp_dso.h"

float Rec_door[17]={
	0.5,0.5,0.39,0.46,0.32,0.32,0.32,0.32,0.32,0.32,0.25,0.23,0.23,0.23,0.23,0.192,0.18
};

uint16_t WaveBuffer[WAVE_BUFF_SIZE];    //�������ݻ���
float fft_inputbuf[WAVE_BUFF_SIZE];	    //FFT��������
float fft_outputbuf[WAVE_BUFF_SIZE/2];	//FFT�������
static uint16_t ADC_DMA_Pos;            //��ȡDMA��ǰλ��
static uint16_t DMA_Pos_In_Array;       //��¼���������±�λ��

//�����ź���������Ϊһ��fft
float maxima_y[20];//���м���ֵ
int maxima_x[20];//���м���ֵ���±�
int maxCount;//����ֵ����

float harmonic_y[20];//����ֵʵ�ʷ�ֵ
int harmonic_x[20];//����ֵ��ʵ��Ƶ��
float max1_y,max2_y; // ���ֵ��ֵ���δ�ֵ��ֵ
int max1_x,max2_x;   // ����ֵ�±ꡢ�δ�ֵ�±�

//�����ź���������Ϊʮ��fft
extern int times;//ִ�еĴ�����0Ϊ��ʼ�ı�־

/*waveType=1���� waveType=2���ǲ� waveType=3���Ҳ� waveType=4�����,Ĭ�ϵ�ƵΪwave1*/
int wave1Type=4;//wave1���ͣ�Ĭ��ΪNULL
int wave2Type=4;//ͬ��
int wave1Freq;//wave1Ƶ��
int wave2Freq;//ͬ��

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
 * @description		: �ҳ���ֵ����ֵ����ȥ0-19k֮��ļ���ֵ��
 * @param - 	    : arr[]---��������
                    size----����ߴ�
                    maxCount----��¼����ֵ����
                    SPS---�����ֱ���
 * @return 			: ��
 ******************************************************/
void GetMax(float arr[], int size, int *maxCount,signed long SPS)
{
	  int i;
	  int HPF_pos_19=19000*2048/SPS;//�˵�19k���µļ���ֵ
	  int BRF_pos_61=61000*2048/SPS;//�˵�19k-61k���ڷ�ֵ����0.3�ļ���ֵ
	
    float find_door=6;//findmaxima_door_value
    //the deciding pointer
    *maxCount=0;

    //set the first value as the reference
    float prevValue=arr[HPF_pos_19];

    //Iterate over arr[] to find the maxima
    for(i=HPF_pos_19;i<size-1;i++)
    {
        if(arr[i]>(prevValue+find_door)&&arr[i]>(arr[i+1]+find_door))
        {
					  float temp=arr[i]/512;//���㼫��ֵ��ֵ
					  if(i<BRF_pos_61)
						{
							if(temp<0.3f) continue;
							else
							{
								maxima_y[*maxCount]=arr[i];
                maxima_x[*maxCount]=i;
								(*maxCount)++;
							}
						}
						else
						{
                maxima_y[*maxCount]=arr[i];
                maxima_x[*maxCount]=i;
                (*maxCount)++;
						}
        }
        prevValue=arr[i];
    }

    //decide the last value of arr[]
    if(arr[size-1]>(prevValue+find_door))
    {
        maxima_y[*maxCount]=arr[size-1];
        maxima_x[*maxCount]=size-1;
        (*maxCount)++;
    }
}

///******************************************************
// * @description		: ������룬�˳����ظ����ֵ�����
// * @param - 	    : input[]---��������
//                    isNonCommon[]---���״̬����
//                    size---�������
// * @return 			: ��
// ******************************************************/
//void keepNonCommonElements(int input[], bool isNonCommon[], int size, int maxSize) {
//    bool isDigitPresent[20] = {false};

//    for (int i = 0; i < size; i++) {
//        isDigitPresent[input[i]] = true;
//    }

//    for (int i = 0; i < 20; i++) {
//        if (isDigitPresent[i]) {
//            isNonCommon[i] = false;
//        } else {
//            isNonCommon[i] = isNonCommon[i] && true;
//        }
//    }
//}
/******************************************************
 * @description		: ��ʾ�ҳ��ķ�ֵ����ֵ����ȥ0-19k֮��ļ���ֵ��
 * @param - 	    : arr[]---��������
                    SPS---�����ֱ���
 * @return 			: ��
 ******************************************************/
void show_Maxima(float arr[],signed long SPS)
{
    int i;
	
	  //���ø������
	  for(int i=0;i<maxCount;i++)
	{
		maxima_y[i]=0;//���м���ֵ
    maxima_x[i]=0;//���м���ֵ���±�
    harmonic_y[i]=0;//����ֵʵ�ʷ�ֵ
    harmonic_x[i]=0;//����ֵ��ʵ��Ƶ��
	}
	  maxCount=0;
	
    GetMax(arr,WAVE_BUFF_SIZE/4,&maxCount,SPS);
	  printf("�ܹ�%d��\r\n",maxCount);
    if(maxCount>0)
    {
        for(i=0;i<maxCount;i++)
        {
					harmonic_y[i]=maxima_y[i]/512;//fft��ֵ�������
					harmonic_x[i]=(int)round((double)SPS/2048*maxima_x[i]/1000/5)*5;//��������У����У��Ϊ5�ı���
					printf("���м���ֵmagnitude:%f and frequency: %dkHz\r\n",harmonic_y[i],harmonic_x[i]);
        }
    }
    else
    {
       printf("NO FOUND\r\n");
    }
}

/******************************************************
 * @description		: �ҳ�����������ֵ
 * @param - 	    : ��
 * @return 			: ��
 ******************************************************/
void findTopTwo()
{
    max1_y = harmonic_y[0]; // ���ֵ
    max2_y = harmonic_y[1]; // �δ�ֵ
	
    if (harmonic_y[0] > harmonic_y[1]) {
        max1_y = harmonic_y[0];
        max2_y = harmonic_y[1];
    } else {
        max1_y = harmonic_y[1];
        max2_y = harmonic_y[0];
    }

    for (int i = 2; i < maxCount; i++) {
        if (harmonic_y[i] > max1_y) {
            max2_y = max1_y;
            max1_y = harmonic_y[i];
        } else if (harmonic_y[i] > max2_y && harmonic_y[i] != max1_y) {
            max2_y = harmonic_y[i];
        }
    }
		
		printf("���ļ���ֵ\r\n");
		for(int i=0;i<maxCount;i++)
		{
			if(max1_y==harmonic_y[i])
			{
				max1_x=harmonic_x[i];
				printf("One:magnitude:%f and frequency: %dkHz\r\n",max1_y,max1_x);
			}
			if(max2_y==harmonic_y[i])
			{
			  max2_x=harmonic_x[i];
				printf("Two:magnitude:%f and frequency: %dkHz\r\n",max2_y,max2_x);
			}
		}
	}
/******************************************************
 * @description		: ʶ���źŲ���Ƶ��
 * @param - 	    : ��
 * @return 			  : ��
 ******************************************************/
void Rec_waveFreq()
{
	if((max1_y-max2_y)>0.6f)//��������ͬƵ
	{
		wave1Freq=wave2Freq=max1_x;
	}
	else
	{
		wave1Freq=(max1_x<max2_x) ? max1_x : max2_x;//��Ƶ
		wave2Freq=(max1_x<max2_x) ? max2_x : max1_x;//��Ƶ
	}
	printf("wave1:%dkHz\r\n",wave1Freq);
	printf("wave2:%dkHz\r\n",wave2Freq);
}

/******************************************************
 * @description		: ʶ���źŲ�������,�ж��������г�������������ǲ�
 * @param - SPS	    : ��
 * @return 			: ��
 ******************************************************/
void Rec_waveType1(int *wave1Sin_Times,int *wave2Sin_Times)
{
    int temp1=3*wave1Freq;
	  //int temp2=5*wave1Freq;
		int temp3=3*wave2Freq;
	  //int temp4=5*wave2Freq;
		
//	  //��������������10��0
//	  if((wave1Sin_Times>10) wave1Sin_Times=0;
//	  if(wave2Sin_Times>10) wave2Sin_Times=0;
//	
    bool foundInArray1=false;
	  bool foundInArray2=false;
    for (int i=0;i<maxCount;i++) {
        if (temp1 == harmonic_x[i]) {
            foundInArray1 = true;
            break;
        }
   }
		for (int i=0;i<maxCount;i++) {
        if (temp3 == harmonic_x[i]) {
            foundInArray2 = true;
            break;
        }
    }
		//���жϵ����ͽ��м��������ж����յĲ���
		if (foundInArray1) {
        wave1Type = 2;
			  
    } else {
        wave1Type = 3;
			  (*wave1Sin_Times)++;
    }
			
		if (foundInArray2) {
        wave2Type = 2;
			  
    } else {
        wave2Type = 3;
			  (*wave2Sin_Times)++;
    }
		printf("wave1Type:%d\r\n",wave1Type);
		printf("wave2Type:%d\r\n",wave2Type);
}
/******************************************************
 * @description		: ʶ���źŲ�������,�ж��������г�������������ǲ�
 * @param - SPS	    : ��
 * @return 			: ��
 ******************************************************/
void Rec_waveType2(int *wave1Sin_Times,int *wave2Sin_Times)
{
		float wave1_A;
		float wave2_A;
	  //��������������10��0
//	  if(wave1Sin_Times>10) wave1Sin_Times=0;
//	  if(wave2Sin_Times>10) wave2Sin_Times=0;
	
		for(int i=0;i<maxCount;i++)
		{
			 if(wave1Freq==harmonic_x[i])
			 {
				 wave1_A=harmonic_y[i];
			 }
			 if(wave2Freq==harmonic_x[i])
			 {
				 wave2_A=harmonic_y[i];
			 }
		}
		
		if(wave1_A>Rec_door[(wave1Freq-20)/5])
		{
			wave1Type=3;
			(*wave1Sin_Times)++;
		}
		else wave1Type=2;
		
		if(wave2_A>Rec_door[(wave2Freq-20)/5])
		{
			wave2Type=3;
			(*wave2Sin_Times)++;
		}
		else wave2Type=2;
	
		printf("wave1Type:%d\r\n",wave1Type);
		printf("wave2Type:%d\r\n",wave2Type);
}
/******************************************************
 * @description		: ���ź�FFT��ȷ�������ź�Ƶ��
 * @param - SPS	    : ADC������
 * @return 			: ��
 ******************************************************/
void FFT(signed long SPS,int *wave1Sin_Times,int *wave2Sin_Times)
{
    int i;
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
		show_Maxima(fft_outputbuf,SPS);
		findTopTwo();
		Rec_waveFreq();//ʶ��Ƶ��
		//Rec_waveType1(wave1Sin_Times,wave2Sin_Times);//fftƵ��ʶ����
		Rec_waveType2(wave1Sin_Times,wave2Sin_Times);//fft��ֵʶ����
//    for(i=0; i < SAMPLE_DEPTH/2+1; i++)    //��������Ӧ�����±�
//    {
//        if(max < fft_outputbuf[i+2])	
//        {
//            max = fft_outputbuf[i+2];	
//            pos = i+2;
//        }
//    }
    
//    WaveParams.Freq = (double)SPS / 2048 * pos;
//    LOG_INFO("======pos: %d fre: %f\r\n", pos, WaveParams.Freq);
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
    //LOG_INFO("WaveParams.PPValue = %f\r\n", WaveParams.PPValue*(3.3f/4095.0f));
}
