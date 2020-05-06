
//-------------------------------------------------------------------------------------//
//RCWL-0530模块与51接口连接：
// 1：VCC   --〉   1.8V-5.5V电源 
// 2：SCL   --〉   P3.5
// 3：SDA   --〉   P3.7
// 4：INT   --〉   NC
// 5：IRD   --〉   NC
// 6：RD    --〉   NC
// 7：GND   --〉   地
//-------------------------------------------------------------------------------------//
# include <reg52.h>
# include <stdio.h>
# include <intrins.h>
//定义IIC接口
sbit IIC_SCL    =P3^5;     //IIC的SCL
sbit IIC_SDA    =P3^7;     //IIC的SDA
bit  IIC_ACK;              //IIC的ACK
int  rda;                  //IIC读出
//-------------------------------------------------------------------------------------//
//函数:    delayms()
//功能:    延时程序
//-------------------------------------------------------------------------------------//
void delayms(unsigned int ms)
{
	unsigned char i=100,j;
	for(;ms;ms--)
	{
		while(--i)
		{
			j=10;
			while(--j);
		}
	}
}
//-------------------------------------------------------------------------------------//
//函数:    void iic_start();
//功能:    I2C总线开始
//-------------------------------------------------------------------------------------//
//   SCL   --- --- ___
//   SDA   --- ___ ___
void iic_start()
{  
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;         
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//函数:    void iic_stop();
//功能:    I2C总线结束
//需定义：
//-------------------------------------------------------------------------------------//
//   SCL   ___ --- ---
//   SDA   ___ ___ ---
void iic_stop()
{  
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//函数:    void iic_sendbyte(unsigned char c);
//功能:    发送 8_BIT 数据
//-------------------------------------------------------------------------------------//
void iic_sendbyte(unsigned char c)
{
unsigned char bitcnt;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
if((c<<bitcnt)&0x80)
 IIC_SDA=1;
else
 IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
}
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
if(IIC_SDA==0)
 IIC_ACK=0;
else
 IIC_ACK=1;
IIC_SCL=0;
_nop_();
_nop_();
}
//-------------------------------------------------------------------------------------//
//函数:    int iic_rcvbyte_nack();
//功能:    接收 8_BIT 数据  最后ack信号
//-------------------------------------------------------------------------------------//
int iic_rcvbyte_nack()
{
unsigned char retc;
unsigned char bitcnt;
retc=0;
IIC_SDA=1;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
retc=retc<<1;
if(IIC_SDA==1)
retc=retc+1;
_nop_();
_nop_();
}
//给出NACK信号
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=1;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
return(retc);
}
//-------------------------------------------------------------------------------------//
//函数:    int iic_rcvbyte_ack();
//功能:    接收 8_BIT 数据 最后ack信号
//-------------------------------------------------------------------------------------//
int iic_rcvbyte_ack()
{
unsigned char retc;
unsigned char bitcnt;
retc=0;
IIC_SDA=1;
for(bitcnt=0;bitcnt<8;bitcnt++)
{
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
retc=retc<<1;
if(IIC_SDA==1)
retc=retc+1;
_nop_();
_nop_();
}
//给出ACK信号
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
IIC_SDA=0;
_nop_();
_nop_();
IIC_SCL=1;
_nop_();
_nop_();
IIC_SCL=0;
_nop_();
_nop_();
return(retc);
}
//-------------------------------------------------------------------------------------//
//函数:      wr_max30100_one_data()
//功能:      写一位max30100数据
//address:   芯片从地址
//saddress:  写寄存器地址
//w_data:    待写数据
//-------------------------------------------------------------------------------------//
void wr_max30100_one_data(int address,int saddress,int w_data )
{
_nop_();
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
iic_sendbyte(saddress);
_nop_();
iic_sendbyte(w_data);
_nop_();
iic_stop();
_nop_();
}
//-------------------------------------------------------------------------------------//
//函数:      rd_max30100_one_data()
//功能:      读一位max30100数据
//address:   芯片从地址
//saddress:  读寄存器地址
//rda:       读出的数据
//-------------------------------------------------------------------------------------//
void rd_max30100_one_data(int address,int saddress)
{
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
iic_sendbyte(saddress);
_nop_();
address=address+1;
_nop_();
iic_start();
_nop_();
iic_sendbyte(address);
_nop_();
rda=iic_rcvbyte_nack();
_nop_();
iic_stop();
}
//-------------------------------------------------------------------------------------//
//函数:      主程序
//功能:      读max30100内部温度
//-------------------------------------------------------------------------------------//
main()
{
double temp,temp1,temp2;
//temp       测量温度
//temp1      30100整数部分温度
//temp2      30100小数部分温度
TMOD=0x21;		  
SCON=0x50;
TH1=0xFD;
TL1=0xFD;
TR1=1;			   
TI=1;   
//设置51的波特率为 9600 N 8 1
//51主频为11.0592MHz STC的MCU注意要选择外部晶体振荡
while(1)
{
//复位芯片，设置模式
wr_max30100_one_data(0xae,0x06,0x0b);  
//读芯片ID
rd_max30100_one_data(0xae,0xff);            // 读出芯片ID
printf("MAX30100 ID =%d\n",rda);            // 串口显示
//点亮芯片LED
wr_max30100_one_data(0xae,0x07,0x43);       // 设置电流，点亮LED
delayms(10);   
//温度功能测试
wr_max30100_one_data(0xae,0x09,0x66);       // 0X06地址B3位TEMP_EN置1
delayms(50);                                // 等待温度转换完成，不等待，读出数据有误
rd_max30100_one_data(0xae,0x16);            // 读出温度信号
printf("temp1=%d\n",rda);                   // 串口显示
temp1=rda;
rd_max30100_one_data(0xae,0x17);            // 读出温度小数部分数据
printf("temp2=%d\n",rda);                   // 串口显示
temp2=rda;
temp=temp1+(temp2*0.0625);                  // 计算温度，小数部分最小温度值0.0625
printf("当前温度=%.4f\n",temp);             // 串口显示当前温度
delayms(100);  
printf("\n");                               // 串口显示当前温度
}
}
