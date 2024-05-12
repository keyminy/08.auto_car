/*
 * 04.TIMER_TEST.c
 *
 * Created: 2024-04-26 오후 3:23:34
 * Author : HARMAN-27
 */ 

#include "def.h"
#include "extern.h"

// timer1번 사용한다.
	/*
		1.LEFT MOTOR
		PF0 : IN1
		PF1 : IN2
		2.RIGHT MOTOR
		PF2 : IN3
		PF3 : IN4
		
		IN1.IN3   IN2.INT4
		==========   =========
		1               0        : 정회전
		0               1        : 역회전
		1               1        : STOP
	*/

#define MOTOR_DRIVER_PORT PORTF
#define MOTOR_DRIVER_PORT_DDR DDRF

#define MOTOR_DDR DDRB
#define MOTOR_RIGHT_PORT_DDR 5 //OC1A
#define MOTOR_LEFT_PORT_DDR 6 //OC1B


void init_timer0();
void init_uart0(void);
void init_pwm_motor(void);
void stop();
void forward(int speed);
void backward(int speed);
void left(int speed);
void right(int speed);

// 내 파일이아님(led.c에 있어)
extern void init_led();
extern void auto_mode_check();

//stidio.h에 file구조체가 들어있다
// 1. for printf
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit,NULL, _FDEV_SETUP_WRITE);
extern int button1_state;
int main(void)
{
	init_led();
	init_timer0();
	init_uart0();
	init_uart1();
	init_button();
	init_pwm_motor();
	stdout = &OUTPUT; // 2. printf가 동작되도록 stdout에 OUTPUT파일 포인터를 assign한다.
	// fprintf(stdout,"test"); == printf("test\n");
	// scanf("%s",buff); --> stdin
	// fgets(input,100,stdin); //여기서 stdin이 가리키는 것은 키보드입니다.
	
	init_ultrasonic(); // sei전에 초음파관련 initialize
	/* 나중에 주석해체, LCD연결안하면 I2C_LCD_init()에서 갇힌다. */
	//I2C_LCD_init();
	sei(); // 전역적으로 인터럽트를 허용하겠다
	
    while (1) 
    {
		// manual_mode_run(); // bt command로 제어한다.
		auto_mode_check();
		//if(button1_state) {
			//// button1이 on이면
			//auto_drive(); // 자율주행 : 이건 여러분이 coding하세요
		//}
		//ultrasonic_distance_check();
	}
}

void auto_drive(){
	
}
void init_pwm_motor(void)
{
	MOTOR_DRIVER_PORT_DDR |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;  // IN1 IN2 IN3 IN4
	MOTOR_DDR |= 1 << MOTOR_RIGHT_PORT_DDR | 1 << MOTOR_LEFT_PORT_DDR;   // PWM Port
	// 16000000HZ / 64 ==> 250000HZ (timer 1번에 공급 되는 clcok)
	// - 펄스를 265개를 count하면 : 256 / 250000 ==> 1.02ms
	// -       127개             : 127 / 250000 ==> 0.5ms
	// - 0x3ff(1023) --> 4ms
	TCCR1B |= 1 << CS11 | 1 << CS10; //64분주
	
	// 모드 14
	TCCR1B |= 1 << WGM13 | 1 << WGM12;   // 모드14  고속PWM ICR1
	TCCR1A |= 1 << WGM11 | 0 << WGM10;   // 모드14
	
	TCCR1A |= 1 << COM1A1 | 0 << COM1A0;   // 비반전모드 : OCR값을 만나면 PWM 이 LOW가
											//             TOP값을 만나면 PWM이 High
	TCCR1A |= 1 << COM1B1 | 0 << COM1B0;   // 비반전모드
	
	MOTOR_DRIVER_PORT &= ~( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 2 | 1 << 0;  // 전진(정회전) 모드로 set
	//0b11110101가 되려나? // INT2,INT4를 0으로만들어줫슴, INT1,INT3 : 1
	ICR1 = 0x3ff;  // 0x3ff(1023) : 4ms
}


// 'F' : 전진
// 'B' : 후진
// 'L' : 좌회전
// 'R' : 우회전
extern volatile uint8_t uart1_rx_data;

void manual_mode_run(void)
{
	switch (uart1_rx_data)
	{
		case 'F':   // forward
		forward(500);  // 4us x 500 ==> 0.002sec(2ms)
		break;

		case 'B': //backward
		backward(500);
		break;
		
		case 'L': //left
		left(700);
		break;
		
		case 'R': //right
		right(700);
		break;
		
		case 'S': //stop
		stop();
		break;
		
		default:
		break;
	}
}

void forward(int speed)
{
	MOTOR_DRIVER_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 2 | 1 << 0;   // 전진(정회전) 모드로 set
	//0b11110101가 되려나? // INT2,INT4를 0으로만들어줫슴, INT1,INT3 : 1
	OCR1A = speed;   // PB5 PWM출력 left
	OCR1B = speed; 	 // PB6 PWM출력 right
}

void backward(int speed)
{
	MOTOR_DRIVER_PORT &= ~( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 3 | 1 << 1; // 0101 후진 모드로 set
	
	OCR1A = speed;   // PB5 PWM출력 left
	OCR1B = speed; 	 // PB6 PWM출력 right
}

void left(int speed)
{
	MOTOR_DRIVER_PORT &= ~( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 2 | 1 << 0; //정회전 전진 모드로 set
	
	OCR1A = speed; //PB5 PWM 출력 left
	OCR1B = 0; //PB6 PWM 출력 right
}

void right(int speed)
{
	MOTOR_DRIVER_PORT &= ~( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 2 | 1 << 0; //정회전 전진 모드로 set
	
	OCR1A = 0; //PB5 PWM 출력 left
	OCR1B = speed; //PB6 PWM 출력 right
}

void stop()
{
	MOTOR_DRIVER_PORT &= ~( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ); //0b11110000의 뜻
	MOTOR_DRIVER_PORT |= 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0;
	
	OCR1A = 0; //PB5 PWM 출력 left
	OCR1B = 0; //PB6 PWM 출력 right
}


