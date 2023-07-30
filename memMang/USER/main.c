// #include "sys.h"
// #include "delay.h"
// #include "usart.h"
// #include "led.h"
// #include "key.h"
// #include "lcd.h"
// #include "usmart.h"
// #include "sram.h"
// #include "malloc.h"
// /************************************************
//  ALIENTEK ̽����STM32F407������ ʵ��37
//  �ڴ����ʵ��-HAL�⺯����
//  ����֧�֣�www.openedv.com
//  �Ա����̣�http://eboard.taobao.com
//  ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
//  ������������ӿƼ����޹�˾
//  ���ߣ�����ԭ�� @ALIENTEK
// ************************************************/

// int main(void)
// {
// 	u8 key;
//  	u8 i=0;
// 	u8 *p=0;
// 	u8 *tp=0;
// 	u8 paddr[18];					//���P Addr:+p��ַ��ASCIIֵ
// 	u8 sramx=0;						//Ĭ��Ϊ�ڲ�sram

//     HAL_Init();                   	//��ʼ��HAL��
//     Stm32_Clock_Init(336,8,2,7);  	//����ʱ��,168Mhz
// 	delay_init(168);               	//��ʼ����ʱ����
// 	uart_init(115200);             	//��ʼ��USART
// //	usmart_dev.init(84); 		    //��ʼ��USMART
// 	LED_Init();						//��ʼ��LED
// 	KEY_Init();						//��ʼ��KEY
// 	LCD_Init();                     //��ʼ��LCD
// 	SRAM_Init();					//��ʼ���ⲿSRAM

// 	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��
// 	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��
// 	my_mem_init(SRAMCCM);			//��ʼ��CCM�ڴ��

//  	POINT_COLOR=RED;//��������Ϊ��ɫ
// 	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");
// 	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");
// 	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
// 	LCD_ShowString(30,110,200,16,16,"2014/5/15");
// 	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
// 	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Read");
//  	POINT_COLOR=BLUE;//��������Ϊ��ɫ
// 	LCD_ShowString(30,170,200,16,16,"SRAMIN");
// 	LCD_ShowString(30,190,200,16,16,"SRAMIN  USED:   %");
// 	LCD_ShowString(30,210,200,16,16,"SRAMEX  USED:   %");
// 	LCD_ShowString(30,230,200,16,16,"SRAMCCM USED:   %");
//  	while(1)
// 	{
// 		key=KEY_Scan(0);//��֧������
// 		switch(key)
// 		{
// 			case 0://û�а�������
// 				break;
// 			case KEY0_PRES:	//KEY0����
// 				p=mymalloc(sramx,2048);//����2K�ֽ�
// 				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//��pд��һЩ����
// 				break;
// 			case KEY1_PRES:	//KEY1����
// 				if(p!=NULL)
// 				{
// 					sprintf((char*)p,"Memory Malloc Test%03d",i);//������ʾ����
// 					LCD_ShowString(30,270,200,16,16,p);			 //��ʾP������
// 				}
// 				break;
// 			case KEY2_PRES:	//KEY2����
// 				myfree(sramx,p);//�ͷ��ڴ�
// 				p=0;			//ָ��յ�ַ
// 				break;
// 			case WKUP_PRES:	//KEY UP����
// 				sramx++;
// 				if(sramx>2)sramx=0;
// 				if(sramx==0)LCD_ShowString(30,170,200,16,16,"SRAMIN ");
// 				else if(sramx==1)LCD_ShowString(30,170,200,16,16,"SRAMEX ");
// 				else LCD_ShowString(30,170,200,16,16,"SRAMCCM");
// 				break;
// 		}
// 		if(tp!=p)
// 		{
// 			tp=p;
// 			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
// 			LCD_ShowString(30,250,200,16,16,paddr);	//��ʾp�ĵ�ַ
// 			if(p)LCD_ShowString(30,270,200,16,16,p);//��ʾP������
// 		    else LCD_Fill(30,270,239,266,WHITE);	//p=0,�����ʾ
// 		}
// 		delay_ms(10);
// 		i++;
// 		if((i%20)==0)//DS0��˸.
// 		{
// 			LCD_ShowNum(30+104,190,my_mem_perused(SRAMIN),3,16);//��ʾ�ڲ��ڴ�ʹ����
// 			LCD_ShowNum(30+104,210,my_mem_perused(SRAMEX),3,16);//��ʾ�ⲿ�ڴ�ʹ����
// 			LCD_ShowNum(30+104,230,my_mem_perused(SRAMCCM),3,16);//��ʾCCM�ڴ�ʹ����
//  			LED0=!LED0;
//  		}
// 	}
// }

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO 1			 // �������ȼ�
#define START_STK_SIZE 128			 // �����ջ��С
TaskHandle_t StartTask_Handler;		 // ������
void start_task(void *pvParameters); // ������

#define LED0_TASK_PRIO 2	   // �������ȼ�
#define LED0_STK_SIZE 50	   // �����ջ��С
TaskHandle_t LED0Task_Handler; // ������
void led0_task(void *p_arg);   // ������

#define LED1_TASK_PRIO 3	   // �������ȼ�
#define LED1_STK_SIZE 50	   // �����ջ��С
TaskHandle_t LED1Task_Handler; // ������
void led1_task(void *p_arg);   // ������

#define FLOAT_TASK_PRIO 4		// �������ȼ�
#define FLOAT_STK_SIZE 128		// �����ջ��С
TaskHandle_t FLOATTask_Handler; // ������
void float_task(void *p_arg);	// ������

int main(void)
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // ����ϵͳ�ж����ȼ�����4
	delay_init(168);								// ��ʼ����ʱ����
	uart_init(115200);								// ��ʼ������
	LED_Init();										// ��ʼ�� LED �˿�

	// ������ʼ����
	xTaskCreate((TaskFunction_t)start_task,			 // ������
				(const char *)"start task",			 // ��������
				(uint16_t)START_STK_SIZE,			 // �����ջ��С
				(void *)NULL,						 // ���ݸ��������Ĳ���
				(UBaseType_t)START_TASK_PRIO,		 // �������ȼ�
				(TaskHandle_t *)&StartTask_Handler); // ������
	vTaskStartScheduler();							 // �����������
}

// ��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // �����ٽ���
	// ���� LED0 ����
	xTaskCreate((TaskFunction_t)led0_task,
				(const char *)"led0_task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);

	// ���� LED1 ����
	xTaskCreate((TaskFunction_t)led1_task,
				(const char *)"led1_task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);

	// �����������
	xTaskCreate((TaskFunction_t)float_task,
				(const char *)"float_task",
				(uint16_t)FLOAT_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)FLOAT_TASK_PRIO,
				(TaskHandle_t *)&FLOATTask_Handler);

	vTaskDelete(StartTask_Handler); // ɾ����ʼ����
	taskEXIT_CRITICAL();			// �˳��ٽ���
}

// LEDO ������
void led0_task(void *pvParameters)
{
	while (1)
	{
		LED0 = ~LED0;
		vTaskDelay(100);
	}
}

// LED1 ������
void led1_task(void *pParameters)
{
	while (1)
	{
		LED1 = 0;
		vTaskDelay(200);
		LED1 = 1;
		vTaskDelay(800);
	}
}

// �����������
void float_task(void *p_arg)
{
	static float float_num = 0.00;
	while (1)
	{
		float_num += 0.01f;
		printf("float_num ��ֵΪ: %.4f\r\n", float_num);
		vTaskDelay(1000);
	}
}
