#include "sdx2.h"
#include "sd_type.h"
#include "sd2_iospi.h"

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

//��������
//--------------------------------------------------------------
extern UINT8 Low_or_High2; //��IOSPI�ж���

UINT8 SD2_Addr_Mode=0; //SD2��Ѱַ��ʽ��1Ϊ��Ѱַ��0Ϊ�ֽ�Ѱַ
UINT8 SD2_Ver=SD_VER_ERR; //SD��2�İ汾
//---------------------------------------------------------------

#define SD2_SPI_SPEED_HIGH() Low_or_High2=0

#define SD2_SPI_SPEED_LOW()  Low_or_High2=1

#define SD2_SPI_WByte(x) SD2_IOSPI_RWByte(x)

#define SD2_SPI_RByte()  SD2_IOSPI_RWByte(0XFF)

/********************************************************************
 - ����������������ZN-X�����塿�ϡ�SD��2��SPI�ӿڳ�ʼ��
 - ����˵������
 - ����˵����0
 - ע��SPI�ӿڳ�ʼ�������ȹ����ڵ���ģʽ��SD���ڳ�ʼ���Ĺ�����Ҫ��
       SPI�ٶ�Ҫ�Ƚϵͣ�ԭ���ϲ�����400KHZ������ֵΪ240KHZ���������
			 SD����ʼ�����ɹ������ɼ�������SPI�ٶȣ�ʵ���ٶ�����ڵ�·��SD
			 ��Ʒ�ʡ�
 ********************************************************************/

UINT8 SD2_SPI_Init(void)
{
 SD2_IOSPI_Init(); //SPI�ӿڳ�ʼ��
	
 return 0;
}

/******************************************************************
 - ������������SD��д����
 - ����˵����SD����������6���ֽڣ�pcmd��ָ�������ֽ����е�ָ��
 - ����˵��������д�벻�ɹ���������0xff
 ******************************************************************/

UINT8 SD2_Write_Cmd(UINT8 *pcmd) 
{
 UINT8 r=0,time=0;
 
 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����8��ʱ�ӣ���߼����ԣ����û�������ЩSD2�����ܲ�֧��   
	
 SET_SD2_CS_PIN_LOW();
 while(0XFF!=SD2_SPI_RByte()); //�ȴ�SD2��׼���ã������䷢������

 //��6�ֽڵ���������д��SD2��
 SD2_SPI_WByte(pcmd[0]);
 SD2_SPI_WByte(pcmd[1]);
 SD2_SPI_WByte(pcmd[2]);
 SD2_SPI_WByte(pcmd[3]);
 SD2_SPI_WByte(pcmd[4]);
 SD2_SPI_WByte(pcmd[5]);
	
 if(pcmd[0]==0X1C) SD2_SPI_RByte(); //�����ֹͣ�������������ֽ�

 do 
 {  
  r=SD2_SPI_RByte();
  time++;
 }while((r&0X80)&&(time<TRY_TIME)); //������Դ�������TRY_TIME�򷵻ش���

 return r;
}

/******************************************************************
 - ����������SD2����ʼ��������ڲ�ͬ��SD2������MMC��SD2��SD2HC����ʼ��
             �����ǲ�ͬ��
 - ����˵������
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/

UINT8 SD2_Init(void)
{
 UINT8 time=0,r=0,i=0;
	
 UINT8 rbuf[4]={0};
	
 UINT8 pCMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0����SD2����Ĭ���ϵ���SD2ģʽ�л���SPIģʽ��ʹSD2������IDLE״̬
 UINT8 pCMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1��MMC��ʹ��CMD1������г�ʼ��
 UINT8 pCMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8�����ڼ���SD2���İ汾�����ɴ�Ӧ���֪SD2���Ĺ�����ѹ
 UINT8 pCMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16������������СΪ512�ֽڣ������������ڳ�ʼ�����֮�������̽�ԵĲ�����
                                                          //��������ɹ���˵����ʼ��ȷʵ�ɹ�
 UINT8 pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD2��������ACMD����Ӧ�ò����� CMD55+ACMD41���ʹ��
                                                          //MMC��ʹ��CMD1�����г�ʼ������SD2����ʹ��CMD55+ACMD41�����г�ʼ��
 UINT8 pACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD2���Ƿ��ʼ����ɣ�MMC���������ô�������2.0��SD2��
 UINT8 pACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD2���Ƿ��ʼ����ɣ�MMC���������ô�������1.0��SD2��

 UINT8 pCMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58�����ڼ���SD22.0������SD2HC��������ͨ��SD2�������߶�������ַ��Ѱַ��ʽ��ͬ
 
 SD2_SPI_Init(); //SPI��س�ʼ����SPI�����ڵ���ģʽ

 SD2_SPI_SPEED_LOW(); //���Ƚ�SPI��Ϊ����
	
 SET_SD2_CS_PIN_HIGH(); 
	
 for(i=0;i<0x0f;i++) //����Ҫ��������74��ʱ���źţ����Ǳ���ģ�����SD��
 {
  SD2_SPI_WByte(0xff); //120��ʱ��
 }

 time=0;
 do
 { 
  r=SD2_Write_Cmd(pCMD0);//д��CMD0
  time++;
  if(time>=TRY_TIME) 
  { 
   return(INIT_CMD0_ERROR);//CMD0д��ʧ��
  }
 }while(r!=0x01);
 
 if(1==SD2_Write_Cmd(pCMD8))//д��CMD8���������ֵΪ1����SD2���汾Ϊ2.0
 {
	rbuf[0]=SD2_SPI_RByte(); rbuf[1]=SD2_SPI_RByte(); //��ȡ4���ֽڵ�R7��Ӧ��ͨ������֪��SD2���Ƿ�֧��2.7~3.6V�Ĺ�����ѹ
	rbuf[2]=SD2_SPI_RByte(); rbuf[3]=SD2_SPI_RByte();
	 
	if(rbuf[2]==0X01 && rbuf[3]==0XAA)//SD2���Ƿ�֧��2.7~3.6V
	{		
	 time=0;
	 do
	 {
		SD2_Write_Cmd(pCMD55);//д��CMD55
		r=SD2_Write_Cmd(pACMD41H);//д��ACMD41�����SD22.0
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV2_ACMD41_ERROR);//��SD22.0ʹ��ACMD41���г�ʼ��ʱ��������
    }
   }while(r!=0);	

   if(0==SD2_Write_Cmd(pCMD58)) //д��CMD58����ʼ����SD22.0
   {
	  rbuf[0]=SD2_SPI_RByte(); rbuf[1]=SD2_SPI_RByte(); //��ȡ4���ֽڵ�OCR������CCSָ������SD2HC������ͨ��SD2
	  rbuf[2]=SD2_SPI_RByte(); rbuf[3]=SD2_SPI_RByte();	

    if(rbuf[0]&0x40) 
		{
		 SD2_Ver=SD_VER_V2HC; //SD2HC��	
		 SD2_Addr_Mode=1; //SD2HC��������Ѱַ��ʽ��������ַ
		}	
    else SD2_Ver=SD_VER_V2; //��ͨ��SD2����2.0�Ŀ�����SD2HC��һЩ��ͨ�Ŀ�				
   }
  }
 }
 else //SD2 V1.0��MMC 
 {
	//SD2��ʹ��ACMD41���г�ʼ������MMCʹ��CMD1�����г�ʼ������������һ���ж���SD2����MMC
	SD2_Write_Cmd(pCMD55);//д��CMD55
	r=SD2_Write_Cmd(pACMD41S);//д��ACMD41�����SD21.0
    
  if(r<=1) //��鷵��ֵ�Ƿ���ȷ�������ȷ��˵��ACMD41������ܣ���ΪSD2��
  {
	 SD2_Ver=SD_VER_V1; //��ͨ��SD21.0����һ����˵�������ᳬ��2G
			
	 time=0;
	 do
	 {
		SD2_Write_Cmd(pCMD55);//д��CMD55
		r=SD2_Write_Cmd(pACMD41S);//д��ACMD41�����SD21.0
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV1_ACMD41_ERROR);//��SD21.0ʹ��ACMD41���г�ʼ��ʱ��������
    }
   }while(r!=0);			 
  }
  else //����ΪMMC	
	{
	 SD2_Ver=SD_VER_MMC; //MMC��������֧��ACMD41�������ʹ��CMD1���г�ʼ��
			
	 time=0;
   do
   { 
    r=SD2_Write_Cmd(pCMD1);//д��CMD1
    time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_CMD1_ERROR);//MMC��ʹ��CMD1������г�ʼ���в�������
    }
   }while(r!=0);			
  }
 }
 
 if(0!=SD2_Write_Cmd(pCMD16)) //SD2���Ŀ��С����Ϊ512�ֽ�
 {
	SD2_Ver=SD_VER_ERR; //������ɹ�����˿�Ϊ�޷�ʶ��Ŀ�
	return INIT_ERROR;
 }	
 
 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����SD2���Ĳ���ʱ�������ﲹ8��ʱ�� 
 
 SD2_SPI_SPEED_HIGH(); //SPI�е�����
 
 return 0;//����0,˵����λ�����ɹ�
}

/******************************************************************
 - ������������SD2�����ɸ��������в����������������е����ݴ󲿷����
             ��Ϊȫ0����Щ��������Ϊȫ0XFF����Ҫʹ�ô˺�������ȷ�ϣ�
 - ����˵����addr_sta����ʼ������ַ   addr_end������������ַ
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/

UINT8 SD2_Erase_nSector(UINT32 addr_sta,UINT32 addr_end)
{
 UINT8 r,time;
 UINT8 i=0;
 UINT8 pCMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //���ò����Ŀ�ʼ������ַ
 UINT8 pCMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //���ò����Ľ���������ַ
 UINT8 pCMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //��������

 if(!SD2_Addr_Mode) {addr_sta<<=9;addr_end<<=9;} //addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD32[1]=addr_sta>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
 pCMD32[2]=addr_sta>>16;
 pCMD32[3]=addr_sta>>8;
 pCMD32[4]=addr_sta;	 

 pCMD33[1]=addr_end>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
 pCMD33[2]=addr_end>>16;
 pCMD33[3]=addr_end>>8;
 pCMD33[4]=addr_end;	

 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD32);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD33);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD38);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);

 return 0; 

}

/****************************************************************************
 - ������������bufferָ���512���ֽڵ�����д�뵽SD2����addr������
 - ����˵����addr:������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD2����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD2_Write_Sector(UINT32 addr,UINT8 *buffer)	//��SD2���е�ָ����ַ������д��512���ֽڣ�ʹ��CMD24��24�����
{  
 UINT8 r,time;
 UINT8 i=0;
 UINT8 pCMD24[]={0x58,0x00,0x00,0x00,0x00,0xff}; //��SD2���е����飨512�ֽڣ�һ��������д�����ݣ���CMD24

 if(!SD2_Addr_Mode) addr<<=9; //addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD24[1]=addr>>24; //���ֽڵ�ַд�뵽CMD24�ֽ�������
 pCMD24[2]=addr>>16;
 pCMD24[3]=addr>>8;
 pCMD24[4]=addr;	

 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD24);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0); 

 while(0XFF!=SD2_SPI_RByte()); //�ȴ�SD2��׼���ã������䷢���������������
	
 SD2_SPI_WByte(0xFE);//д�뿪ʼ�ֽ� 0xfe���������Ҫд���512���ֽڵ�����	
	
 for(i=0;i<4;i++) //����������Ҫд���512���ֽ�д��SD2��������ѭ���������������д���ٶ�
 {
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
 }
  
 SD2_SPI_WByte(0xFF); 
 SD2_SPI_WByte(0xFF); //�����ֽڵ�CRCУ���룬���ù���
       
 r=SD2_SPI_RByte();   //��ȡ����ֵ
 if((r & 0x1F)!=0x05) //�������ֵ�� XXX00101 ˵�������Ѿ���SD2��������
 {
  return(WRITE_BLOCK_ERROR); //д������ʧ��
 }
 
 while(0xFF!=SD2_SPI_RByte());//�ȵ�SD2����æ�����ݱ������Ժ�SD2��Ҫ����Щ����д�뵽�����FLASH�У���Ҫһ��ʱ�䣩
						                 //æʱ����������ֵΪ0x00,��æʱ��Ϊ0xff

 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����SD2���Ĳ���ʱ�������ﲹ8��ʱ�� 
 
 return(0);		 //����0,˵��д���������ɹ�
} 

/****************************************************************************
 - ������������ȡaddr������512���ֽڵ�bufferָ������ݻ�����
 - ����˵����addr:������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD2����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD2_Read_Sector(UINT32 addr,UINT8 *buffer)//��SD2����ָ�������ж���512���ֽڣ�ʹ��CMD17��17�����
{
 UINT8 i;
 UINT8 time,r;
	
 UINT8 pCMD17[]={0x51,0x00,0x00,0x00,0x00,0x01}; //CMD17���ֽ�����
   
 if(!SD2_Addr_Mode) addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD17[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
 pCMD17[2]=addr>>16;
 pCMD17[3]=addr>>8;
 pCMD17[4]=addr;	

 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD17); //д��CMD17
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_BLOCK_ERROR); //����ʧ��
  }
 }while(r!=0); 
   			
 while(SD2_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������

 for(i=0;i<4;i++)	 //������д�뵽���ݻ�������
 {	
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
 }

 SD2_SPI_RByte();
 SD2_SPI_RByte();//��ȡ�����ֽڵ�CRCУ���룬���ù�������

 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����SD2���Ĳ���ʱ�������ﲹ8��ʱ�� 

 return 0;
}

/****************************************************************************
 - ������������addr������ʼ��nsec������д�����ݣ���Ӳ��������д�룩
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD2����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD2_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)	
{  
 UINT8 r,time;
 UINT32 i=0,j=0;
	
 UINT8 pCMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25������ɶ������д
 UINT8 pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD2��������ACMD,CMD55+ACMD23
 UINT8 pACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23���������Ԥ����

 if(!SD2_Addr_Mode) addr<<=9; 

 pCMD25[1]=addr>>24;
 pCMD25[2]=addr>>16;
 pCMD25[3]=addr>>8;
 pCMD25[4]=addr;

 pACMD23[1]=nsec>>24;
 pACMD23[2]=nsec>>16;
 pACMD23[3]=nsec>>8;
 pACMD23[4]=nsec; 

 if(SD2_Ver!=SD_VER_MMC) //�������MMC����������д��Ԥ�������CMD55+ACMD23�����������������д���ٶȻ����
 {
	SD2_Write_Cmd(pCMD55);
	SD2_Write_Cmd(pACMD23);
 }

 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD25);
  time++;
  if(time==TRY_TIME) 
  { 
   return(WRITE_CMD25_ERROR); //����д��ʧ��
  }
 }while(r!=0); 

 while(0XFF!=SD2_SPI_RByte()); //�ȴ�SD2��׼���ã������䷢���������������

 for(j=0;j<nsec;j++)
 {
  SD2_SPI_WByte(0xFC);//д�뿪ʼ�ֽ� 0xfc���������Ҫд���512���ֽڵ�����	
	
  for(i=0;i<4;i++) //����������Ҫд���512���ֽ�д��SD2��
  {
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
   SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));SD2_SPI_WByte(*(buffer++));
  }
  
  SD2_SPI_WByte(0xFF); 
  SD2_SPI_WByte(0xFF); //�����ֽڵ�CRCУ���룬���ù���
       
  r=SD2_SPI_RByte();   //��ȡ����ֵ
  if((r & 0x1F)!=0x05) //�������ֵ�� XXX00DELAY_TIME1 ˵�������Ѿ���SD2��������
  {
   return(WRITE_NBLOCK_ERROR); //д������ʧ��
  }
 
  while(0xFF!=SD2_SPI_RByte());//�ȵ�SD2����æ�����ݱ������Ժ�SD2��Ҫ����Щ����д�뵽�����FLASH�У���Ҫһ��ʱ�䣩
						                  //æʱ����������ֵΪ0x00,��æʱ��Ϊ0xff
 }

 SD2_SPI_WByte(0xFD);

 while(0xFF!=SD2_SPI_RByte());

 SET_SD2_CS_PIN_HIGH();//�ر�Ƭѡ

 SD2_SPI_WByte(0xFF);//����SD2���Ĳ���ʱ�������ﲹ8��ʱ��

 return(0);		 //����0,˵��д���������ɹ�
} 

/****************************************************************************
 - ������������ȡaddr������ʼ��nsec�����������ݣ���Ӳ����������ȡ��
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD2����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD2_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)
{
 UINT8 r,time;
 UINT32 i=0,j=0;
	
 UINT8 pCMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18���ֽ�����
 UINT8 pCMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; //CMD12��ǿ��ֹͣ����
   
 if(!SD2_Addr_Mode) addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD18[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
 pCMD18[2]=addr>>16;
 pCMD18[3]=addr>>8;
 pCMD18[4]=addr;	

 time=0;
 do
 {  
  r=SD2_Write_Cmd(pCMD18); //д��CMD18
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_CMD18_ERROR); //д��CMD18ʧ��
  }
 }while(r!=0); 
 
 for(j=0;j<nsec;j++)
 {  
  while(SD2_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������
 
  for(i=0;i<4;i++)	 //������д�뵽���ݻ�������
  {	
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
   *(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();*(buffer++)=SD2_SPI_RByte();
  }
 
  SD2_SPI_RByte();
  SD2_SPI_RByte();//��ȡ�����ֽڵ�CRCУ���룬���ù�������
 }

 SD2_Write_Cmd(pCMD12); //д��CMD12���ֹͣ���ݶ�ȡ 

 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����SD2���Ĳ���ʱ�������ﲹ8��ʱ�� 

 return 0;
}

/****************************************************************************
 - ������������ȡSD2��������������ͨ����ȡSD2����CSD���������õ�����������
 - ����˵������
 - ����˵��������SD2������������
 - ע����
 ****************************************************************************/

UINT32 SD2_GetTotalSec(void)
{
 UINT8 pCSD[16];
 UINT32 Capacity;  
 UINT8 n,i;
 UINT16 csize; 

 UINT8 pCMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	

 if(SD2_Write_Cmd(pCMD9)!=0) //д��CMD9����
 {
	return GET_CSD_ERROR; //��ȡCSDʱ��������
 }

 while(SD2_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������16�ֽڵ�CSD����

 for(i=0;i<16;i++) pCSD[i]=SD2_SPI_RByte(); //��ȡCSD����

 SD2_SPI_RByte();
 SD2_SPI_RByte(); //��ȡ�����ֽڵ�CRCУ���룬���ù�������

 SET_SD2_CS_PIN_HIGH();
 SD2_SPI_WByte(0xFF); //����SD2���Ĳ���ʱ�������ﲹ8��ʱ�� 
	
 //���ΪSDHC�����������������������淽ʽ����
 if((pCSD[0]&0xC0)==0x40)	 //SD22.0�Ŀ�
 {	
	csize=pCSD[9]+(((UINT16)(pCSD[8]))<<8)+1;
  Capacity=((UINT32)csize)<<10;//�õ�������	 		   
 }
 else //SD1.0�Ŀ�
 {	
	n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
	csize=(pCSD[8]>>6)+((UINT16)pCSD[7]<<2)+((UINT16)(pCSD[6]&0x03)<<0x0A)+1;
	Capacity=(UINT32)csize<<(n-9);//�õ�������   
 }
 return Capacity;
}



