#include <reg52.h>
#include <stdio.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
//写的时候，是低电平改变数据，上升沿送入数据
//读的时候，是低电平读入数据，下降沿使得从设备把数据放到总线上 
//读的时候，是低电平读，但没有读出，提供下降沿使得从设备把数据放到总线上

sbit P11=P1^1;	//定时和时钟切换
sbit P12=P1^2;	//时调整
sbit P13=P1^3;	//分调整
sbit P14=P1^4;	//秒调整

sbit CLK =P3^5;
sbit IO  =P3^6;
sbit RST =P3^7;

uchar m,m10,f,f10,s,s10;

unsigned char data discode[10]={0XFC,0X60,0XDA,0XF2,0X66,0XB6,0XBE,0XE0,0XFE,0XF6};//共阴串行数组代表0~9

unsigned char leddis[6]={1,2,3,4,5,6};//显存,有几个数码管可以定义几个

/***************************延时函数*************************************/
 del()  /*延时0.2秒*/  
{
unsigned int i,j,k;
for(i=100;i>0;i--)
for(j=20;j>0;j--)
for(k=248;k>0;k--);
}


/****************************************************************/
/////////////////////DS1302////////////////////////
//--------------WriteOneByteData-----------------/
void WriteByte(uchar ucda)
	{
	uchar i; 	  
	for(i=8;i>0;i--)
		{
		IO=(bit)(ucda&0x01);
		CLK=1;		   
		CLK=0;
		ucda>>=1;
		}
	}

//--------------ReadOneByteData-----------------/
uchar ReadByte(void)
    {
	uchar i,a;
	for(i=8;i>0;i--)
		{ 		   		   
		a>>=1;
		if(IO)a|=0x80;
		CLK=1;           
		CLK=0;
		}	
	return(a);
	}
//--------------WriteAddrFist,ThenWriteData-----------------/

void write1302(uchar Addr,uchar Data)
	{
	RST=0;
	CLK=0;
	RST=1;
	WriteByte(Addr);
	WriteByte(Data);
	CLK=1;
	RST=0;
	}

//--------------WriteAddrFirst,ThenReadData-----------------/

uchar read1302(uchar Addr)
	{
	uchar Data;
	RST=0;
	CLK=0;	
	RST=1;
	WriteByte(Addr);
	Data=ReadByte();
	CLK=1;
	RST=0;
	return(Data);
	}


/************************************************************************/
////////////////ds1302初始化//////////////////调整时间时用
void csh1302(void)
	{  	
	CLK=0;
	RST=0; 
	write1302(0x8e,0x00);//打开写保护
	write1302(0x90,0x06);//禁止充电
/////////////////////////////////////////////////
	write1302(0x80,0x20);//预置秒
	write1302(0x82,0x30);//预置分
	write1302(0x84,0x08);//预置时
	write1302(0x86,0x22);//预置日期
	write1302(0x88,0x07);//预置月
	//write1302(0x8a,0x02);//预置星期
	write1302(0x8c,0x08);//预置年	  
////////////////////////////////////////////////
	write1302(0x80,0x00);//启动时钟
	write1302(0x8e,0x80);//关闭写保护
	}
/************************************************************************/
//////////////////显示函数////////////////////////////
void xianshi(void)
{
	s10=read1302(0x85);
	s=read1302(0x85)&0x0f;		//时
	s10=s10>>4;
	s10=s10&0x07;
  ////////////////////////////////////
    f10=read1302(0x83);			//分
	f=read1302(0x83)&0x0f;
	f10=f10>>4;
	f10=f10&0x07;
  ////////////////////////////////////
    m10=read1302(0x81);			 //秒
	m=read1302(0x81)&0x0f;
	m10=m10>>4;
	m10=m10&0x07;
 }

/**************************************************************************/
void display(void)  //数码管显示函数
{
    unsigned char count;//数码管个数
        
    for (count=6;count>0;count--)
    {
        SBUF=discode[leddis[count-1]];
        while(TI==0);
        TI=0;
    }
}    


void show(void)	 //显示时间函数
{
        xianshi();
        leddis[0]=s10;
		leddis[1]=s;
   		leddis[2]=f10;
   		leddis[3]=f;
		leddis[4]=m10;
   		leddis[5]=m;
	
		display();	//	显示函数
}
/************************************************************************
						   主函数
************************************************************************/

void main()

{
SCON=0x00;	//设定UART的工作方式为MODEO
	   
csh1302(); //初始化

while(1)
{
show();
del();
 }

}


