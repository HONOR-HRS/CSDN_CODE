#include <REG51.H>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit  DS1302_CLK = P2^4;              //ʵʱʱ��ʱ�������� 
sbit  DS1302_IO  = P2^5;              //ʵʱʱ������������ 
sbit  DS1302_RST = P2^6;              //ʵʱʱ�Ӹ�λ������
sbit  DQ = P2^7;                      //�¶ȴ�������IO��
sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;
uchar hide_sec,hide_min,hide_hour,hide_day,hide_week,hide_month,hide_year;  //��,��,ʱ����,��,��λ���ļ���
sbit Set = P1^0;       //ģʽ�л��� ��Ӧʵ���SW19
sbit Up  = P1^1;        //�ӷ���ť ��Ӧʵ���SW20
sbit Down= P1^2;      //������ť  ��Ӧʵ���SW21
sbit out = P1^3;       //������������ģʽ��ť ��Ӧʵ���SW22
uchar done,count,temp,up_flag,down_flag;
uchar temp_value,temp1_value;      //temp_value�¶�ֵ�������� temp1_value�¶�ֵС������ 
uchar TempBuffer[8],week_value[3];
void show_time();   //Һ����ʾ����
//****************************************
//12864Һ����ʾ�����ӳ���ģ��
//****************************************
sbit rs		= P3^0;
sbit rw		= P3^1;
sbit e  	= P3^2;
#define lcddata P0
sbit busy=P0^7;   //lcd busy bit
void wr_d_lcd(uchar content);
void wr_i_lcd(uchar content);
void clrram_lcd (void);
void init_lcd(void);
void busy_lcd(void);
void rev_row_lcd(uchar row);
void rev_co_lcd(uchar row,uchar col,uchar mode);
void clr_lcd(void);
void wr_co_lcd(uchar row,uchar col,uchar lcddata1,uchar lcddtta2);
void wr_row_lcd(uchar row,char *p);
//**********************************
//Һ����ʼ��
//**********************************
void init_lcd(void)
{
	wr_i_lcd(0x06);  /*�����ƶ�����*/
	wr_i_lcd(0x0c);  /*����ʾ�����α�*/
}
//***********************************
//���Һ��DDRAMȫΪ�ո�
//**********************************
void clrram_lcd (void)
{
	wr_i_lcd(0x30);
	wr_i_lcd(0x01);
}
//***********************************
//��Һ��д����
//contentΪҪд�������
//***********************************
void wr_d_lcd(uchar content)
{
	busy_lcd();
	rs=1;
    rw=0;
	lcddata=content;
	e=1;
	;
	e=0;
}
//********************************
//��Һ��дָ��
//contentΪҪд���ָ�����
//*****************************
void wr_i_lcd(uchar content)
{
	busy_lcd();
	rs=0;
    rw=0;
	lcddata=content;
	e=1;
	;
	e=0;
}
//********************************
//Һ�����æ״̬
//��д��֮ǰ����ִ��
//********************************
void busy_lcd(void)
{
  lcddata=0xff;
  rs=0;
  rw=1;
  e =1;
  while(busy==1);
  e =0;
}
//********************************
//ָ��Ҫ��ʾ�ַ�������
//*******************************
void gotoxy(unsigned char y, unsigned char x)
{
	if(y==1)
		wr_i_lcd(0x80|x);
	if(y==2)
        wr_i_lcd(0x90|x);
	if(y==3)
		wr_i_lcd((0x80|x)+8);
	if(y==4)
        wr_i_lcd((0x90|x)+8);
}
//**********************************
//Һ����ʾ�ַ�������
//**********************************
void print(uchar *str)
{
	while(*str!='\0')
	{
		wr_d_lcd(*str);
		str++;
	}
}






//***********************************
//DS1302ʱ�Ӳ����ӳ���ģ��
//***********************************
typedef struct __SYSTEMTIME__
{
	uchar Second;
	uchar Minute;
	uchar Hour;
	uchar Week;
	uchar Day;
	uchar Month;
	uchar  Year;
	uchar DateString[11];
	uchar TimeString[9];
}SYSTEMTIME;	//�����ʱ������
SYSTEMTIME CurrentTime;


#define AM(X)	X
#define PM(X)	(X+12)            	  // ת��24Сʱ��
#define DS1302_SECOND	0x80          //ʱ��оƬ�ļĴ���λ��,���ʱ��
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84 
#define DS1302_WEEK		0x8A
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_YEAR		0x8C 
//**********************************
//ʵʱʱ��д��һ�ֽ�(�ڲ�����)
//**********************************
void DS1302InputByte(uchar d) 	
{ 
    uchar i;
    ACC = d;
    for(i=8; i>0; i--)
    {
        DS1302_IO = ACC0;           	//�൱�ڻ���е� RRC
        DS1302_CLK = 1;
        DS1302_CLK = 0;
        ACC = ACC >> 1; 
    } 
}
//*************************************
//ʵʱʱ�Ӷ�ȡһ�ֽ�(�ڲ�����)
//*************************************
uchar DS1302OutputByte(void) 	
{ 
    uchar i;
    for(i=8; i>0; i--)
    {
        ACC = ACC >>1;         			//�൱�ڻ���е� RRC 
        ACC7 = DS1302_IO;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
    } 
    return(ACC); 
}
//**************************************
//ucAddr: DS1302��ַ, ucData: Ҫд������
//**************************************
void Write1302(uchar ucAddr, uchar ucDa)	
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr);       	// ��ַ������ 
    DS1302InputByte(ucDa);       	// д1Byte����
    DS1302_CLK = 1;
    DS1302_RST = 0;
} 
//**************************************
//��ȡDS1302ĳ��ַ������
//**************************************
uchar Read1302(uchar ucAddr)	
{
    uchar ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr|0x01);        // ��ַ������ 
    ucData = DS1302OutputByte();         // ��1Byte����
    DS1302_CLK = 1;
    DS1302_RST = 0;
    return(ucData);
}

//******************************************
//��ȡʱ��оƬ��ʱ�����ݵ��Զ���Ľṹ������
//******************************************
void DS1302_GetTime(SYSTEMTIME *Time)  
{
	uchar ReadValue;
	ReadValue = Read1302(DS1302_SECOND);
	Time->Second = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);//ת��Ϊ��Ӧ��10������
	ReadValue = Read1302(DS1302_MINUTE);
	Time->Minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_HOUR);
	Time->Hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_DAY);
	Time->Day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	
	ReadValue = Read1302(DS1302_WEEK);
	Time->Week = ((ReadValue&0x10)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MONTH);
	Time->Month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_YEAR);
	Time->Year = ((ReadValue&0xf0)>>4)*10 + (ReadValue&0x0F);	
}
//******************************************
//��ʱ����,��,��,��������ת����Һ
//����ʾ�ַ���,�ŵ�������DateString[]
//******************************************
void DateToStr(SYSTEMTIME *Time)    
{   
   uchar tab[ ]={0XD2,0XBB,0XB6,0XFE,0XC8,0XFD,0XCB,0XC4,0XCE,0XE5,0XC1,0XF9,0XC8,0XD5};
   if(hide_year<2)          //�����if,else��䶼���ж�λ��˸,<2��ʾ����,>2�Ͳ���ʾ,����ַ���Ϊ 2007/07/22
    {                               
	  Time->DateString[0] = '2';
	  Time->DateString[1] = '0';	 
	  Time->DateString[2] = Time->Year/10 + '0';
	  Time->DateString[3] = Time->Year%10 + '0';
	}
	  else
	    { 
	      Time->DateString[0] = ' ';
	      Time->DateString[1] = ' ';		 
	      Time->DateString[2] = ' ';
	      Time->DateString[3] = ' ';
		}
    Time->DateString[4]='-';
	if(hide_month<2)
	{
	  Time->DateString[5] = Time->Month/10 + '0';
	  Time->DateString[6] = Time->Month%10 + '0';
	}
	  else
	  {
	    Time->DateString[5] = ' ';
	    Time->DateString[6] = ' ';
	  }
    Time->DateString[7]='-';
	if(hide_day<2)
	{
	  Time->DateString[8] = Time->Day/10 + '0';
	  Time->DateString[9] = Time->Day%10 + '0';
	}
	  else
	  {
	    Time->DateString[8] = ' ';
	    Time->DateString[9] = ' ';	    
	  }
	if(hide_week<2)
	{
	  week_value[0] =tab[2*(Time->Week%10)-2];  //���ڵ���������ŵ� week_value[]������,����,��,�յķֿ����,��Ϊ��һ��Ҫ�������ʾ
	  week_value[1] =tab[2*(Time->Week%10)-1];
	}
	  else
	  {
	    week_value[0] = ' ';
		week_value[1]=' ';
	  }
	  week_value[2] = '\0';

	Time->DateString[10] = '\0'; //�ַ���ĩβ�� '\0' ,�жϽ����ַ�
}
//******************************************
//��ʱ,��,������ת����Һ��
//��ʾ�ַ��ŵ����� TimeString[]
//*****************************************
void TimeToStr(SYSTEMTIME *Time) 
{   if(hide_hour<2)
    {
	  Time->TimeString[0] = Time->Hour/10 + '0';
	  Time->TimeString[1] = Time->Hour%10 + '0';
	}
	  else
	    {
	      Time->TimeString[0] = ' ';
	      Time->TimeString[1] = ' ';
		}
	Time->TimeString[2] = ':';
    if(hide_min<2)
	{
	  Time->TimeString[3] = Time->Minute/10 + '0';
	  Time->TimeString[4] = Time->Minute%10 + '0';
	}
	  else
	    {
	      Time->TimeString[3] = ' ';
	      Time->TimeString[4] = ' ';
   	    }
	Time->TimeString[5] = ':';
    if(hide_sec<2)
    {
	  Time->TimeString[6] = Time->Second/10 + '0';
	  Time->TimeString[7] = Time->Second%10 + '0';
    }
      else
       {
         Time->TimeString[6] = ' ';
	     Time->TimeString[7] = ' ';
       }
	Time->TimeString[8] = '\0';
}

//******************************
//ʱ��оƬ��ʼ��
//******************************
void Initial_DS1302(void)   
{   
	uchar Second=Read1302(DS1302_SECOND);
	if(Second&0x80)	      //�ж�ʱ��оƬ�Ƿ�ر�	  
    {
	Write1302(0x8e,0x00); //д������
	Write1302(0x8c,0x07); //����д���ʼ��ʱ�� ����:07/07/25.����: 3. ʱ��: 23:59:55
	Write1302(0x88,0x07);
	Write1302(0x86,0x25);
	Write1302(0x8a,0x07);
	Write1302(0x84,0x23);
	Write1302(0x82,0x59);
	Write1302(0x80,0x55);
	Write1302(0x8e,0x80); //��ֹд��
	}

}

//****************************************
//ds18b20�ӳ���ģ��
//****************************************


/***********ds18b20�ӳ���*************************/

/***********ds18b20�ӳ��Ӻ���������12MHz ��*******/ 

void delay_18B20(unsigned int i)
{
	while(i--);
}

/**********ds18b20��ʼ������**********************/

void Init_DS18B20(void) 
{
	 unsigned char x=0;
	 DQ = 1;          //DQ��λ
	 delay_18B20(8);  //������ʱ
	 DQ = 0;          //��Ƭ����DQ����
	 delay_18B20(40); //��ȷ��ʱ ���� 480us
	 DQ = 1;          //��������
	 delay_18B20(7);
	 x=DQ;            //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	 delay_18B20(10);
}

/***********ds18b20��һ���ֽ�**************/  

uchar ReadOneChar(void)
{
	uchar i=0;
	uchar dat = 0;
	for (i=8;i>0;i--)
	 {
		  DQ = 0; // �������ź�
		  dat>>=1;
		  DQ = 1; // �������ź�
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(4);
	 }
 	return(dat);
}

/*************ds18b20дһ���ֽ�****************/  

void WriteOneChar(uchar dat)
{
 	uchar i=0;
 	for (i=8; i>0; i--)
 	{
  		DQ = 0;
 		DQ = dat&0x01;
    	delay_18B20(5);
 		DQ = 1;
    	dat>>=1;
 }
}

/**************��ȡds18b20��ǰ�¶�************/

void ReadTemp(void)
{
	uchar a=0;
	uchar b=0;
	uchar t;

	Init_DS18B20();
	WriteOneChar(0xCC);    	// ����������кŵĲ���
	WriteOneChar(0x44); 	// �����¶�ת��

	delay_18B20(100);       // this message is wery important

	Init_DS18B20();
	WriteOneChar(0xCC); 	//����������кŵĲ���
	WriteOneChar(0xBE); 	//��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�

	delay_18B20(50);

	a=ReadOneChar();    	//��ȡ�¶�ֵ��λ
	b=ReadOneChar();   		//��ȡ�¶�ֵ��λ
	t=b&0xf8;
	if(t)
	{
	  TempBuffer[0]=':';
	  TempBuffer[1]='-';
  	  temp_value=b<<4;
	  temp_value+=(a&0xf0)>>4;  
	  temp_value=~temp_value+1;
	  temp1_value=~a&0x0f;
	}
	else
	{  
        temp_value=b<<4;
        temp_value+=(a&0xf0)>>4; 
		 temp1_value=a&0x0f;
	    TempBuffer[0]=':';
	    TempBuffer[1]=temp_value/100+'0';  //��λ
       if(TempBuffer[1]=='1')
       {
            TempBuffer[1]='1';
       }
       else
       {
           TempBuffer[1]=' ';
       } 
    }
}
void temp_to_str()   //�¶�����ת����Һ���ַ���ʾ
{		

  TempBuffer[2]=temp_value%100/10+'0';  //ʮλ
  TempBuffer[3]=temp_value%10+'0';  //��λ
  TempBuffer[4]='.';
  TempBuffer[5]=temp1_value*625/1000%10+'0';
  TempBuffer[6]=temp1_value*625/100%10+'0';
  TempBuffer[7]=temp1_value*625/10%10+'0';
  //TempBuffer[8]=temp1_value*625%10+'0';
  TempBuffer[8]='\0';
}
//**********************************************
//��ʱ�ӳ���ģ��
//**********************************************
void mdelay(uint delay)
{	uint i;
 	for(;delay>0;delay--)
   		{for(i=0;i<80;i++) //1ms��ʱ.
       		{;}
   		}
}

//************************************************
//�������ó���ģ��
//************************************************

//************************************
//��������ģʽ,����Ĭ����ʾ
//************************************
void outkey()                    
{ uchar Second;
  if(out==0)         
  { mdelay(5); 
	count=0;
	hide_sec=0,hide_min=0,hide_hour=0,hide_day=0,hide_week=0,hide_month=0,hide_year=0;
	Second=Read1302(DS1302_SECOND);
    Write1302(0x8e,0x00); //д������
	Write1302(0x80,Second&0x7f);
	Write1302(0x8E,0x80);          //��ֹд��
	done=0;           
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************
//���򰴼�
//*************************
void Upkey()
{	   
		Up=1;
	            if(Up==0)
		          {
				   mdelay(5);
				       switch(count)
				          {case 1:
                                  temp=Read1302(DS1302_SECOND);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //������1
                                  up_flag=1;    //���ݵ�������±�־
								  if((temp)>59)   //����59��,����
                                  temp=0;	
							  	  temp=temp/10*16+temp%10;
								  break;
				           case 2:
                                  temp=Read1302(DS1302_MINUTE);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //������1
                                  up_flag=1;
								  if(temp>59)          //����59��,����
								  temp=0;
								  temp=temp/10*16+temp%10;
								  break;
				           case 3:
                                  temp=Read1302(DS1302_HOUR);  //��ȡСʱ��
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //Сʱ����1
                                  up_flag=1;
								  if(temp>23)   //����23Сʱ,����
								  temp=0;
								  temp=temp/10*16+temp%10;
								  break;
				           case 4:
                                  temp=Read1302(DS1302_WEEK);  //��ȡ������
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //��������1
                                  up_flag=1;
								  if(temp>7)  
								  temp=1;
								  temp=temp/10*16+temp%10;
								  break;
				           case 5:
                                  temp=Read1302(DS1302_DAY);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //������1
                                  up_flag=1;
								  if(temp>31)
								  temp=1;
								  temp=temp/10*16+temp%10;
								  break;
				           case 6:
                                  temp=Read1302(DS1302_MONTH);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //������1
                                  up_flag=1;
								  if(temp>12)
								  temp=1;
								  temp=temp/10*16+temp%10;
								  break;
				           case 7:
                                  temp=Read1302(DS1302_YEAR);  //��ȡ����
								  temp=((temp&0xf0)>>4)*10 + (temp&0x0F);
								  temp=temp+1;  //������1
                                  up_flag=1;
								  if(temp>99)
								  temp=0;
								  temp=temp/10*16+temp%10;
								  break;
					       default:break;
				          }
					  
				 //  while(Up==0);
				  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//************************
//���򰴼�
//************************
void Downkey()
{	    
		Down=1;
	    if(Down==0)
		          {
				   mdelay(5);
				     switch(count)
				          {case 1:
                                  temp=Read1302(DS1302_SECOND);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;						    //������1
                                  down_flag=1;       //���ݵ�������±�־
								  if(temp==-1)     //С��0��,����59��
								  temp=59;
								  temp=temp/10*16+temp%10;
								  break;
				           case 2:
                                  temp=Read1302(DS1302_MINUTE);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //������1
                                  down_flag=1;
								  if(temp==-1)
								  temp=59;      //С��0��,����59��
								  temp=temp/10*16+temp%10;
								  break;
				           case 3:
                                  temp=Read1302(DS1302_HOUR);  //��ȡСʱ��
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //Сʱ����1
                                  down_flag=1;
								  if(temp==-1)
								  temp=23;
								  temp=temp/10*16+temp%10;
								  break;
				           case 4:
                                  temp=Read1302(DS1302_WEEK);  //��ȡ������;
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //��������1
                                  down_flag=1;
								  if(temp==0)
								  temp=7;
								  temp=temp/10*16+temp%10;
								  break;
				           case 5:
                                  temp=Read1302(DS1302_DAY);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //������1
                                  down_flag=1;
								  if(temp==0)
								  temp=31;
								  temp=temp/10*16+temp%10;
								  break;
				           case 6:
                                  temp=Read1302(DS1302_MONTH);  //��ȡ����
								  temp=((temp&0x70)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //������1
                                  down_flag=1;
								  if(temp==0)
								  temp=12;
								  temp=temp/10*16+temp%10;
								  break;
				           case 7:
                                  temp=Read1302(DS1302_YEAR);  //��ȡ����
								  temp=((temp&0xf0)>>4)*10 + (temp&0x0F);
								  temp=temp-1;  //������1
                                  down_flag=1;
								  if(temp==-1)
								  temp=99;
								  temp=temp/10*16+temp%10;
								  break;
					      default:break;
				         }
					 
				 //  while(Down==0);
				  }
}

//**************************
//ģʽѡ�񰴼�
//**************************
void Setkey()
{
		Set=1;
		if(Set==0)
	    {
           mdelay(5);
           count=count+1;	 //Setkey��һ��,count�ͼ�1
		   done=1;			 //�������ģʽ
           while(Set==0);
		 }

}

//*************************
//��������ִ��
//*************************
void keydone()
{        uchar Second;
		/* if(flag==0)    //�ر�ʱ��,ֹͣ��ʱ
         { Write1302(0x8e,0x00); //д������
           temp=Read1302(0x80);
           Write1302(0x80,temp|0x80);
	       Write1302(0x8e,0x80); //��ֹд��
           flag=1;
         }*/
         Setkey();				            //ɨ��ģʽ�л�����
		 switch(count)
		 {case 1:do					        //count=1,������
		          {
                   
                   outkey();			   //ɨ��������ť
				   Upkey();                //ɨ��Ӱ�ť
				   Downkey();              //ɨ�����ť
				   if(up_flag==1||down_flag==1)  //���ݸ��£�����д���µ�����
				   {
				     Write1302(0x8e,0x00); //д������
				     Write1302(0x80,temp); //д���µ�����
				     Write1302(0x8e,0x80); //��ֹд��
				     up_flag=0;
			    	 down_flag=0;
				  }
                     if(Down!=0&&Up!=0)
                     {
						hide_sec++;
				        if(hide_sec>3)
				        hide_sec=0;
                     }
					 else hide_sec=0;
					 show_time();         //Һ����ʾ����
				  }while(count==2);break;  
		  case 2:do					        //count=2,������
		          {
				   hide_sec=0;
				   outkey();
				   Upkey();
				   Downkey();
				   if(temp>0x60)
				     temp=0;
				   if(up_flag==1||down_flag==1)
				   {
				     Write1302(0x8e,0x00); //д������
				     Write1302(0x82,temp); //д���µķ���
				     Write1302(0x8e,0x80); //��ֹд��
				     up_flag=0;
				     down_flag=0;
				  }
                     if(Down!=0&&Up!=0)
                     {
						hide_min++;
				        if(hide_min>3)
				        hide_min=0;
                     }
					 else hide_min=0;
					 show_time();
				  }while(count==3);break;
		  case 3:do					        //count=3,����Сʱ
		          {
                   hide_min=0; 
				   outkey();
				   Upkey();
				   Downkey();
				   if(up_flag==1||down_flag==1)
				   {
				      Write1302(0x8e,0x00); //д������
				      Write1302(0x84,temp); //д���µ�Сʱ��
				      Write1302(0x8e,0x80); //��ֹд��
				      up_flag=0;
				      down_flag=0;
				   }
                      if(Down!=0&&Up!=0)
                     {
						hide_hour++;
				        if(hide_hour>3)
				        hide_hour=0;
                     }
					 else  hide_hour=0;
					  show_time();
				  }while(count==4);break;
		  case 4:do					        //count=4,��������
		          {
                   hide_hour=0; 
				   outkey();
				   Upkey();
				   Downkey();
				   if(up_flag==1||down_flag==1)
				   {
				     Write1302(0x8e,0x00); //д������
				     Write1302(0x8a,temp); //д���µ�������
				     Write1302(0x8e,0x80); //��ֹд��
				     up_flag=0;
				     down_flag=0;
				   }
                     if(Down!=0&&Up!=0)
                     {
					   hide_week++;
				       if(hide_week>3)
				       hide_week=0;
                     }
					 else hide_week=0;
					 show_time();
				  }while(count==5);break;
		  case 5:do					        //count=5,������
		          {
				   hide_week=0; 
				   outkey();
				   Upkey();
				   Downkey();
				   if(up_flag==1||down_flag==1)
				   {
				     Write1302(0x8e,0x00); //д������
				     Write1302(0x86,temp); //д���µ�����
				     Write1302(0x8e,0x80); //��ֹд��
				     up_flag=0;
				     down_flag=0;
				   }
                     if(Down!=0&&Up!=0)
				     {
						hide_day++;
				        if(hide_day>3)
				        hide_day=0;
					 }
				    else hide_day=0;
                     show_time();
				  }while(count==6);break;
		  case 6:do					        //count=6,������
		          {
                   hide_day=0; 
				   outkey();
				   Upkey();
				   Downkey();
				   if(up_flag==1||down_flag==1)
				   {
				      Write1302(0x8e,0x00); //д������
				      Write1302(0x88,temp); //д���µ�����
				      Write1302(0x8e,0x80); //��ֹд��
				      up_flag=0;
				      down_flag=0;
				   }
                     if(Down!=0&&Up!=0)
				     {
						 hide_month++;
				         if(hide_month>3)
				         hide_month=0;
					 }
					else hide_month=0;
                     show_time();
				  }while(count==7);break;
		  case 7:do					        //count=7,������
		          {
                   hide_month=0; 
				   outkey();
				   Upkey();
				   Downkey();
				   if(up_flag==1||down_flag==1)
				   {
				      Write1302(0x8e,0x00); //д������
				      Write1302(0x8c,temp); //д���µ�����
				      Write1302(0x8e,0x80); //��ֹд��
				      up_flag=0;
				      down_flag=0;
				  }
                      if(Down!=0&&Up!=0)
				      {
					 	 hide_year++;
				         if(hide_year>3)
				         hide_year=0;
                      }
					  else hide_year=0;
                      show_time();
				  }while(count==8);break;
		  case 8: count=0;hide_year=0;  //count8, ��������ģʽ,����Ĭ����ʾ״̬
	              Second=Read1302(DS1302_SECOND);
                  Write1302(0x8e,0x00); //д������
	              Write1302(0x80,Second&0x7f);
	              Write1302(0x8E,0x80);          //��ֹд��
				  done=0;
		  break; //count=7,�����ж�,��־λ��0���˳�
		  default:break;

		 }

}


//***************************************
//Һ����ʾ������ģ��
//***************************************
void show_time()   
{
  DS1302_GetTime(&CurrentTime);  //��ȡʱ��оƬ��ʱ������
  TimeToStr(&CurrentTime);       //ʱ������ת��Һ���ַ�
  DateToStr(&CurrentTime);       //��������ת��Һ���ַ�
  ReadTemp();                    //�����¶Ȳɼ�����
  temp_to_str();                 //�¶�����ת����Һ���ַ�
  gotoxy(4,0);
  print("�¶�");
  gotoxy(4,2);                  //Һ���ַ���ʾλ��
  print(TempBuffer);             //��ʾ�¶�
  gotoxy(4,6);
  print("��");
  gotoxy(3,0);
  print("ʱ��:");
  gotoxy(3,3);
  print(CurrentTime.TimeString); //��ʾʱ��
  gotoxy(2,3);
  print(CurrentTime.DateString); //��ʾ����
  gotoxy(2,0);
  print("����");
  gotoxy(2,2);
  print(week_value);             //��ʾ����
  gotoxy(1,1);
  print("����־���ӡ�");
  mdelay(500);                 //ɨ����ʱ
}


//************************************
//������
//*************************************
main()
{
  //  flag=1;           //ʱ��ֹͣ��־
	init_lcd();
	clrram_lcd();
	Init_DS18B20( ) ;      //DS18B20��ʼ��
	Initial_DS1302(); //ʱ��оƬ��ʼ��
	up_flag=0;
	down_flag=0;
	done=0;           //����Ĭ��Һ����ʾ
	while(1)
	{   
        while(done==1)
          keydone();    //�������ģʽ
		while(done==0)
 	    {  
            show_time();                //Һ����ʾ����
           // flag=0;                  
  		    Setkey();				 //ɨ������ܼ�
 		}		
	}
}




