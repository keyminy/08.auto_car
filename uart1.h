/*
 * uart1.h
 *
 * Created: 2024-05-07 오후 3:40:06
 *  Author: HARMAN-27
 */ 


#ifndef UART1_H_
#define UART1_H_

//uart0로 부터 들어온 문자를 저장하는 버퍼
//interrupt루틴에서 사용하는 변수는 최적화방지를위해 volatile키워드
volatile uint8_t rx1_buff[100];
//기본 : 0으로 set해줍니다
volatile uint8_t rx1_ready_flag = 0; // 완전한 문장(\n)을 만나면 1로 set된다.

volatile int rear1=0;   // ISR(USART1_RX_vect) 저장 하는 index
volatile int front1=0;  // bit_processing에서 꺼내가는 index
volatile char rx1_Quebuff[20][40];


void init_uart1(void);
void UART1_transmit(uint8_t data);
void bit_command_processing(void);



#endif /* UART1_H_ */