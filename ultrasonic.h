#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

/* port와, 인터럽트관련 내용 include */
#define F_CPU 16000000UL
#include <avr/io.h> // PORTA DDRA 등의 symbom이 정의 되어 있다.
#include <util/delay.h> // _delay_ms 등의 함수가 들어있다.
#include <avr/interrupt.h>


#define TRIG_DDR DDRG
#define TRIG_PORT PORTG
#define TRIG 4

#define ECHO_DDR DDRE
#define ECHO_PIN PINE // INT4
#define ECHO 4

void ultrasonic_trigger();
void init_ultrasonic();
void ultrasonic_distance_check();

#endif /* ULTRASONIC_H_ */