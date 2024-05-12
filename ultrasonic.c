#include "ultrasonic.h"
#include "extern.h"
#include "def.h"

// 거리 계산
volatile int ultrasonic_distance = 0;
volatile int distance = 0;
char scm[50];

// ISR은 파일의 맨 위에 짜주는 것이 좋습니다.(중요하기 때문)
// PE4는 외부 INT4 초음파센서 상승,하강에지 둘다 이곳으로 들어온다.
// 결국은 상승에지에서 1번 / 하강에지에서 1번 2번 들어온다.
ISR(INT4_vect){
	// 1.상승 에지일때 처리하는 부분
	// port echo핀의 4번핀이 1이 됫을때임
	if(ECHO_PIN & (1<<ECHO)){
		// 이때부터 길이를 재기 시작함(pulse를 몇개를 count했는지를 통해서)
		//카운트된 펄스값을 us로 환산하면 길이가 나온다
		TCNT1 = 0;
	}else {
		// 2.하강 에지일때 처리하는 부분
		// port echo핀의 4번핀이 0이 됫을때임
		/* 에코핀에 들어오는 펄스 길이를 us단위로 환산한다 
		(1024로 분주하잖아요?) */
		ultrasonic_distance = 1000000.0 * TCNT1 * 1024 / F_CPU;
		// 예) TCNT1에 10이 들어있다고 가정하자.
		// 15.625kHz의 1주기가 64us이다.
		// 0.000064 * 10 = 0.00064sec(640us)
		 // 640us / 58us = 11cm
		 // 1cm : 58us가 소요됨(왕복시간 반영)
		 sprintf(scm,"dis %dcm\n",ultrasonic_distance/58); //cm단위로 환산해준다
		 double distance =  ultrasonic_distance/58;
		 if(distance > 4 && distance <= 5){
			LED_PORT = 0x1f;
		 }else if(distance >3 && distance <= 4){
			 LED_PORT = 0x0f;
		 }else if(distance > 2 && distance <= 3){
			 LED_PORT= 0x07;
		 }else if(distance > 1 && distance <= 2){
			 LED_PORT = 0x03;
		 }else if(distance > 0 && distance <= 1){
			LED_PORT = 0x01;
		}else {
			 LED_PORT = 0x00;
		 }
	}
}


void init_ultrasonic(){
	TRIG_DDR |= 1 << TRIG; // output mode로 설정
	ECHO_DDR &= ~(1 << ECHO); //input mode로 설정
	
	//p.289 표 12-6
	// 0 1로 해야 : 상승엣지, 하강엣지 둘다 INT를 뛰우도록 설정할 수 있음.
	EICRB |= 0 << ISC41 | 1 << ISC40;
	// 16비트 타이머 1번을 설정을 해서 사용한다.
	// 16비트는 65535까지임
	// 16MHz clock
	// 16MHz를 1024 분주한다 16000000/1024 = 15625Hz = 15.625kHz
	// 1주기(1개의 pulse길이) T = 1/f = 1/15.625k = 0.000064 = 64us
	TCCR1B |= 1 << CS12 | 1 << CS10; //1024로 분주 설정 // timer1번 설정
	
	EIMSK |= 1 << INT4; // 외부 INT4(ECHO)사용
}

void ultrasonic_trigger(){
	TRIG_PORT &= ~(1 << TRIG); // LOW로 떨어트리는 것
	_delay_us(1);
	TRIG_PORT |= 1 << TRIG; // HIGH 유지(15ms정도 high신호를 유지하자)
	_delay_us(15);
	// 유지한 후 trigger신호를 low로 떨어트리자
	TRIG_PORT &= ~(1 << TRIG); // LOW로 떨어트리는 것
}

void ultrasonic_distance_check(){
	if(ultrasonic_trigger_timer > 1000){
		// 1초
		ultrasonic_trigger_timer = 0;
		printf("%s",scm);
		ultrasonic_trigger();
	}
}