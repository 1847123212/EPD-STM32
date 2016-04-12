#include "key.h" 

 /*
 * ��������Key_GPIO_Config
 * ����  �����ð����õ���I/O��
 * ����  ����
 * ���  ����
 */
void Key_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_UP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

 /*
 * ��������Key_Scan
 * ����  ������Ƿ��а�������
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��KEY_OFF(û���°���)��KEY_ON�����°�����
 */
u8 Key_ScanL(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
		/*����Ƿ��а������� */
   if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 ) 
  {	   
	 	 /*��ʱ����*/
    DelayCycle(5000);		//CPUռ�øߣ�5000 is enough
    if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0 )  
    {	 
	/*�ȴ������ͷ� */
	while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0);  
	return 	1;	 
    }
    else
	return 0;
  }
  else
    return 0;
}

unsigned char Key_Scan(unsigned char id)
{
  switch (id)
  {
  case 0: return Key_ScanL(GPIOB,GPIO_Pin_12);
  case 1: return Key_ScanL(GPIOB,GPIO_Pin_13);
  case 2: return Key_ScanL(GPIOB,GPIO_Pin_14);
  case 3: return Key_ScanL(GPIOB,GPIO_Pin_15);
  }
}
