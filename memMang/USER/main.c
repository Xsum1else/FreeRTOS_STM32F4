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
//  ALIENTEK 探索者STM32F407开发板 实验37
//  内存管理实验-HAL库函数版
//  技术支持：www.openedv.com
//  淘宝店铺：http://eboard.taobao.com
//  关注微信公众平台微信号："正点原子"，免费获取STM32资料。
//  广州市星翼电子科技有限公司
//  作者：正点原子 @ALIENTEK
// ************************************************/

// int main(void)
// {
// 	u8 key;
//  	u8 i=0;
// 	u8 *p=0;
// 	u8 *tp=0;
// 	u8 paddr[18];					//存放P Addr:+p地址的ASCII值
// 	u8 sramx=0;						//默认为内部sram

//     HAL_Init();                   	//初始化HAL库
//     Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
// 	delay_init(168);               	//初始化延时函数
// 	uart_init(115200);             	//初始化USART
// //	usmart_dev.init(84); 		    //初始化USMART
// 	LED_Init();						//初始化LED
// 	KEY_Init();						//初始化KEY
// 	LCD_Init();                     //初始化LCD
// 	SRAM_Init();					//初始化外部SRAM

// 	my_mem_init(SRAMIN);			//初始化内部内存池
// 	my_mem_init(SRAMEX);			//初始化外部内存池
// 	my_mem_init(SRAMCCM);			//初始化CCM内存池

//  	POINT_COLOR=RED;//设置字体为红色
// 	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");
// 	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");
// 	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
// 	LCD_ShowString(30,110,200,16,16,"2014/5/15");
// 	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
// 	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Read");
//  	POINT_COLOR=BLUE;//设置字体为蓝色
// 	LCD_ShowString(30,170,200,16,16,"SRAMIN");
// 	LCD_ShowString(30,190,200,16,16,"SRAMIN  USED:   %");
// 	LCD_ShowString(30,210,200,16,16,"SRAMEX  USED:   %");
// 	LCD_ShowString(30,230,200,16,16,"SRAMCCM USED:   %");
//  	while(1)
// 	{
// 		key=KEY_Scan(0);//不支持连按
// 		switch(key)
// 		{
// 			case 0://没有按键按下
// 				break;
// 			case KEY0_PRES:	//KEY0按下
// 				p=mymalloc(sramx,2048);//申请2K字节
// 				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//向p写入一些内容
// 				break;
// 			case KEY1_PRES:	//KEY1按下
// 				if(p!=NULL)
// 				{
// 					sprintf((char*)p,"Memory Malloc Test%03d",i);//更新显示内容
// 					LCD_ShowString(30,270,200,16,16,p);			 //显示P的内容
// 				}
// 				break;
// 			case KEY2_PRES:	//KEY2按下
// 				myfree(sramx,p);//释放内存
// 				p=0;			//指向空地址
// 				break;
// 			case WKUP_PRES:	//KEY UP按下
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
// 			LCD_ShowString(30,250,200,16,16,paddr);	//显示p的地址
// 			if(p)LCD_ShowString(30,270,200,16,16,p);//显示P的内容
// 		    else LCD_Fill(30,270,239,266,WHITE);	//p=0,清除显示
// 		}
// 		delay_ms(10);
// 		i++;
// 		if((i%20)==0)//DS0闪烁.
// 		{
// 			LCD_ShowNum(30+104,190,my_mem_perused(SRAMIN),3,16);//显示内部内存使用率
// 			LCD_ShowNum(30+104,210,my_mem_perused(SRAMEX),3,16);//显示外部内存使用率
// 			LCD_ShowNum(30+104,230,my_mem_perused(SRAMCCM),3,16);//显示CCM内存使用率
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

#define START_TASK_PRIO 1			 // 任务优先级
#define START_STK_SIZE 128			 // 任务堆栈大小
TaskHandle_t StartTask_Handler;		 // 任务句柄
void start_task(void *pvParameters); // 任务函数

#define LED0_TASK_PRIO 2	   // 任务优先级
#define LED0_STK_SIZE 50	   // 任务堆栈大小
TaskHandle_t LED0Task_Handler; // 任务句柄
void led0_task(void *p_arg);   // 任务函数

#define LED1_TASK_PRIO 3	   // 任务优先级
#define LED1_STK_SIZE 50	   // 任务堆栈大小
TaskHandle_t LED1Task_Handler; // 任务句柄
void led1_task(void *p_arg);   // 任务函数

#define FLOAT_TASK_PRIO 4		// 任务优先级
#define FLOAT_STK_SIZE 128		// 任务堆栈大小
TaskHandle_t FLOATTask_Handler; // 任务句柄
void float_task(void *p_arg);	// 任务函数

int main(void)
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置系统中断优先级分组4
	delay_init(168);								// 初始化延时函数
	uart_init(115200);								// 初始化串口
	LED_Init();										// 初始化 LED 端口

	// 创建开始任务
	xTaskCreate((TaskFunction_t)start_task,			 // 任务函数
				(const char *)"start task",			 // 任务名称
				(uint16_t)START_STK_SIZE,			 // 任务堆栈大小
				(void *)NULL,						 // 传递给任务函数的参数
				(UBaseType_t)START_TASK_PRIO,		 // 任务优先级
				(TaskHandle_t *)&StartTask_Handler); // 任务句柄
	vTaskStartScheduler();							 // 开启任务调度
}

// 开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // 进入临界区
	// 创建 LED0 任务
	xTaskCreate((TaskFunction_t)led0_task,
				(const char *)"led0_task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);

	// 创建 LED1 任务
	xTaskCreate((TaskFunction_t)led1_task,
				(const char *)"led1_task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);

	// 浮点测试任务
	xTaskCreate((TaskFunction_t)float_task,
				(const char *)"float_task",
				(uint16_t)FLOAT_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)FLOAT_TASK_PRIO,
				(TaskHandle_t *)&FLOATTask_Handler);

	vTaskDelete(StartTask_Handler); // 删除开始任务
	taskEXIT_CRITICAL();			// 退出临界区
}

// LEDO 任务函数
void led0_task(void *pvParameters)
{
	while (1)
	{
		LED0 = ~LED0;
		vTaskDelay(100);
	}
}

// LED1 任务函数
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

// 浮点测试任务
void float_task(void *p_arg)
{
	static float float_num = 0.00;
	while (1)
	{
		float_num += 0.01f;
		printf("float_num 的值为: %.4f\r\n", float_num);
		vTaskDelay(1000);
	}
}
