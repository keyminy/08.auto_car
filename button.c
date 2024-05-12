/*
 * button.c
 *
 * Created: 2024-04-25 오후 1:30:32
 *  Author: HARMAN-27
 */ 
#include "button.h"
#include "def.h"

void init_button(void);
int get_button(int button_pin, int button_number);

extern void stop();
void auto_mode_check();
int button1_state = 0;

// 채터링제거를 위한 flag변수(각각의 버튼에 대한 정보를 담는 table)
// 지금은 전역변수로 선언했지만 싫다면, static변수로 만들어줘도됨
unsigned char previous_button_status[BUTTON_NUMBER] = 
{BUTTON_RELEASE};

// 76543210
// DDRD &= 0x0f; // port[7:4]까지 input으로 선언된것
void init_button(void){
	// one's complement
	BUTTON_DDR &= ~(1<<BUTTON1_PIN);
}


void auto_mode_check(){
	if(get_button(BUTTON1_PIN,BUTTON1)){
		button1_state = !button1_state;
	}
	if(button1_state){
		PORTG |= 1 << 3; //3번핀의 LED ON
		stop();
	}else{
		PORTG &= ~(1 << 3); // LED OFF
		manual_mode_run();
	}
}

// 버튼이 온전하게 눌렀다 떼어진 상태이면 1을 리턴한다
int get_button(int button_pin, int button_number){
	int current_state;

	//예)BUTTON1은 4가 넘어옴, 2는 5, 3은 5...
	//BUTTON_PIN값은 눌리면1 안눌리면 0값을 감지함
	current_state = BUTTON_PIN & (1 << button_pin); // 10000 (1<<4결과)
	//current_state변수로 버튼 값을 읽습니다.

	if(current_state && previous_button_status[button_number]== BUTTON_RELEASE){
		// 버튼이 처음 눌러진 상태
		_delay_ms(60); // 노이즈가 지나가기를 기다린다.
		previous_button_status[button_number] = BUTTON_PRESS; // 처음 눌러진 상태가 아니다
		return 0; //아직 완전히 눌렀다 떼어진 상태가 아니다.
	} // 버튼이 이전에 눌러진 상태였으면 현재는 떼어진 상태
	else if(previous_button_status[button_number] == BUTTON_PRESS && current_state == BUTTON_RELEASE){
		previous_button_status[button_number] = BUTTON_RELEASE; // 다음 버튼 상태를 체크하기 위해 초기화
		_delay_ms(60); // 노이즈가 지나가기를 기다린다.
		return 1; // 완전히 눌렀다 떼어진 상태
	}
	// 아직 HIGH인 상태인 것이지
	// 아직 완전히 스위치를 눌렀다 뗀 상태가 아니거나 스위치가 open된 상태 -> return 0;
	return 0;
}
