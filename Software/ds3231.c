/******************************************************************************
* @ File name --> ds3231.c
* @ Author    --> By@ Sam Chan
* @ Version   --> V1.0
* @ Date      --> 02 - 01 - 2014
* @ Brief     --> �߾���ʼ��оƬDS3231��������
*
* @ Copyright (C) 20**
* @ All rights reserved
*******************************************************************************
*
*                                  File Update
* @ Version   --> V1.
* @ Author    -->
* @ Date      -->
* @ Revise    -->
*
******************************************************************************/

#include "ds3231.h"

/******************************************************************************
                               ������ʾʱ���ʽ
                         Ҫ�ı���ʾ�ĸ�ʽ���޸Ĵ�����
******************************************************************************/

u8 Display_Time[8] = {0x30,0x30,0x3a,0x30,0x30,0x3a,0x30,0x30};	
					//ʱ����ʾ����   ��ʽ  00:00:00

u8 Display_Date[13] = {0x32,0x30,0x31,0x33,0x2f,0x31,0x30,0x2f,0x32,0x30,0x20,0x37,0x57};
					//������ʾ����   ��ʽ  2013/10/20 7W

/******************************************************************************
                               ������صı�������
******************************************************************************/

//Time_Typedef TimeValue;	//����ʱ������ָ��
u8 TimeValue[7];

u8 Time_Buffer[7];	//ʱ���������ݻ���

void IIC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_9;   //ѡ��PB6��PB7����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //����PB6��PB7�ĸ��ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //PB6��PB7����Ϊ��©���
  GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;   //PB6��PB7�ڲ�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   
  GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SDA_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_UP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H();
	Delay_Us(1);	  	  
	IIC_SCL_H();
	Delay_Us(5);
 	IIC_SDA_L();//START:when CLK is high,DATA change form high to low 
	Delay_Us(5);
	IIC_SCL_L();//ǯסI2C���ߣ�׼�����ͻ�������� 
	Delay_Us(2);
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L();
	IIC_SDA_L();//STOP:when CLK is high DATA change form low to high
 	Delay_Us(4);
	IIC_SCL_H(); 
	Delay_Us(5);
	IIC_SDA_H();//����I2C���߽����ź�
	Delay_Us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H();Delay_Us(1);	   
	IIC_SCL_H();Delay_Us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>500)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L();//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_SAck(void)
{
	IIC_SCL_L();
	SDA_OUT();
	IIC_SDA_L();
	Delay_Us(2);
	IIC_SCL_H();
	Delay_Us(2);
	IIC_SCL_L();
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L();
	SDA_OUT();
	IIC_SDA_H();
	Delay_Us(2);
	IIC_SCL_H();
	Delay_Us(2);
	IIC_SCL_L();
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
u8 IIC_Write_Byte(u8 txd)
{                        
    u8 t;   
    
    SDA_OUT(); 	    
    IIC_SCL_L();//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_H();
		else
			IIC_SDA_L();
		txd<<=1; 	  
		Delay_Us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H();
		Delay_Us(2); 
		IIC_SCL_L();	
		Delay_Us(2);
    }	 

    t = IIC_Wait_Ack();
    
    Delay_Us(5);
    
    return t;
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte()
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL_L(); 
        Delay_Us(2);
		IIC_SCL_H();
        receive<<=1;
        if(READ_SDA)receive++;   
		Delay_Us(1); 
    }					 
    /*if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   */
    return receive;
}

void IIC_Ack(unsigned char ack)
{
  if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_SAck(); //����ACK   
}

/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ���д��һ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
*                   dat��Ҫд�������
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void DS3231_Write_Byte(u8 REG_ADD,u8 dat)
{
	IIC_Start();
        Delay_Us(5);
	if(!(IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
	{
		IIC_Write_Byte(REG_ADD);
		IIC_Write_Byte(dat);	//��������
	}
	IIC_Stop();
        
        //Delay_Us(500);
}
/******************************************************************************
* Function Name --> DS3231ĳ�Ĵ�����ȡһ���ֽ�����
* Description   --> none
* Input         --> REG_ADD��Ҫ�����Ĵ�����ַ
* Output        --> none
* Reaturn       --> ��ȡ���ļĴ�������ֵ 
******************************************************************************/
u8 DS3231_Read_Byte(u8 REG_ADD)
{
	u8 ReData;
        
	IIC_Start();
	if(!(IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
	{
		IIC_Write_Byte(REG_ADD);	//ȷ��Ҫ�����ļĴ���
		IIC_Start();	//��������
		IIC_Write_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
		ReData = IIC_Read_Byte();	//��ȡ����
		IIC_Ack(1);	//���ͷ�Ӧ���źŽ������ݴ���
	}
	IIC_Stop();
        
        //Delay_Us(500);
        
	return ReData;
}
/******************************************************************************
* Function Name --> DS3231��ʱ�������Ĵ���������д�����ݻ��߶�ȡ����
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> REG_ADD��Ҫ�����Ĵ�����ʼ��ַ
*                   *WBuff��д�����ݻ���
*                   num��д����������
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Operate_Register(u8 REG_ADD,u8 *pBuff,u8 num,u8 mode)
{
	u8 i;
	if(mode)	//��ȡ����
	{
		IIC_Start();
		if(!(IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
		{
			IIC_Write_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
			IIC_Start();	//��������
			IIC_Write_Byte(DS3231_Read_ADD);	//���Ͷ�ȡ����
			for(i = 0;i < num;i++)
			{
				*pBuff = IIC_Read_Byte();	//��ȡ����
				if(i == (num - 1))	IIC_Ack(1);	//���ͷ�Ӧ���ź�
				else IIC_Ack(0);	//����Ӧ���ź�
				pBuff++;
			}
		}
		IIC_Stop();	
	}
	else	//д������
	{		 	
		IIC_Start();
		if(!(IIC_Write_Byte(DS3231_Write_ADD)))	//����д������Ӧ��λ
		{
			IIC_Write_Byte(REG_ADD);	//��λ��ʼ�Ĵ�����ַ
			for(i = 0;i < num;i++)
			{
				IIC_Write_Byte(*pBuff);	//д������
				pBuff++;
			}
		}
		IIC_Stop();
	}
}
/******************************************************************************
* Function Name --> DS3231��ȡ����д��ʱ����Ϣ
* Description   --> ����д��n�ֽڻ���������ȡn�ֽ�����
* Input         --> *pBuff��д�����ݻ���
*                   mode������ģʽ��0��д�����ݲ�����1����ȡ���ݲ���
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_ReadWrite_Time(u8 mode)
{
	u8 Time_Register[8];	//����ʱ�仺��
	
	if(mode)	//��ȡʱ����Ϣ
	{
		//DS3231_Operate_Register(Address_second,Time_Register,7,1);	//�����ַ��0x00����ʼ��ȡʱ����������
                Time_Register[6] = DS3231_Read_Byte(0x06);
                Time_Register[5] = DS3231_Read_Byte(0x05);
                Time_Register[4] = DS3231_Read_Byte(0x04);
                //Time_Register[3] = DS3231_Read_Byte(0x03);
                Time_Register[2] = DS3231_Read_Byte(0x02);
                Time_Register[1] = DS3231_Read_Byte(0x01);
                Time_Register[0] = DS3231_Read_Byte(0x00);
          
          
		/******�����ݸ��Ƶ�ʱ��ṹ���У��������������******/
		TimeValue[0] = Time_Register[0] & Shield_secondBit;	//������
		TimeValue[1] = Time_Register[1] & Shield_minuteBit;	//��������
		TimeValue[2] = Time_Register[2] & Shield_hourBit;	//Сʱ����
		TimeValue[3] = Time_Register[3] & Shield_weekBit;	//��������
		TimeValue[4] = Time_Register[4] & Shield_dateBit;	//������
		TimeValue[5] = Time_Register[5] & Shield_monthBit;	//������
		TimeValue[6] = Time_Register[6];	//������
	}
	else
	{
		/******��ʱ��ṹ���и������ݽ���******/
		Time_Register[0] = TimeValue[0];	//��
		Time_Register[1] = TimeValue[1];	//����
		Time_Register[2] = TimeValue[2] | Hour_Mode24;	//Сʱ
		Time_Register[3] = TimeValue[3];	//����
		Time_Register[4] = TimeValue[4];	//��
		Time_Register[5] = TimeValue[5];	//��
		Time_Register[6] = TimeValue[6];	//��
		
		//DS3231_Operate_Register(Address_second,Time_Register,7,0);	//�����ַ��0x00����ʼд��ʱ����������
                DS3231_Write_Byte(0x06, Time_Register[6]);
                DS3231_Write_Byte(0x05, Time_Register[5]);
                DS3231_Write_Byte(0x04, Time_Register[4]);
                //DS3231_Write_Byte(0x03, Time_Register[3]);
                DS3231_Write_Byte(0x02, Time_Register[2]);
                DS3231_Write_Byte(0x01, Time_Register[1]);
                DS3231_Write_Byte(0x00, Time_Register[0]);
	}
}
/******************************************************************************
* Function Name --> ʱ��������ʼ��
* Description   --> none
* Input         --> *TimeVAL��RTCоƬ�Ĵ���ֵָ��
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void DS3231_Init(void)
{	
	//ʱ����������
	/*Time_Buffer[0] = TimeVAL->second;
	Time_Buffer[1] = TimeVAL->minute;
	Time_Buffer[2] = TimeVAL->hour;
	Time_Buffer[3] = TimeVAL->week;
	Time_Buffer[4] = TimeVAL->date;
	Time_Buffer[5] = TimeVAL->month;
	Time_Buffer[6] = TimeVAL->year;
	
	DS3231_Operate_Register(Address_second,Time_Buffer,7,0);	//���루0x00����ʼд��7������*/
	DS3231_Write_Byte(Address_control, OSC_Enable);
	DS3231_Write_Byte(Address_control_status, Clear_OSF_Flag);
}
/******************************************************************************
* Function Name --> DS3231��⺯��
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> 0: ����
*                   1: ������������Ҫ��ʼ��ʱ����Ϣ
******************************************************************************/
u8 DS3231_Check(void)
{
	if(DS3231_Read_Byte(Address_control_status) & 0x80)  //����ֹͣ������
	{
		return 1;  //�쳣
	}
	else if(DS3231_Read_Byte(Address_control) & 0x80)  //���� EOSC����ֹ��
	{
		return 1;  //�쳣
	}
	else	return 0;  //����
}
/******************************************************************************
* Function Name --> ʱ���������ݴ�����
* Description   --> ����ȡ����ʱ��������Ϣת����ASCII�󱣴浽ʱ���ʽ������
* Input         --> none
* Output        --> none
* Reaturn       --> none
******************************************************************************/
void Time_Handle(void)
{
	/******************************************************
	                   ��ȡʱ��������Ϣ
	******************************************************/
	
	//DS3231_ReadWrite_Time(1);	//��ȡʱ����������
	
	/******************************************************
	            ʱ����Ϣת��ΪASCII������ַ�
	******************************************************/
	
	Display_Time[6] = (TimeValue[0] >> 4) + 0x30;
	Display_Time[7] = (TimeValue[0] & 0x0f) + 0x30;	//Second

	Display_Time[3] = (TimeValue[1] >> 4) + 0x30;
	Display_Time[4] = (TimeValue[1] & 0x0f) + 0x30;	//Minute

	Display_Time[0] = (TimeValue[2] >> 4) + 0x30;
	Display_Time[1] = (TimeValue[2] & 0x0f) + 0x30;	//Hour 

	Display_Date[8] = (TimeValue[4] >> 4) + 0x30;
	Display_Date[9] = (TimeValue[4] & 0x0f) + 0x30;	//Date

	Display_Date[5] = (TimeValue[5] >> 4) + 0x30;
	Display_Date[6] = (TimeValue[5] & 0x0f) + 0x30;	//Month

	//Display_Date[0] = (u8)(TimeValue.year >> 12) + 0x30;
	//Display_Date[1] = (u8)((TimeValue.year & 0x0f00) >> 8) + 0x30;
	Display_Date[2] = (TimeValue[6] >> 4) + 0x30;
	Display_Date[3] = (TimeValue[6] & 0x0f) + 0x30;	//Year

	Display_Date[11] = (TimeValue[3] & 0x0f) + 0x30;	//week

}
/******************************************************************************
* Function Name --> ��ȡоƬ�¶ȼĴ���
* Description   --> �¶ȼĴ�����ַΪ0x11��0x12�������Ĵ���Ϊֻ��
* Input         --> none
* Output        --> *Temp�������¶���ʾ�ַ�����
* Reaturn       --> none
******************************************************************************/
void DS3231_Read_Temp(u8 *Temp)
{
	u8 temph,templ;
	float temp_dec;

	temph = DS3231_Read_Byte(Address_temp_MSB);	//��ȡ�¶ȸ�8bits
	templ = DS3231_Read_Byte(Address_temp_LSB) >> 6;	//��ȡ�¶ȵ�2bits

	//�¶�ֵת��
	if(temph & 0x80)	//�ж��¶�ֵ������
	{	//���¶�ֵ
		temph = ~temph;	//��λȡ��
		templ = ~templ + 0x01;	//��λȡ����1
		Temp[0] = 0x2d;	//��ʾ��-��
	}
	else	Temp[0] = 0x20;	//���¶Ȳ���ʾ���ţ���ʾ������0x2b

	//С�����ּ��㴦��
	temp_dec = (float)templ * (float)0.25;	//0.25��ֱ���

	//�������ּ��㴦��
	temph = temph & 0x70;	//ȥ������λ
	Temp[1] = temph % 1000 / 100 + 0x30;	//��λ
	Temp[2] = temph % 100 / 10 + 0x30;	//ʮλ
	Temp[3] = temph % 10 + 0x30;	//��λ
	Temp[4] = 0x2e;	//.

	//С�����ִ���
	Temp[5] = (u8)(temp_dec * 10) + 0x30;	//С�����һλ
	Temp[6] = (u8)(temp_dec * 100) % 10 + 0x30;	//С������λ

	if(Temp[1] == 0x30)	Temp[1] = 0x20;	//��λΪ0ʱ����ʾ
	if(Temp[2] == 0x30)	Temp[2] = 0x20;	//ʮλΪ0ʱ����ʾ
}

