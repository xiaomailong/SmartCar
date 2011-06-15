#define LASER_MAX 12          //���������
#define IR_NUM 7              //���������
#define PWM67 1881
#define PWM45 3666
#define ANGLE_DELTA 30
#define PWM6_MID 224
//#define Speed 50
#define N 5                  //�����˲�Ȩֵ		
/*===============ȫ�ֱ�������======================
ȫ�ֱ������ 
*/
  float YP1=0.4,YP2=0.5,YP3=0.7,YP4=0.8,YP5=0.9,YD=2.7;
  float JP1=1.7,JP2=3.5,JP3=4.9,JP4=6.5,JP5=7.8,JD=2.3,SpeedAver,SpeedMax,SpeedMin,SpeedNow;/*ҡͷP1,ҡͷP2,ҡͷP3,ҡͷP4,ҡͷP5,ҡͷD,���P1,���P2,���P3,���P4,���P5,���P6,���D
  ƽ���ٶ�,����ٶ�,��С�ٶ�,��ǰ�ٶ�*/
  int DP1=15,DP2=13,DP3=11,DP4=10,DP5=8,DP6=7,DP7=6;
 
  char SCIreceive[150];                    /*�������ߴ�����ʾ���ַ���*/  
  int temp_pwm67=PWM67;						         //�����ͷ�����ʼֵ
  int temp_pwm45=PWM45;					         	 //ת���ͷ�����ʼֵ
  
  byte light_temp_laser_array[LASER_MAX];  //��ǰ�������Ϣ��������
  uint IR_temp_laser_array[7];             //��ǰ������Ϣ��������
  int  light_count;                        //��������ӳ�
  
  byte special_flag;
  byte start_flag,cross_flag;             //���� ʮ�ֱ�־
  
  //int error;
  
  double temp_speed;
  int delay_count=1;                      //��ͨ��ʱ����
  int send_count=1;                       //���ڽ�����ʱ����
  int q_temp_laser_num[LASER_MAX];        //����ܶ�Ӧ��Ȩֵ
  int countPIT0=0;
  int position;                     //λ�� 
  
  byte right,left,middle;                    //��������ֵ
  int HitBlackNum;
  
  int baitou_diff; 
  
  byte SS_flag;                           //Сs���  1��Ч
  byte LS_flag;                           //��S���   1��Ч
  byte Straight_flag;                     //ֱ�����  ��0��Ч
  byte turn_flag;                         //������  ��0��Ч                 

  int road_point[6];                      //5���Ϊһ��  ����2.5m/s 5��Ϊ1.8cm  road_point[5]Ϊ����ֵ
  int point_count;                        //5����� ͬʱҲ����Ϊ���жϿ�ʼ�ı�־
  int road_section[48];                   //8�ι�Ϊһ��·  ͬ����8��Ϊ15cm
  int section_flag;                      //8�μ���
  
  int  befo_General_pos;
  long General_pos[2];                     //�ۺ�ƫ�� ��������һ���˲� ����100��  01Ϊ��һ��  23Ϊ�ڶ���  3Ϊ�����
  int change_JG_DJ_array[23]={-103,-90,-80,-73,-62,-52,-46,-36,-24,-18,-10,0,10,18,24,36,46,52,62,73,80,90,103}; 
  //int coordinate
 //int standard_position_array[23]=
  //��׼position��ֵ
 // int temp_position_array[22];
  //���ڼ�¼position��ֵ���γ�Ԥ�е����� 
//int last_laser_array[20][11];    
//�����ά������Ϊ����ܵ���ʷ��¼
 
  int dajiao_Slope[3];                   //��Ƕ��������б�� 2Ϊ�ۼ�ֵ
 
  int IR_position[2];                     //����λ��   ���ⲿ�ֱ�������IR��ͷ
  int IR_blacknun=0;                      //����ڵ�
  void Level_IR( void);                   //����  ����ȥ���ɲ�����
  long IR_clear[2];                       //�����˲�ֵ
  
  int  baitou_delay=1;                    //��ͷ�ӳ�  ͬʱ�����ȷְ�ͷ��ÿ�ζ��ֵ
  long JG_clear[4];                      //����һ�ε����˲� �˴κ��ϴ�
  int JG_clear_Pos[2];                  //���뵱ǰ����һ�ΰ�ͷʱ��JG_clear ��ֵ
                             
 // int speed_collect;                     //�ٶȲ�׽ֵ
  int speed_clera[2];                    //�ٶ��˲�ֵ  ���ս�� �˴κ��ϴ�
  //int speed[48];                          //��
  
  void calculate_light(void);
  void Status_Judge(void);
  
 // int g_countPIT0 =0;   //����PIT���ĵ�������
	int	g_temp_laser_array[11]; //���ڽ��ܸ�������ܵ������ȡ������
	
	
	int temp_laser_array[11];
	


/*===============ʱ�ӳ�ʼ��========================*/
void SetBusCLK_40M()
{   
    CLKSEL=0X00;				//disengage PLL to system
    PLLCTL_PLLON=1;			//turn on PLL
    SYNR =0xc0 | 0x04;                        
    REFDV=0x80 | 0x01; 
    POSTDIV=0x00;       //pllclock=2*osc*(1+SYNR)/(1+REFDV)=80MHz;
    _asm(nop);          //BUS CLOCK=40M
    _asm(nop);
    while(!(CRGFLG_LOCK==1));	  //when pll is steady ,then use it;
    CLKSEL_PLLSEL =1;		        //engage PLL to system; 
}     


//==============��ʱ����================================//
void delayms(int ms)    //��ʱ����
{   
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)
     for(jj=0;jj<3338;jj++);    //40MHz--1ms      
}

void delayMS()
{
int jj;
     for(jj=0;jj<833;jj++);    //1/4ms     
}



//===============aabs=================================//
long aabs(long num) {
  if(num>=0)return num;
  else {
    num=-num;
    return num;
  }
}


//=====================PWM��ʼ��======================//
void PWM_Init (void) {   //0519��ʱд�꣡
  // Local Declarations

  // Statements
   PWME = 0X00;       //��ֹPWM���  
   PWMCAE = 0X00;     //�����
   
   PWMCNT01 = 0;			//������01����
   PWMCNT23 = 0;			//������23����
   PWMCNT45 = 0;			//������45����
   PWMCNT67 = 0;			//������67����
 
   PWMPOL = 0XFF;     //������ߵ�ƽ   PWM���ԼĴ���
   PWMCTL = 0XF0;     //ͨ��01.23.45.67����  0B11111111   01��ת 23��ת
   PWMPRCLK = 0X21;   //clockA 2��Ƶ,clockA=busclock/2=20MHz;CLK B 4��Ƶ:10Mhz 
   PWMSCLA = 4;       //��clock SA ����2*4=8��Ƶ��pwm clock=clockA/8=2.5MHz;
   PWMSCLB = 4;       //��clock SB ����2*4=8��Ƶ��pwm clock=clockB/8=1.25MHz;
   PWMCLK_PCLK1 = 1;  //ѡ��clock SA��ʱ��Դ  01
   PWMCLK_PCLK3 = 1;  //ѡ��clock SB��ʱ��Դ  23
   PWMCLK_PCLK5 = 1;  //ѡ��clock SA��ʱ��Դ  45
   PWMCLK_PCLK7 = 1;  //ѡ��clock SB��ʱ��Դ  67	   

   PWMPER01 = 250;    //Ƶ�� 10kHz  ����0.1ms
   PWMPER23 = 125;    //Ƶ�� 10kHz  ����0.1ms
   PWMPER45 = 25000;  //Ƶ�� 100Hz  ����50ms
   PWMPER67 = 12500;  //Ƶ�� 100Hz  ����50ms
   
   PWMDTY01 = 25;      //ռ�ձ�10%
   PWMDTY23 = 80;      //ռ�ձ�50%
   PWMDTY45 = 3666;      //
   PWMDTY67 = 1881;      //ռ�ձ�50%
   PWME_PWME1 = 1;    //ͨ��1���,�����תʹ��   ��ת
   PWME_PWME3 = 1;    //ͨ��3���,�����תʹ�� 
   PWME_PWME5 = 1;    //ͨ��5���,ǰ�ֶ��ʹ��     
   PWME_PWME7 = 1;    //ͨ��7���,��ͷ���ʹ��     
} //PWMInit



//=====================ECT��׽���ٳ�ʼ��======================//
 void Tect_Speed_Init()
 { 
   TSCR1=0x80;          //ʹ�ܶ�ʱ��
   PACTL=0X50;  //PT7���벶׽  
   PACNT=0;
 }



//=====================�����ʼ��======================//
 void LIGHT_Init(void){ 
	DDRA = 0X3F;      //PA0--PA5������źŵ���
	DDRB = 0Xf0;      //PB0--PB3������źŽ���
 }




/*=====================PIT��ʼ��======================*/
 void PITInit (void) {
//���㹫ʽ��time-out period = (PITMTLD + 1) * (PITLD + 1) / fBUS.
//BUSΪ���ߣ�����

    // Local Declarations

    // Statements
  PITCFLMT_PITE = 0;       //PITͨ��ʹ��λ      
  PITCE_PCE0 = 1;          //��ʱ��ͨ��0ʹ��    
  PITMUX_PMUX0 = 0;       //��ʱͨ��0ʹ��΢������0     
  PITMTLD0 =200-1;  //����΢������0�ļ��ؼĴ�����8λ��ʱ����ֵ�趨��200��Ƶ����40MHzBusClock�£�Ϊ0.2MHz����5us.    
  PITLD0 = 300-1;    //16λ��ʱ����ֵ�趨��4000 -->  20ms   1500-->7.5ms  
  PITINTE_PINTE0 = 1;//��ʱ���ж�ͨ��0�ж�ʹ��            
 // PITCFLMT_PITE = 1;       //PITͨ��ʹ��λ

  PITCE_PCE1 = 1;          //��ʱ��ͨ��0ʹ��    
  PITMUX_PMUX1 = 1;       //��ʱͨ��0ʹ��΢������0     
  PITMTLD1 =200-1;  //����΢������0�ļ��ؼĴ�����8λ��ʱ����ֵ�趨��200��Ƶ����40MHzBusClock�£�Ϊ0.2MHz����5us.    
  PITLD1 = 1500-1;    //16λ��ʱ����ֵ�趨��4000 -->  20ms   1500-->7.5ms  
  PITINTE_PINTE1 = 1;//��ʱ���ж�ͨ��0�ж�ʹ��            
  PITCFLMT_PITE = 1;       //PITͨ��ʹ��λ
} //PITInit




/*=====================�������======================
����������ö�ʱ�жϻ��ǡ������Ծͻ������������е���
*/
 void Light_Up(void) {
   for(light_count=0;light_count<=5;light_count++) 
   {
    if(light_count == 0)   
       { 
      PORTA = 0B00000001;
      delayMS();
	    light_temp_laser_array[0] = PORTB_PB0^1;
      light_temp_laser_array[6] = PORTB_PB2^1;
       }   
  
    else if(light_count == 1)   
       { 
      PORTA = 0B00001000;
      delayMS();
	    light_temp_laser_array[3] = PORTB_PB1^1;	
      light_temp_laser_array[9] = PORTB_PB3^1;
       }
    
    else if(light_count ==2)   
       { 
      PORTA = 0B00000010;
      delayMS();
	    light_temp_laser_array[1] = PORTB_PB0^1;
      light_temp_laser_array[7] = PORTB_PB2^1;
       } 
    
    else	if(light_count == 3)       
       { 
      PORTA = 0B00010000;
      delayMS(); 
	    light_temp_laser_array[4] = PORTB_PB1^1;
      light_temp_laser_array[10] = PORTB_PB3^1; 
	     }
   
   else if(light_count == 4)   
      {
      PORTA = 0B00000100;
      delayMS();  
	    light_temp_laser_array[2] = PORTB_PB0^1;
      light_temp_laser_array[8] = PORTB_PB2^1;
      } 
      
   else if(light_count == 5)   
      {
      PORTA = 0B00100000;
      delayMS();  
	    light_temp_laser_array[5] = PORTB_PB1^1;
      light_temp_laser_array[11] = PORTB_PB3^1;  
      }
 PORTA = 0B00000000;         //������ر�ͨ��
   }
light_count=0;   
} 