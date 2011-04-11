#include <hidef.h>      /* common defines and macros */
#include <MC9S12XS128.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12xs128"

unsigned char RX=0,temp0;


void uart_putchar(unsigned char ch)
{ 
  if (ch == '\n')  
  {
      while(!(SCI0SR1&0x80)) ;     
      SCI0DRL= 0x0d;            
     return;
  }
  while(!(SCI0SR1&0x80)) ;       
  SCI0DRL=ch;
}

static void PWM_Init(void)
{     
    //SB,B for ch2367
    //SA,A for ch0145   
      
    PWMCTL_CON01=1;		  //0和1联合成16位PWM；
    PWMCAE_CAE1=0;			//选择输出模式为左对齐输出模式
    PWMCNT01 = 0;				//计数器清零；
    PWMPOL_PPOL1=1;			//先输出高电平，计数到DTY时，反转电平
    PWMPRCLK = 0X40;    //clockA不分频,clockA=busclock=16MHz;CLK B 16分频:1Mhz   
    PWMSCLA = 8;        //对clock SA 进行2*8=16分频；pwm clock=clockA/16=1MHz;     
    PWMCLK_PCLK1 = 1;   //选择clock SA做时钟源
    PWMPER01  =  20000;   //周期20ms； 50Hz;(可以使用的范围：50-200hz)
    PWMDTY01  =  1500;   //高电平时间为1.5ms;  
    PWME_PWME1 = 1;        
} 

void setbusclock(void)   //PLL setting
{   
    CLKSEL=0X00;    //disengage PLL to system
    PLLCTL_PLLON=1;   //turn on PLL
    SYNR=1;          
    REFDV=1;          //pllclock=2*osc*(1+SYNR)/(1+REFDV)=32MHz;
    _asm(nop);          //BUS CLOCK=16M
    _asm(nop);
    while(!(CRGFLG_LOCK==1));   //when pll is steady ,then use it;
    CLKSEL_PLLSEL =1;          //engage PLL to system; 
}

static void SCI_Init(void)  //SCI
{
    SCI0CR1=0x00;
    SCI0CR2=0x2c; //enable Receive Full Interrupt,RX enable,Tx enable
    SCI0BDH=0x00; //busclk  8MHz,19200bps,SCI0BDL=0x1a
    SCI0BDL=0x68; //SCI0BDL=busclk/(16*SCI0BDL)                   
                  //busclk 16MHz, 9600bps,SCI0BDL=0x68                   
}

interrupt 20 void SCI_Rx_IRS(void)    //中断接收
{ 
    unsigned char  RxData;
    DisableInterrupts;
    if  (SCI0SR1&0x20) 
    {
        
       RxData = SCI0DRL;  
          
       switch(RX) 
       {
          case 0:
            if (RxData==0xfe) RX=1;
            break;
          case 1:
            temp0 = RxData;
            RX=2;
            break;
          case 2:
             PWMDTY01= ((unsigned int)temp0 <<8) | RxData;
             PORTB=~PORTB;
             RX=0;
             break;           
       }
        uart_putchar(RxData); 
    }    
    EnableInterrupts;      
} 

void main(void) 
{
  EnableInterrupts;
  setbusclock();
  SCI_Init();
  PWM_Init();
  DDRB=0xff;
  for(;;)   {
  }
}