#ifndef _SDX2_H_
#define _SDX2_H_

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
 �����ģ�飺������ZN-X�����塿�ϡ�SD��2����������  ��STM32����:STM32F103RBT6��
 �﹦��������ʵ����SD����������д����������д��������������ȡ�������������ȹ���
             ��������֧�ּ������е�SD��������MMC/SD/SDHC
 �����׽̳���ο����ϣ�
   �����ϵ�znFAT--Ƕ��ʽFAT32�ļ�ϵͳ�����ʵ�顷һ�� �²� ��11�¡�SD������������
	 �����ϵĵ�Ƭ���߼�����ϵ����Ƶ�̡̳�֮��SD��ר����
****************************************************************************************/

#include "main.h"
#include "sd_type.h"

//#define SD2_CS PBout(9)
#define SET_SD2_CS_PIN_HIGH()  GPIOC->BSRRL = GPIO_Pin_11
#define SET_SD2_CS_PIN_LOW()  GPIOC->BSRRH = GPIO_Pin_11

#define TRY_TIME 10   //��SD��д������֮�󣬶�ȡSD���Ļ�Ӧ����������TRY_TIME�Σ������TRY_TIME���ж�������Ӧ��������ʱ��������д��ʧ��

//��غ궨��
//-------------------------------------------------------------
#define SD_VER_ERR     0X00
#define SD_VER_MMC     0X01
#define SD_VER_V1      0X02
#define SD_VER_V2      0X03
#define SD_VER_V2HC    0X04

#define INIT_ERROR                  0x01 //��ʼ������
#define INIT_CMD0_ERROR             0x02 //CMD0����
#define INIT_CMD1_ERROR             0x03 //CMD1����
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41����
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41����

#define WRITE_CMD24_ERROR           0x06 //д��ʱ����CMD24����
#define WRITE_BLOCK_ERROR           0x07 //д�����

#define READ_BLOCK_ERROR            0x08 //�������

#define WRITE_CMD25_ERROR           0x09 //���������дʱ����CMD25����
#define WRITE_NBLOCK_ERROR          0x0A //�������дʧ��

#define READ_CMD18_ERROR            0x0B //����������ʱ����CMD18����
 
#define GET_CSD_ERROR               0x0C //��CSDʧ��

//-------------------------------------------------------------
UINT8 SD2_Init(void); //SD����ʼ��

UINT8 SD2_Write_Sector(UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д���ַΪaddr��������
UINT8 SD2_Read_Sector(UINT32 addr,UINT8 *buffer);	 //�ӵ�ַΪaddr�������ж�ȡ���ݵ�buffer���ݻ�������
UINT8 SD2_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д����ʼ��ַΪaddr��nsec������������
UINT8 SD2_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д����ʼ��ַΪaddr��nsec������������
UINT8 SD2_Erase_nSector(UINT32 addr_sta,UINT32 addr_end);
UINT32 SD2_GetTotalSec(void); //��ȡSD������������

#endif
