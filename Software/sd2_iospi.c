#include "sd2_iospi.h"
#include "main.h"
#include "sd_type.h"

/*�������������������������
  �����ϵ�znFAT--Ƕ��ʽFAT32�ļ�ϵͳ�����ʵ�֡�
	 һ��[���²�]�Ѿ��ɱ�����������ʽ���淢�С�
	 ��������������3���ʱ��Ǳ�ı��������ֽ�������Ψ
	 һһ�׽���FAT32�ļ�ϵͳ��SD����Ƕ��ʽ�洢������
	 ר�������л������˴����ı�̼��������ϵĿ������顣
	 ���ڸ�����������ƽ̨������znFAT�������ɹ���
	 ��ȫ���������Ҳ���ۡ�
	 ���ϵ�ZN-X�����壬֧��51��AVR��STM32(M0/M3/M4)��
	 CPU���˰���������ף����ϸ��־��ʵ�ʵ��ʵ�������
	 ������վwww.znmcu.cn
  �������������������������*/
	
/***************************************************************************************
 �����ģ�飺������ZN-X�����塿�ϡ�SD��2��IOģ��SPI  ��STM32����:STM32F103RBT6��
 �﹦��������IOģ��SPI���ɵ���SPIͨ���ٶ�
 �����׽̳���ο����ϣ�
   �����ϵ�znFAT--Ƕ��ʽFAT32�ļ�ϵͳ�����ʵ�顷һ�� �²� ��11�¡�SD������������
	 �����ϵĵ�Ƭ���߼�����ϵ����Ƶ�̡̳�֮��SD��ר����
****************************************************************************************/

UINT8 Low_or_High2=0; //��������SPIΪ���ٻ��Ǹ���
                     //���˱���Ϊ1������IOģ��SPIʱͨ������delay�������ٶ�
										 //DELAY_TIMEԽ���ٶ�Խ��
										 //SPI�л��ٶȣ���Ҫ����ΪSD���ڳ�ʼ��ʱҪ��ϵ͵�ͨ���ٶ�
										 //����ٶȹ��ߣ��������ɳ�ʼ��ʧ�ܡ���ʼ���ɹ�֮����Ҫ
										 //�������SPI�ٶȣ������������д���ٶ�

void delay_us(u32 us)
{
    u32 time=us;   
    while(--time);  
}

/***********************************************************************
 - ����������������ZN-X�����塿�ϡ�SD��2��IOģ��SPI�ӿ�IO�������ʼ��
 - ����˵������
 - ����˵����0
 ***********************************************************************/

UINT8 SD2_IOSPI_Init(void) //��SPI IO�йس�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_11 |GPIO_Pin_12);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        GPIO_SetBits(GPIOD, GPIO_Pin_2);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************
 - ����������������ZN-X�����塿�ϡ�SD��2��IOģ��SPI�ֽڶ�д
 - ����˵����x:Ҫ���͵��ֽ�
 - ����˵������ȡ�����ֽ�
 ***********************************************************************/
 
UINT8 SD2_IOSPI_RWByte(UINT8 x) //IOģ��SPI
{
 UINT8 rbyte=0;	
	
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 if (x&0x80) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x80)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x80;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);

 if (x&0x40) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x40)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x40;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);

 if (x&0x20) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x20)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x20;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);

 if (x&0x10) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x10)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x10;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 if (x&0x08) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x08)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x08;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 if (x&0x04) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x04)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x04;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 if (x&0x02) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x02)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x02;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 if (x&0x01) SET_SPI_SI_PIN_HIGH(); else SET_SPI_SI_PIN_LOW();
 //SET_SPI_SI_PIN((x&0x01)?1:0);
 SET_SPI_SCL_PIN_LOW(); 
 if(Low_or_High2) delay_us(DELAY_TIME);
 if(GET_SPI_SO_PIN()) rbyte|=0x01;
 SET_SPI_SCL_PIN_HIGH();
 if(Low_or_High2) delay_us(DELAY_TIME);
 
 return rbyte;
}