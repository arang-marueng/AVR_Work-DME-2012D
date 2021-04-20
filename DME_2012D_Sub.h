/*
*********************************************************************************************************
*                            DME-2012D A/V BOARD Controller User Functions
*
*                          (c) Copyright 2012, MARUENG Co., Ltd., Daejeon, South Korea
*                                           All Rights Reserved
*
* File : DME_2012D_Sub.h
* By   : DongHyun Kwon
*********************************************************************************************************
*/

void MCU_initialize(void)			/* initialize ATmege128 MCU */
{ 

	DDRA	= 0xFF;
	PORTA	= 0x00;

	DDRB	= 0x00;
	PORTB	= 0xFF;

	DDRC	= 0x00;
	PORTC	= 0xFF;

	DDRD	= 0xFF;
	PORTD	= 0x00;

  DDRE	= 0x00;	
  PORTE	= 0xFF;
  
  DDRF	= 0xFF;
  PORTF	= 0x00;
  
  DDRG	= 0x00;
  PORTG	= 0x0F;

}


void Delay_nop(void)		/* time delay for us */
{ 

}


void Delay_us(unsigned char time_us)		/* time delay for us */
{ register unsigned char i;

  for(i = 0; i < time_us; i++)			// 4 cycle +
    { asm volatile(" PUSH  R0 ");		// 2 cycle +
      asm volatile(" POP   R0 ");		// 2 cycle +
//    asm volatile(" PUSH  R0 ");		// 2 cycle +
//    asm volatile(" POP   R0 ");		// 2 cycle +
      asm volatile(" PUSH  R0 ");		// 2 cycle +
      asm volatile(" POP   R0 ");		// 2 cycle = 16 cycle = 1 us for 16MHz
    }
}


void Delay_ms(unsigned int time_ms)		/* time delay for ms */
{ register unsigned int i;

  for(i = 0; i < time_ms; i++)
    { Delay_us(250);
      Delay_us(250);
      Delay_us(250);
      Delay_us(150);
    }
}



void TX0_char(unsigned char data)		/* transmit a character by USART0 */
{
  while((UCSR0A & 0x20) == 0x00);		// data register empty ?
  UDR0 = data;
}


void TX1_char(unsigned char data)		/* transmit a character by USART1 */
{
  while((UCSR1A & 0x20) == 0x00);		// data register empty ?
  UDR1 = data;
}

void Beep(void)
{
	sbi(PORTD,7);
	Delay_ms(50);
	cbi(PORTD,7);
}

