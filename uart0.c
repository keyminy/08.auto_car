#include "def.h"
#include "uart0.h"
#include "extern.h"

// 1.전송속도 : 9600bps(총 byte수 : 
// 1초에 9600bps를 보낼수있으니까, 총 byte글자수는 9600/10 --> 960자
// 1글자 전송하는데 소요되는 시간은 : 약1ms이다

// 2.비동기 방식이다(clock신호에 의지하지않고, 별도의 부호비트[start/stop bit]로 data를 구분하는 방식)
// data는 8bit를하고, none parity방식으로 할 것

// 3.RX인터럽트(수신) : interrupt를 활성화 시킬거야
// 인터럽트벡터 서비스루틴은 선언할 필요없고,제어만 해주면됨
// ISR : HW와 SW의 만남의 장소, 인터럽트가 뜨면 여기로 들어와라
// 1byte를 수신 할때 마다 이곳으로 들어온다.
ISR(USART0_RX_vect){
	// 최적화 방지를위해 ISR내에서는 volatile키워드
	volatile static int i=0;
	// 1byte를 읽어 처리할 로직을 여기넣음
	volatile uint8_t data;
	
	data = UDR0; //UART0의 hardware register(UDR0)로 부터 1byte를 읽어간다.
	//printf("%c\n",data); // 하나찍을때 마다 1ms (다음 데이터 못받을수잇으므로 하지마라)
	if(data == '\r' || data == '\n'){
		rx_Quebuff[rear][i] = '\0'; //문장의 끝을 알리는 null sign을 insert함.
		i = 0; // i를 초기화 : 다음 문장을 입력받을 준비
		rear++;
		rear %= COMMAND_NUMBER;
		
		// Is queue full?
		
	}else{
		rx_Quebuff[rear][i++] = data;
	}
}

void init_uart0(void){
	// 표 9-9(p.219)
	UBRR0H = 0x00;
	UBRR0L = 207; //9600bps설정하는것임 
	//data는 8bit고 non parity설정을하자 : UCSRnA레지스터
	
	UCSR0A |= 1 << U2X0; //2배속 통신
	UCSR0C |= 0x06; // ASYNC(비동기) / data8bit / none parity
	
	// RXEN0 : UART0로 부터 수신이 가능하도록 설정
	// TXEN0 : UART0로 부터 송신이 가능하도록 설정
	// RXCIEN0 : UART0로 부터 1byte가 들어오면(stopbit까지 수신완료된 상황)이면, rx 인터럽트를 발생시킨다.
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
	// shift연산자가 5번 우선순위고, |가 우선순위가 10번이므로 ()치지마라.
}


//UART0를 1byte를 전송하는 함수
void UART0_transmit(uint8_t data){
	while(!(UCSR0A & 1 << UDRE0));
		// 우선순위가 << : 5, & : 8
		//UDRE0는 5비트위치
		//데이터가 전송중이면 전송이 끝날때 까지 기다린다.
		// no operation : NOP
		UDR0 = data; 
}

// UART ISR에서, rx_ready_flag = 1;된후
// command parsing작업 필요
void pc_command_processing()
{
	char* cmd;
	if( front != rear){
		cmd = rx_Quebuff[front]; //&rx_buff[front][0]로 // 2차원 배열의 0번째 주소를 넘겨줌 
		printf("%s\n",cmd); 
		front = (front+1) % 20;	
		
		// Is queue full?
		
		if(strncmp(cmd,"led_all_on",strlen("led_all_on")) == 0){
			LED_PORT = 0xff;
		}else if(strncmp(cmd,"led_all_off",strlen("led_all_off")) == 0){
			LED_PORT = 0x00;
		}else if(strncmp(cmd,"led_all_on_off",strlen("led_all_on_off"))==0){
		
		}else if(strncmp(cmd,"shift_left22right_keep_ledon",strlen("shift_left22right_keep_ledon")) == 0){
		
		}else if(strncmp(cmd,"shift_right22left_keep_ledon",strlen("shift_right22left_keep_ledon")) == 0){
			
		}else if(strncmp(cmd,"flower_on2",strlen("flower_on2"))==0){
			
		}else if(strncmp(cmd,"flower_off2",strlen("flower_off2"))==0){
			
		}
	}
}