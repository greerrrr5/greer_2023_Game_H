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

uint16_t WaveBuffer[WAVE_BUFF_SIZE];    //波形数据缓存
float fft_inputbuf[WAVE_BUFF_SIZE];	    //FFT输入数组
float fft_outputbuf[WAVE_BUFF_SIZE/2];	//FFT输出数组
static uint16_t ADC_DMA_Pos;            //获取DMA当前位置
static uint16_t DMA_Pos_In_Array;       //记录波形数组下标位置

//下列信号重置周期为一次fft
float maxima_y[20];//所有极大值
int maxima_x[20];//所有极大值的下标
int maxCount;//极大值个数

float harmonic_y[20];//极大值实际幅值
int harmonic_x[20];//极大值的实际频率
float max1_y,max2_y; // 最大值幅值、次大值幅值
int max1_x,max2_x;   // 最大幅值下标、次大值下标

//下列信号重置周期为十次fft
extern int times;//执行的次数，0为开始的标志

/*waveType=1方波 waveType=2三角波 waveType=3正弦波 waveType=4无输出,默认低频为wave1*/
int wave1Type=4;//wave1类型，默认为NULL
int wave2Type=4;//同上
int wave1Freq;//wave1频率
int wave2Freq;//同上

arm_cfft_radix4_instance_f32 scfft;

WAVEPARAMS WaveParams ={0, 4095, 0, 0, 0};
const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1] =
{
    //5us为单位1
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
 * @description		: 找出幅值极大值（除去0-19k之间的极大值）
 * @param - 	    : arr[]---输入数组
                    size----数组尺寸
                    maxCount----记录极大值个数
                    SPS---采样分辨率
 * @return 			: 无
 ******************************************************/
void GetMax(float arr[], int size, int *maxCount,signed long SPS)
{
	  int i;
	  int HPF_pos_19=19000*2048/SPS;//滤掉19k以下的极大值
	  int BRF_pos_61=61000*2048/SPS;//滤掉19k-61k以内幅值低于0.3的极大值
	
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
					  float temp=arr[i]/512;//计算极大值幅值
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
// * @description		: 多次输入，滤出不重复出现的数字
// * @param - 	    : input[]---输入数组
//                    isNonCommon[]---输出状态数组
//                    size---输入次数
// * @return 			: 无
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
 * @description		: 显示找出的幅值极大值（除去0-19k之间的极大值）
 * @param - 	    : arr[]---输入数组
                    SPS---采样分辨率
 * @return 			: 无
 ******************************************************/
void show_Maxima(float arr[],signed long SPS)
{
    int i;
	
	  //重置各项参数
	  for(int i=0;i<maxCount;i++)
	{
		maxima_y[i]=0;//所有极大值
    maxima_x[i]=0;//所有极大值的下标
    harmonic_y[i]=0;//极大值实际幅值
    harmonic_x[i]=0;//极大值的实际频率
	}
	  maxCount=0;
	
    GetMax(arr,WAVE_BUFF_SIZE/4,&maxCount,SPS);
	  printf("总共%d个\r\n",maxCount);
    if(maxCount>0)
    {
        for(i=0;i<maxCount;i++)
        {
					harmonic_y[i]=maxima_y[i]/512;//fft幅值结果计算
					harmonic_x[i]=(int)round((double)SPS/2048*maxima_x[i]/1000/5)*5;//进行数据校正，校正为5的倍数
					printf("所有极大值magnitude:%f and frequency: %dkHz\r\n",harmonic_y[i],harmonic_x[i]);
        }
    }
    else
    {
       printf("NO FOUND\r\n");
    }
}

/******************************************************
 * @description		: 找出最大的两个幅值
 * @param - 	    : 无
 * @return 			: 无
 ******************************************************/
void findTopTwo()
{
    max1_y = harmonic_y[0]; // 最大值
    max2_y = harmonic_y[1]; // 次大值
	
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
		
		printf("最大的极大值\r\n");
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
 * @description		: 识别信号波形频率
 * @param - 	    : 无
 * @return 			  : 无
 ******************************************************/
void Rec_waveFreq()
{
	if((max1_y-max2_y)>0.6f)//两个波形同频
	{
		wave1Freq=wave2Freq=max1_x;
	}
	else
	{
		wave1Freq=(max1_x<max2_x) ? max1_x : max2_x;//低频
		wave2Freq=(max1_x<max2_x) ? max2_x : max1_x;//高频
	}
	printf("wave1:%dkHz\r\n",wave1Freq);
	printf("wave2:%dkHz\r\n",wave2Freq);
}

/******************************************************
 * @description		: 识别信号波形类型,判断有无奇次谐波，有则是三角波
 * @param - SPS	    : 无
 * @return 			: 无
 ******************************************************/
void Rec_waveType1(int *wave1Sin_Times,int *wave2Sin_Times)
{
    int temp1=3*wave1Freq;
	  //int temp2=5*wave1Freq;
		int temp3=3*wave2Freq;
	  //int temp4=5*wave2Freq;
		
//	  //两个技术参数过10清0
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
		//对判断的类型进行计数用于判断最终的波形
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
 * @description		: 识别信号波形类型,判断有无奇次谐波，有则是三角波
 * @param - SPS	    : 无
 * @return 			: 无
 ******************************************************/
void Rec_waveType2(int *wave1Sin_Times,int *wave2Sin_Times)
{
		float wave1_A;
		float wave2_A;
	  //两个技术参数过10清0
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
 * @description		: 对信号FFT，确定分离信号频率
 * @param - SPS	    : ADC采样率
 * @return 			: 无
 ******************************************************/
void FFT(signed long SPS,int *wave1Sin_Times,int *wave2Sin_Times)
{
    int i;
    float ADC_temp;
    
    //FFT数据处理
    for(i=0; i<SAMPLE_DEPTH; i++)
    {
        ADC_temp = ((float)WaveBuffer[i]*(3.3f/4095.0f));
        fft_inputbuf[2*i] = ADC_temp;
		    fft_inputbuf[2*i+1] = 0;
    }
    
    arm_cfft_radix4_f32(&scfft, fft_inputbuf);	                    //FFT计算（基4）
    arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf, SAMPLE_DEPTH);	//把运算结果复数求模得幅值 
		show_Maxima(fft_outputbuf,SPS);
		findTopTwo();
		Rec_waveFreq();//识别频率
		//Rec_waveType1(wave1Sin_Times,wave2Sin_Times);//fft频率识别波形
		Rec_waveType2(wave1Sin_Times,wave2Sin_Times);//fft幅值识别波形
//    for(i=0; i < SAMPLE_DEPTH/2+1; i++)    //幅度最大对应数组下标
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
 * @description		: 拷贝ADC数据
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void CopyDataToWaveBuff(void)
{
    uint16_t i, j;
    /* 拷贝数据到WaveBuffer，同时避免DMA过快覆盖数组数据 */
    /* 获取ADC_DMA当前位置 */
    ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(DMA2_Stream0);

    LOG_DEBUG("ADC_DMA当前位置1 = %d\n",ADC_DMA_Pos);

    if (ADC_DMA_Pos < SAMPLE_DEPTH)
    {
        /* 用于校准拷贝数组时的位置 */
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

    /* 对WaveBuffer做一些处理----峰峰值 */
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
