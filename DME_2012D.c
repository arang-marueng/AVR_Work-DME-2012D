/*
*********************************************************************************************************
*                            DME-2012D A/V BOARD Controller Main Functions
*
*                          (c) Copyright 2012, MARUENG co., ltd., Daejeon, South Korea
*                                           All Rights Reserved
*
* File : DME_2012D.C
* By   : DongHyun Kwon
*********************************************************************************************************

*********************************************************************************************************
  ATMEGA128 Fuse Bits
  /BootLock12 /BootLock11  /BootLock02  /BootLock01  /Lock2 /Lock1
                       /M103C  /WDTON
  /OCDEN  /JTAGEN *CKOPT  *EESAVE /BOOTSZ1  /BOOTSZ0  /BOOTRST
  *BODLEVEL /BODEN  *SUT1 /SUT0 /CKSEL3 /CKSEL2 /CKSEL1 /CKSEL0
*********************************************************************************************************      
*/

// External 11.0592MHz

#include <avr/io.h>
#include <DME_2012D_Sub.h>
#include <avr/interrupt.h>
#include <avr/math.h>
#include <avr/signal.h>
#include <string.h>
#include <stdlib.h>


unsigned char ETI_LOS,ETI_LOS_BUF,ETI_LOS_TEMP=0xFF;
unsigned int  i,j,Time_Count=10,Serial_Count0,Buf_Count0,Serial_Time_Count0=100,Time_Flag=0;
unsigned int  Serial_Count1,Buf_Count1,Serial_Time_Count1=100;
unsigned char Key_Input,Key_Flag=0,Remote_Flag=0;
unsigned char SW_HOME_A,SW_HOME_B;

unsigned char RXD_BUF0[30],RXD_BUF1[30];

unsigned char INPUT_TYPE_A,INPUT_TYPE_B;
unsigned char E_SYNC_A,E_FCNT_A,E_FCRC_A,E_HCRC_A,AUDIO_MUTE_A;
unsigned char E_SYNC_B,E_FCNT_B,E_FCRC_B,E_HCRC_B,AUDIO_MUTE_B;

unsigned char RESET_FLAG=0,READY_FLAG=2;

unsigned char RM_RST_KEY,RM_RST_KEY_Flag;
unsigned char Key_Clear,Key_Clear_Flag;

unsigned char LED_TIME_Count;

unsigned char ETI_LOS_BUF0,ETI_LOS_BUF1;

unsigned long	Log_LED_Clear_Time_Count;



SIGNAL(SIG_UART0_RECV)
{
  cli();
	RXD_BUF0[Serial_Count0]=UDR0;
	
	Serial_Count0++;

	if((RXD_BUF0[2] == 0x00) && (RXD_BUF0[3] == 0x01)  && (Serial_Count0 == 13)){

		// Led Status Data
		INPUT_TYPE_A = RXD_BUF0[6];
		E_SYNC_A = RXD_BUF0[7];
		E_FCNT_A = RXD_BUF0[8];
		E_FCRC_A = RXD_BUF0[9];
		E_HCRC_A = RXD_BUF0[10];
		
		if(INPUT_TYPE_A == 0)
		{
			//sbi(PORTF,5);cbi(PORTF,6);cbi(PORTF,7);// NI
			sbi(PORTA,6);cbi(PORTA,5);cbi(PORTA,4);// NI
		
		}else if(INPUT_TYPE_A == 1){
			
			//cbi(PORTF,5);sbi(PORTF,6);cbi(PORTF,7);// NA 5376
			cbi(PORTA,6);sbi(PORTA,5);cbi(PORTA,4);// NA 5376
			
		}else if(INPUT_TYPE_A == 2){
			
			//cbi(PORTF,5);cbi(PORTF,6);sbi(PORTF,7);// NA 5592
			cbi(PORTA,6);cbi(PORTA,5);sbi(PORTA,4);// NA 5592
		}

		if(E_SYNC_A == 1){	sbi(PORTA,1);sbi(PORTA,0);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}
		if(E_FCNT_A == 1){	sbi(PORTF,5);sbi(PORTF,4);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}
		if(E_FCRC_A == 1){	sbi(PORTF,3);sbi(PORTF,2);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	} 
		if(E_HCRC_A == 1){	sbi(PORTF,1);sbi(PORTF,0);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}


		for(Buf_Count0=0;Buf_Count0<Serial_Count0;Buf_Count0++)
		{
			RXD_BUF0[Buf_Count0]=0x00;
		}

		Serial_Count0=0;
		Serial_Time_Count0=0;


	}else if((RXD_BUF0[2] == 0x00) && (RXD_BUF0[3] == 0x02)  && (Serial_Count0 == 8)){

		// LOS Status Data
		READY_FLAG =1;
		
		TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x12);TX0_char(0x00);TX0_char(0x01);TX0_char(ETI_LOS_BUF0);TX0_char(0x1B);TX0_char(0x03);

		for(Buf_Count0=0;Buf_Count0<Serial_Count0;Buf_Count0++)
		{
			RXD_BUF0[Buf_Count0]=0x00;
		}

		Serial_Count0=0;
		Serial_Time_Count0=0;

	}else if((RXD_BUF0[2] == 0x00) && (RXD_BUF0[3] == 0x03)  && (Serial_Count0 == 9)){

		AUDIO_MUTE_A = RXD_BUF0[6];
		
		if(AUDIO_MUTE_A == 0){ sbi(PORTD,6);	}
			
			
		
		for(Buf_Count0=0;Buf_Count0<Serial_Count0;Buf_Count0++)
		{
			RXD_BUF0[Buf_Count0]=0x00;
		}

		Serial_Count0=0;
		Serial_Time_Count0=0;

	}
	

	if((RXD_BUF0[0] != 0x1B) && (RXD_BUF0[1] != 0x02))
	{
		Serial_Count0=0;
		Serial_Time_Count0=0;
	}
 
  sei();

}	// end of SIGNAL(SIG_UART0_RECV)



SIGNAL(SIG_UART1_RECV)
{
  cli();
	RXD_BUF1[Serial_Count1]=UDR1;
	
	Serial_Count1++;

	if((RXD_BUF1[2] == 0x00) && (RXD_BUF1[3] == 0x01)  && (Serial_Count1 == 13)){

		// Led Status Data
		INPUT_TYPE_B = RXD_BUF1[6];
		E_SYNC_B = RXD_BUF1[7];
		E_FCNT_B = RXD_BUF1[8];
		E_FCRC_B = RXD_BUF1[9];
		E_HCRC_B = RXD_BUF1[10];
		
		if(INPUT_TYPE_B == 0)
		{
			//sbi(PORTF,5);cbi(PORTF,6);cbi(PORTF,7);// NI
			sbi(PORTA,6);cbi(PORTA,5);cbi(PORTA,4);// NI
		
		}else if(INPUT_TYPE_B == 1){
			
			//cbi(PORTF,5);sbi(PORTF,6);cbi(PORTF,7);// NA 5376
			cbi(PORTA,6);sbi(PORTA,5);cbi(PORTA,4);// NA 5376
			
		}else if(INPUT_TYPE_B == 2){
			
			//cbi(PORTF,5);cbi(PORTF,6);sbi(PORTF,7);// NA 5592
			cbi(PORTA,6);cbi(PORTA,5);sbi(PORTA,4);// NA 5592
		}

		if(E_SYNC_B == 1){	sbi(PORTA,1);sbi(PORTA,0);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}
		if(E_FCNT_B == 1){	sbi(PORTF,5);sbi(PORTF,4);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}
		if(E_FCRC_B == 1){	sbi(PORTF,3);sbi(PORTF,2);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	} 
		if(E_HCRC_B == 1){	sbi(PORTF,1);sbi(PORTF,0);LED_TIME_Count=0;Log_LED_Clear_Time_Count=0;	}


		for(Buf_Count1=0;Buf_Count1<Serial_Count1;Buf_Count1++)
		{
			RXD_BUF1[Buf_Count1]=0x00;
		}

		Serial_Count1=0;
		Serial_Time_Count1=0;


	}else if((RXD_BUF1[2] == 0x00) && (RXD_BUF1[3] == 0x02)  && (Serial_Count1 == 8)){

		// LOS Status Data
		READY_FLAG =1;
		
		TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x12);TX1_char(0x00);TX1_char(0x01);TX1_char(ETI_LOS_BUF1);TX1_char(0x1B);TX1_char(0x03);

		for(Buf_Count1=0;Buf_Count1<Serial_Count1;Buf_Count1++)
		{
			RXD_BUF1[Buf_Count1]=0x00;
		}

		Serial_Count1=0;
		Serial_Time_Count1=0;

	}else if((RXD_BUF1[2] == 0x00) && (RXD_BUF1[3] == 0x03)  && (Serial_Count1 == 9)){

		AUDIO_MUTE_A = RXD_BUF1[6];
		
		if(AUDIO_MUTE_A == 0){ sbi(PORTD,6);	}
			
			
		
		for(Buf_Count1=0;Buf_Count1<Serial_Count1;Buf_Count1++)
		{
			RXD_BUF1[Buf_Count1]=0x00;
		}

		Serial_Count1=0;
		Serial_Time_Count1=0;

	}
	

	if((RXD_BUF1[0] != 0x1B) && (RXD_BUF1[1] != 0x02))
	{
		Serial_Count1=0;
		Serial_Time_Count1=0;
	}
 
  sei();

}	// end of SIGNAL(SIG_UART0_RECV)



SIGNAL(SIG_OUTPUT_COMPARE1A)            //OC1A interrupt function 
{

  Time_Flag=1;
  Time_Count++;
	Serial_Time_Count0++;
	Serial_Time_Count1++;
	LED_TIME_Count++;
	Log_LED_Clear_Time_Count++;
	
	if(LED_TIME_Count == 10)
	{
		//LED OFF
		cbi(PORTA,0);
		cbi(PORTF,0);
		cbi(PORTF,2);
		cbi(PORTF,4);
	}


	if(Log_LED_Clear_Time_Count == 72000)// 2 hours
	{
		//Log LED OFF
		//cbi(PORTA,1);
		//cbi(PORTF,1);
		//cbi(PORTF,3);
		//cbi(PORTF,5);
	}

	if(Serial_Time_Count0 >= 5)
	{
			Serial_Count0=0;
			for(Buf_Count0=0;Buf_Count0<30;Buf_Count0++)
			{
				RXD_BUF0[Buf_Count0]=0x00;
			}
	}

	if(Serial_Time_Count1 >= 5)
	{
			Serial_Count1=0;
			for(Buf_Count1=0;Buf_Count1<30;Buf_Count1++)
			{
				RXD_BUF1[Buf_Count1]=0x00;
			}
	}

 if(Time_Count >= 10)
 {
 		Time_Count = 10;
	}

 if(Time_Count >= 10)
 {
 		Time_Count = 10;
	}


 if(Log_LED_Clear_Time_Count > 72000)
 {
 		Log_LED_Clear_Time_Count = 73000;
	}

 if(Serial_Time_Count0 >= 100)
 {
 		Serial_Time_Count0 = 100;
	}

 if(Serial_Time_Count1 >= 100)
 {
 		Serial_Time_Count1 = 100;
	}
	

}	// end of SIGNAL(SIG_OUTPUT_COMPARE1A)


int main(void)
{ 

  MCU_initialize(); // initialize MCU
  Delay_ms(100);

	sbi(PORTD,7);//buzzer on
	//Led On
	PORTA = 0xFF;
	PORTF = 0xFF;
	
// Timer 1 setting
  TCCR1A = 0x00;                                // CTC mode(4)
  TCCR1B = 0x0C;                                // 11.0592MHz/256/(1+21599) = 2Hz       4319 = 10Hz   43199 = 1Hz
  OCR1AH = (4319  >> 8);
  OCR1AL = 4319 & 0xFF;
  TCNT1H = 0x00;                                // clear Timer/Counter1
  TCNT1L = 0x00;
  TIMSK	 = 0x10;                                 // enable OC1A interrupt
  TIFR	 = 0x00;                                  // clear all interrupt flags



// USART0 Setting 
  UCSR0A = 0x00;
  UCSR0B = 0x98;
  UCSR0C = 0x06;				// 8data, no parity, 1 stop
  UBRR0H = (35 >> 8);				// 19200 baud rate
  UBRR0L = 35 & 0xFF;


// USART1 Setting 
  UCSR1A = 0x00;
  UCSR1B = 0x98;
  UCSR1C = 0x06;				// 8data, no parity, 1 stop
  UBRR1H = (35 >> 8);				// 19200 baud rate
  UBRR1L = 35 & 0xFF;


	Delay_ms(500);

//LED Off
	PORTA = 0x00;
	PORTF = 0x00;

	cbi(PORTD,7);//buzzer off
	
	SW_HOME_A =0;
	SW_HOME_B =0;
	
	Log_LED_Clear_Time_Count=0;

  sei();

while(1)
	{

	Delay_ms(10);

	if(Time_Flag ==1)
	{
		Time_Flag =0;

	  //ETI_LOS=inp(PINE) & 0x40;
	  //if(ETI_LOS == 0x40){	ETI_LOS_BUF = 1; } else{ ETI_LOS_BUF = 0; }
	  ETI_LOS=inp(PINE) & 0xC0;
	  if((ETI_LOS & 0x40) == 0x40){	ETI_LOS_BUF0 = 1; } else{ ETI_LOS_BUF0 = 0; }
	  if((ETI_LOS & 0x80) == 0x80){	ETI_LOS_BUF1 = 1; } else{ ETI_LOS_BUF1 = 0; }
	 

		if(ETI_LOS != ETI_LOS_TEMP)
		{
			ETI_LOS_TEMP = ETI_LOS;
	
			if(ETI_LOS == 0xC0)
		  {
				Beep();//ETI ON
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x12);TX0_char(0x00);TX0_char(0x01);TX0_char(ETI_LOS_BUF0);TX0_char(0x1B);TX0_char(0x03);
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x12);TX1_char(0x00);TX1_char(0x01);TX1_char(ETI_LOS_BUF1);TX1_char(0x1B);TX1_char(0x03);
				cbi(PORTD,5);
				cbi(PORTD,6);
				sbi(PORTA,3);
				cbi(PORTA,2);
		  	
		  }else{
		  	Beep();//ETI OFF
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x12);TX0_char(0x00);TX0_char(0x01);TX0_char(ETI_LOS_BUF0);TX0_char(0x1B);TX0_char(0x03);
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x12);TX1_char(0x00);TX1_char(0x01);TX1_char(ETI_LOS_BUF1);TX1_char(0x1B);TX1_char(0x03);
				sbi(PORTD,5);
		  	sbi(PORTD,6);
		  	sbi(PORTD,7);
		  	cbi(PORTA,3);
		  	sbi(PORTA,2);
		 
			}
		}
	
	
		Key_Input= (inp(PINB) & 0xF0) | (inp(PINC) & 0x0F);
		RM_RST_KEY = inp(PINE) & 0x20;
		Key_Clear =  inp(PING) & 0x01;
	
		if((RM_RST_KEY == 0x00) && (Remote_Flag == 0))
		{
				Beep();
				Remote_Flag =1;
				sbi(PORTD,7);//buzzer on
				//Led On
				PORTA = 0xFF;
				PORTF = 0xFF;
				Delay_ms(500);
				//LED Off
				PORTA = 0x00;
				PORTF = 0x00;
				cbi(PORTD,7);//buzzer off
				cbi(PORTD,5);// ETI Fault
				cbi(PORTD,6);// Buzzer Out
	
				ETI_LOS_TEMP=0xFF;
				Key_Flag =0;			
				SW_HOME_A =0;
				SW_HOME_B =0;
	
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x06);TX0_char(0x1B);TX0_char(0x03);
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x06);TX1_char(0x1B);TX1_char(0x03);
					
		}else if(RM_RST_KEY == 0x20){
				
				Remote_Flag =0;
		}

		
		if((Key_Flag ==0) && (Key_Clear != 0x00))	
		{
			if((Key_Input == 0x9F) && (READY_FLAG==1)){
				//Service A Up_Down SW LCD On/Off
				Key_Flag =1;
				if(SW_HOME_A == 0)
				{
					TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x07);TX0_char(0x1B);TX0_char(0x03);
					Beep();
	
				}
	
			}else if((Key_Input == 0xF9) && (READY_FLAG==1)){
				//Service B Up_Down SW LCD On/Off
				Key_Flag =1;
				if(SW_HOME_B == 0)
				{
					TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x07);TX1_char(0x1B);TX1_char(0x03);
					Beep();
	
				}
	
			}else	if((Key_Input == 0xEF) && (READY_FLAG==1)){
				//Service A Home SW
				Key_Flag =1;
				RM_RST_KEY_Flag=0;
				SW_HOME_A =1;
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x01);TX0_char(0x1B);TX0_char(0x03);
				Beep();

			}else	if((Key_Input == 0xF7) && (READY_FLAG==1)){
				//Service B Home SW
				Key_Flag =1;
				RM_RST_KEY_Flag=0;
				SW_HOME_B =1;
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x01);TX1_char(0x1B);TX1_char(0x03);
				Beep();

			}else if((Key_Input == 0xDF) && (READY_FLAG==1)){
				//Service A UP SW
				Key_Flag =1;
				if(SW_HOME_A == 1)
				{
					TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x02);TX0_char(0x1B);TX0_char(0x03);
					Beep();
					
				}
	
			}else if((Key_Input == 0xBF) && (READY_FLAG==1)){
				//Service A Down SW
				Key_Flag =1;
				if(SW_HOME_A == 1)
				{
					TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x03);TX0_char(0x1B);TX0_char(0x03);
					Beep();
					
				}
	
			}else if((Key_Input == 0xFB) && (READY_FLAG==1)){
				//Service B UP SW
				Key_Flag =1;
				if(SW_HOME_B == 1)
				{
					TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x02);TX1_char(0x1B);TX1_char(0x03);
					Beep();
				}
	
			}else if((Key_Input == 0xFD) && (READY_FLAG==1)){
				//Service B Down SW
				Key_Flag =1;
				if(SW_HOME_B == 1)
				{
					TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x03);TX1_char(0x1B);TX1_char(0x03);
					Beep();
				}
	
			}else if((Key_Input == 0x7F) && (READY_FLAG==1)){
				//Service A Select SW
				Key_Flag =1;
				if(SW_HOME_A == 1)
				{
					TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x05);TX0_char(0x1B);TX0_char(0x03);
					Beep();
					SW_HOME_A=0;
					
				}
	
			}else if((Key_Input == 0xFE) && (READY_FLAG==1)){
				//Service B Select SW
				Key_Flag =1;
				if(SW_HOME_B == 1)
				{
					TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x05);TX1_char(0x1B);TX1_char(0x03);
					Beep();
					SW_HOME_B=0;
				}
			}

		}//if(Key_Flag ==0)	
		
		if((Key_Clear == 0x00) && (Key_Clear_Flag==0))
		{

		  	Key_Clear_Flag =1;
		  	cbi(PORTA,3); //ETI ON
		  	cbi(PORTA,2); //ETI OFF
		  	
				cbi(PORTA,1); //Sync
				cbi(PORTA,0); //Sync

				cbi(PORTF,4); //Fcnt
				cbi(PORTF,2); //Fcrc
				cbi(PORTF,0); //Hcrc

				cbi(PORTF,5); //Fcnt
				cbi(PORTF,3); //Fcrc
				cbi(PORTF,1); //Hcrc

				cbi(PORTD,5);// ETI Fault
				cbi(PORTD,6);// Buzzer Out
  			
  			ETI_LOS=inp(PINE) & 0xC0;
  			if(ETI_LOS == 0xC0){	sbi(PORTA,3); } else{ sbi(PORTA,2); }
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x04);TX0_char(0x1B);TX0_char(0x03);
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x04);TX1_char(0x1B);TX1_char(0x03);

				Beep();
		}
		if(Key_Clear == 0x01){	Key_Clear_Flag =0; RM_RST_KEY_Flag=0;}
		if(Key_Input == 0xFF){	Key_Flag =0; }

		if((Key_Clear == 0x00) && (Key_Input==0xF9)&& (RM_RST_KEY_Flag==0))
		{
				RM_RST_KEY_Flag =1;
				sbi(PORTD,7);//buzzer on
				//Led On
				PORTA = 0xFF;
				PORTF = 0xFF;
				Delay_ms(500);
				//LED Off
				PORTA = 0x00;
				PORTF = 0x00;
				cbi(PORTD,7);//buzzer off
				cbi(PORTD,5);// ETI Fault
				cbi(PORTD,6);// Buzzer Out
	
				ETI_LOS_TEMP=0xFF;
				Key_Flag =0;			
				SW_HOME_A =0;
				SW_HOME_B =0;
	
				TX0_char(0x1B);TX0_char(0x02);TX0_char(0x00);TX0_char(0x11);TX0_char(0x00);TX0_char(0x01);TX0_char(0x06);TX0_char(0x1B);TX0_char(0x03);
				TX1_char(0x1B);TX1_char(0x02);TX1_char(0x00);TX1_char(0x11);TX1_char(0x00);TX1_char(0x01);TX1_char(0x06);TX1_char(0x1B);TX1_char(0x03);

				Beep();
		}

				
	}//if(Time_Flag ==1)
 }		// end of while
}			// end of main


