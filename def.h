/*
 * def.h
 *
 * Created: 2024-04-29 오후 1:40:03
 *  Author: HARMAN-27
 */ 


#ifndef DEF_H_
#define DEF_H_

#define F_CPU 16000000UL
#include <avr/io.h> // PORTA DDRA 등의 symbom이 정의 되어 있다.
#include <util/delay.h> // _delay_ms 등의 함수가 들어있다.
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define LED_DDR DDRA
#define LED_PORT PORTA

#define LEFT2RIGHT 0
#define RIGHT2LEFT 1
#define FLOWER_ON 2
#define FLOWER_OFF 3
#define ON_OFF 4

#define COMMAND_NUMBER 20
#define COMMAND_LENGTH 40

#endif /* DEF_H_ */