/*
 * timer.c
 *
 * Created: 2024-05-06 오전 12:54:39
 *  Author: me
 */ 
#include "def.h"

//led를 가동해보자(전역변수 선언)
// ISR안에 쓰는 변수는 변수 type앞에 volatile써야한다.
// 이유는 최적화 방지를 위함이다.(내가 작성한 코드 일일이 수행하게 해줘)
volatile uint32_t ms_count = 0; // ms count

//타이머 관련, 전역변수로 선언
volatile uint32_t shift_timer = 0;
//1초에 한번씩 trigger를 쏘겟다
volatile uint32_t ultrasonic_trigger_timer = 0; // trigger를 동작시키는 주기 : 1000ms

void init_uart0(void);

//timer0를 초기화 한다.
void init_timer0(){
	// 8bit timer, timer 0번과 2번 사용한다.
	//분주비를 64로 설정한다.
	// 1.분주비 계산
	// 16000000Hz/64 = 250,000Hz
	
	// 2.T(주기) : pulse 1개가 잡아 먹는 시간(주파수의 역수 : 1/f)
	// 1 / 250,000 = 0.000004sec(=4us,0.004ms)
	// 3. 8bit timer overflow : 0.004ms * 256개 카운트 = 0.001024sec = 1.024ms
	// 1ms가 정확히 안되자너... 1ms가 정확히 될려면 어떻게 해야하나?
	// 0.004ms * 250 = 0.001(=1ms)
	
	//진짜 나중에 추가한 코드임
	TCNT0 = 6; // 6부터 count하는것임, TCNT : 0~256카운트하기때문에, 정확히 1ms마다 TIMER0_OVF_vect로 진입한다.
	
	TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00); // (1)분주비 64로 셋팅(p.296 표13-1 참고)
	TIMSK = 1 << TOIE0; // (2) TIMER0 overflow interrupt허용
}

// 256개의 pulse(=1ms)를 count하면 이곳으로 자동적으로 진입한다.
// 즉, 256개의 pulse == 1ms
ISR(TIMER0_OVF_vect){
	/* 인터럽트 루틴을 가능한 짧게 짜라, ms_count만 증가시키고 빠져나오게함 */
	TCNT0=6; // 6 ~ 256개의 pulse카운트 --> 1ms를 맞춰주기 위해서 TCNT0을 6으로 설정
	ms_count++;
	//ms_count delay없애는 작업필요함
	shift_timer++;
	ultrasonic_trigger_timer++;
}


