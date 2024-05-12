#include "def.h"
#include "uart1.h"
#include "extern.h"

// 1.전송속도 : 9600bps
// 1초에 9600bps를 보낼수있으니까, 총 byte글자수는 9600/10 --> 960자
// 1글자 전송하는데 소요되는 시간은 : 약1ms이다

// 2.비동기 방식이다(clock신호에 의지하지않고, 별도의 부호비트[start/stop bit]로 data를 구분하는 방식)
// data는 8bit를하고, none parity방식으로 할 것

// 3.RX인터럽트(수신) : interrupt를 활성화 시킬거야
// 인터럽트벡터 서비스루틴은 선언할 필요없고,제어만 해주면됨
// ISR : HW와 SW의 만남의 장소, 인터럽트가 뜨면 여기로 들어와라
// 1byte를 수신 할때 마다 이곳으로 들어온다.
// 입력패턴예: led_all_on\n
volatile uint8_t uart1_rx_data = 0;
extern void UART0_transmit(uint8_t data);
ISR(USART1_RX_vect){
	uart1_rx_data = UDR1; //UART1의 hardware register(UDR1)로 부터 1byte를 읽어간다.
	UART0_transmit(uart1_rx_data);
}

void init_uart1(void){
	// 표 9-9(p.219)
	UBRR1H = 0x00;
	UBRR1L = 207; //9600bps설정하는것임
	//data는 8bit고 non parity설정을하자 : UCSRnA레지스터
	
	UCSR1A |= 1 << U2X1; //2배속 통신
	UCSR1C |= 0x06; // ASYNC(비동기) / data8bit / none parity
	
	// RXEN1 : UART1로 부터 수신이 가능하도록 설정
	// TXEN1 : UART1로 부터 송신이 가능하도록 설정
	// RXCIEN1 : UART1로 부터 1byte가 들어오면(stopbit까지 수신완료된 상황)이면, rx 인터럽트를 발생시킨다.
	UCSR1B |= 1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1;
	// shift연산자가 5번 우선순위고, |가 우선순위가 10번이므로 ()치지마라.
}


//UART0를 1byte를 전송하는 함수
void UART1_transmit(uint8_t data){
	while(!(UCSR1A & 1 << UDRE1));
	// 우선순위가 << : 5, & : 8
	//UDRE0는 5비트위치
	//데이터가 전송중이면 전송이 끝날때 까지 기다린다.
	// no operation : NOP
	UDR1 = data;
}

// UART ISR에서, rx_ready_flag = 1;된후
// command parsing작업 필요
void bit_command_processing()
{

#if 0 // 이 코드는 테스트용이다. 사용후 반드시 삭제할것

UART1_transmit('9'); //0x61 0100001
_delay_ms(500);

#endif
if (rx1_ready_flag)   // if (rx_ready_flag >= 1)  data가 \n까지 들어 왔으면
{
	rx1_ready_flag=0;
	printf("%s\n", rx1_buff);
	if(strncmp(rx1_buff,"led_all_on_off",strlen("led_all_on_off"))==0)
	{
		PORTA= 0xff;
	}
	else if(strncmp(rx1_buff,"led_all_off",strlen("led_all_off"))==0)
	{
		PORTA= 0x00;
	}
	// 		else if(strncmp(rx_buff,"flower_on",strlen("flower_on"))==0)
	// 		{
	// 			flower_on();
	// 		}
	// 		else if(strncmp(rx_buff,"flower_off",strlen("flower_off"))==0)
	// 		{
	// 			flower_off();
	// 		}
	// 		else if(strncmp(rx_buff,"shift_left2right_keep_ledon",strlen("shift_left2right_keep_ledon"))==0)
	// 		{
	// 			shift_left2right_keep_ledon();
	// 		}
	// 		else if(strncmp(rx_buff,"shift_right2left_keep_ledon",strlen("shift_right2left_keep_ledon"))==0)
	// 		{
	// 			shift_right2left_keep_ledon();
	// 		}
}
}