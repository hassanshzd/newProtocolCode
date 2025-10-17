
#include "uart.h"
#include "stm32f10x.h"


#define rx_len	200

u8 rx_buff[rx_len],*p_rx_buff=rx_buff;
u8 rce_len;

rx_enum rx_flg;


//static void prvSetupUart(void)
//{
//   GPIO_InitTypeDef gpio_uart_pins;

//   //1. Enable the UART2 and GPIOA Peripheral clock
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

//   //PA2 is UART2_TX, PA3 is UART2_RX

//   //2. Alternate function configuration of MCU pins to behave as UART2 TX and RX

//   gpio_uart_pins.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//   gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF_PP;
//   gpio_uart_pins.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(GPIOB, &gpio_uart_pins);

//   //3. AF mode settings for the pins
////	GPIO_Init
////   GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //PA2
////   GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //PA3
//}
void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    // Configure PB10 (TX) as alternate function push-pull
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure PB11 (RX) as input floating
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // USART configuration
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStruct);
    // Enable RX interrupt
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // Enable USART3 IRQ in NVIC
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // Enable USART3
    USART_Cmd(USART3, ENABLE);
}
void USART3_SendChar(char c)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, c);
}

void USART3_SendString(const char *str)
{
    while (*str)
    {
        USART3_SendChar(*str++);
    }
}
void USART3_SendArray(u8* data_addr, u16 len)
{
	u16 inx=0;
	for(inx=0; inx< len; inx++)
	{
		USART3_SendChar(*data_addr++);
	}
}
void USART3_RxIntByte(u8 data_byte)
{
//	*p_rx_buff++=data_byte;
//	if(data_byte == '\r' || data_byte == '\n')
//	{
//		//packet received
//		p_rx_buff=rx_buff;
//		rx_flg = line_cmplt;
//		hndl_rx_pkt();
//	}
//	else //if( cmplted!=line_cmplt)
//	{
//		rx_flg = receiving;
//	}
//	if( (p_rx_buff - rx_buff) > rx_len) p_rx_buff=rx_buff;
	if(rx_flg !=receiving)
	{
		if(data_byte==rx_header)
		{
			rx_flg = receiving;
			*p_rx_buff++=data_byte;
			rce_len++;
		}
	}
	else if(rx_flg == receiving)
	{
		*p_rx_buff++=data_byte;
		rce_len++;
	}
	if(rce_len >= rec_pkt_len)
	{
		if(data_byte== rx_footer)
		{
			rx_flg = cmplted;
			hndl_rx_pkt();
		}
		else
			rx_flg=no_data;
		p_rx_buff=rx_buff;
		rce_len=0;
	}
	if( (p_rx_buff - rx_buff) > rx_len) p_rx_buff=rx_buff;
	
}
void hndl_rx_pkt(void)
{
	//do smt
	received_pkt=(rec_pkt*)&rx_buff;
	req_state=received_pkt->req;
	
}
