#ifndef _SD2_IOSPI_H_
#define _SD2_IOSPI_H_

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
 �����ģ�飺������ZN-X�����塿�ϡ�SD��1��IOģ��SPI  ��STM32����:STM32F103RBT6��
 �﹦��������IOģ��SPI���ɵ���SPIͨ���ٶ�
 �����׽̳���ο����ϣ�
   �����ϵ�znFAT--Ƕ��ʽFAT32�ļ�ϵͳ�����ʵ�顷һ�� �²� ��11�¡�SD������������
	 �����ϵĵ�Ƭ���߼�����ϵ����Ƶ�̡̳�֮��SD��ר����
****************************************************************************************/

#include "sd_type.h"

//====================Ŀ��ƽ̨�û�����==============

//��IO����������ĺ�����
//#define SET_SPI_SCL_PIN(val) PBout(13)=val //����SPI��ʱ��
//#define SET_SPI_SI_PIN(val)  PBout(15)=val //����SPI����������
//#define GET_SPI_SO_PIN()     PBin(14)      //��ȡSPI����ӳ�
#define GET_SPI_SO_PIN()        (GPIOC->IDR & GPIO_Pin_8)
#define SET_SPI_SI_PIN_HIGH()   GPIOD->BSRRL = GPIO_Pin_2
#define SET_SPI_SI_PIN_LOW()    GPIOD->BSRRH = GPIO_Pin_2
#define SET_SPI_SCL_PIN_HIGH()  GPIOC->BSRRL = GPIO_Pin_12
#define SET_SPI_SCL_PIN_LOW()   GPIOC->BSRRH = GPIO_Pin_12
   
#define DELAY_TIME 500 //SPI����ʱ��������ֵԽ��SPI�ٶ�Խ��

UINT8 SD1_IOSPI_Init(void); //IOģ��SPI��س�ʼ��
UINT8 SD1_IOSPI_RWByte(UINT8 x); //IOģ��SPI�ֽڶ�д

#endif