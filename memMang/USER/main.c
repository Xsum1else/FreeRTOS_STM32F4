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
	delay_init(168);   // ��ʼ����ʱ����
	uart_init(115200); // ��ʼ������
	LED_Init();		   // ��ʼ�� LED �˿�

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
		vTaskDelay(50);
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
