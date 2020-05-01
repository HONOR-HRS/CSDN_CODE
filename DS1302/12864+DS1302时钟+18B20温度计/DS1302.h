/**************************************************************************
  
                   		THE REAL TIMER DS1302 DRIVER LIB
  
             		COPYRIGHT (c)   2005 BY JJJ.
                       		--  ALL RIGHTS RESERVED  --
  
   File Name:       DS1302.h
   Author:          Jiang Jian Jun
   Created:         2003/7/21
   Modified:		NO
   Revision: 		1.0
  
***************************************************************************/
#ifndef _REAL_TIMER_DS1302_2003_7_21_
#define _REAL_TIMER_DS1302_2003_7_21_

sbit  DS1302_CLK = P2^4;              //ʵʱʱ��ʱ�������� 
sbit  DS1302_IO  = P2^5;              //ʵʱʱ������������ 
sbit  DS1302_RST = P2^6;              //ʵʱʱ�Ӹ�λ������
sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;
char hide_sec,hide_min,hide_hour,hide_day,hide_week,hide_month,hide_year;

typedef struct __SYSTEMTIME__
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	unsigned char Week;
	unsigned char Day;
	unsigned char Month;
	unsigned char  Year;
	unsigned char DateString[11];
	unsigned char TimeString[9];
}SYSTEMTIME;	//�����ʱ������

#define AM(X)	X
#define PM(X)	(X+12)            	  // ת��24Сʱ��
#define DS1302_SECOND	0x80
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84 
#define DS1302_WEEK		0x8A
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_YEAR		0x8C
#define DS1302_RAM(X)	(0xC0+(X)*2)   	//���ڼ��� DS1302_RAM ��ַ�ĺ� 

void DS1302InputByte(unsigned char d) 	//ʵʱʱ��д��һ�ֽ�(�ڲ�����)
{ 
    unsigned char i;
    ACC = d;
    for(i=8; i>0; i--)
    {
        DS1302_IO = ACC0;           	//�൱�ڻ���е� RRC
        DS1302_CLK = 1;
        DS1302_CLK = 0;
        ACC = ACC >> 1; 
    } 
}

unsigned char DS1302OutputByte(void) 	//ʵʱʱ�Ӷ�ȡһ�ֽ�(�ڲ�����)
{ 
    unsigned char i;
    for(i=8; i>0; i--)
    {
        ACC = ACC >>1;         			//�൱�ڻ���е� RRC 
        ACC7 = DS1302_IO;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
    } 
    return(ACC); 
}

void Write1302(unsigned char ucAddr, unsigned char ucDa)	//ucAddr: DS1302��ַ, ucData: Ҫд������
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr);       	// ��ַ������ 
    DS1302InputByte(ucDa);       	// д1Byte����
    DS1302_CLK = 1;
    DS1302_RST = 0;
} 

unsigned char Read1302(unsigned char ucAddr)	//��ȡDS1302ĳ��ַ������
{
    unsigned char ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr|0x01);        // ��ַ������ 
    ucData = DS1302OutputByte();         // ��1Byte����
    DS1302_CLK = 1;
    DS1302_RST = 0;
    return(ucData);
}

void DS1302_SetProtect(bit flag)        //�Ƿ�д����
{
	if(flag)
		Write1302(0x8E,0x10);
	else
		Write1302(0x8E,0x00);
}

void DS1302_SetTime(unsigned char Address, unsigned char Value)        // ����ʱ�亯��
{
	DS1302_SetProtect(0);
	Write1302(Address, ((Value/10)<<4 | (Value%10))); 
}

void DS1302_GetTime(SYSTEMTIME *Time)
{
	unsigned char ReadValue;
	ReadValue = Read1302(DS1302_SECOND);
	Time->Second = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MINUTE);
	Time->Minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_HOUR);
	Time->Hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_DAY);
	Time->Day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	
	ReadValue = Read1302(DS1302_WEEK);
	Time->Week = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MONTH);
	Time->Month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_YEAR);
	Time->Year = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	
}

void DateToStr(SYSTEMTIME *Time)
{   if(hide_year<2)
    {
	  Time->DateString[0] = Time->Year/10 + '0';
	  Time->DateString[1] = Time->Year%10 + '0';
	}
	  else
	    {
	      Time->DateString[0] = ' ';
	      Time->DateString[1] = ' ';
		}
	Time->DateString[2] = '-';
	if(hide_month<2)
	{
	  Time->DateString[3] = Time->Month/10 + '0';
	  Time->DateString[4] = Time->Month%10 + '0';
	}
	  else
	  {
	    Time->DateString[3] = ' ';
	    Time->DateString[4] = ' ';
	  }
	Time->DateString[5] = '-';
	if(hide_day<2)
	{
	  Time->DateString[6] = Time->Day/10 + '0';
	  Time->DateString[7] = Time->Day%10 + '0';
	}
	  else
	  {
	    Time->DateString[6] = ' ';
	    Time->DateString[7] = ' ';	    
	  }
	if(hide_week<2)
	{
	  Time->DateString[8] = ' ';
	  Time->DateString[9] = Time->Week%10 + '0';
	}
	  else
	  {
	    Time->DateString[8] = ' ';
	    Time->DateString[9] = ' ';
	  }

	Time->DateString[10] = '\0';
}

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
	Time->DateString[8] = '\0';
}

void Initial_DS1302(void)
{   
	unsigned char Second=Read1302(DS1302_SECOND);
	Write1302(0x8e,0x00); //д������
	Write1302(0x80,0x50);
	Write1302(0x82,0x56);
	Write1302(0x84,0x22);
	Write1302(0x8e,0x80); //��ֹд��
	if(Second&0x80)		  
		DS1302_SetTime(DS1302_SECOND,0x80);
}

#endif