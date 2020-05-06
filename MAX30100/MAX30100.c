
//-------------------------------------------------------------------------------------//
//RCWL-0530ģ����51�ӿ����ӣ�
// 1��VCC   --��   1.8V-5.5V��Դ 
// 2��SCL   --��   P3.5
// 3��SDA   --��   P3.7
// 4��INT   --��   NC
// 5��IRD   --��   NC
// 6��RD    --��   NC
// 7��GND   --��   ��
//-------------------------------------------------------------------------------------//
# include <reg52.h>
# include <stdio.h>
# include <intrins.h>
//����IIC�ӿ�
sbit IIC_SCL    =P3^5;     //IIC��SCL
sbit IIC_SDA    =P3^7;     //IIC��SDA
bit  IIC_ACK;              //IIC��ACK
int  rda;                  //IIC����
//-------------------------------------------------------------------------------------//
//����:    delayms()
//����:    ��ʱ����
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
//����:    void iic_start();
//����:    I2C���߿�ʼ
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
//����:    void iic_stop();
//����:    I2C���߽���
//�趨�壺
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
//����:    void iic_sendbyte(unsigned char c);
//����:    ���� 8_BIT ����
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
//����:    int iic_rcvbyte_nack();
//����:    ���� 8_BIT ����  ���ack�ź�
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
//����NACK�ź�
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
//����:    int iic_rcvbyte_ack();
//����:    ���� 8_BIT ���� ���ack�ź�
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
//����ACK�ź�
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
//����:      wr_max30100_one_data()
//����:      дһλmax30100����
//address:   оƬ�ӵ�ַ
//saddress:  д�Ĵ�����ַ
//w_data:    ��д����
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
//����:      rd_max30100_one_data()
//����:      ��һλmax30100����
//address:   оƬ�ӵ�ַ
//saddress:  ���Ĵ�����ַ
//rda:       ����������
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
//����:      ������
//����:      ��max30100�ڲ��¶�
//-------------------------------------------------------------------------------------//
main()
{
double temp,temp1,temp2;
//temp       �����¶�
//temp1      30100���������¶�
//temp2      30100С�������¶�
TMOD=0x21;		  
SCON=0x50;
TH1=0xFD;
TL1=0xFD;
TR1=1;			   
TI=1;   
//����51�Ĳ�����Ϊ 9600 N 8 1
//51��ƵΪ11.0592MHz STC��MCUע��Ҫѡ���ⲿ������
while(1)
{
//��λоƬ������ģʽ
wr_max30100_one_data(0xae,0x06,0x0b);  
//��оƬID
rd_max30100_one_data(0xae,0xff);            // ����оƬID
printf("MAX30100 ID =%d\n",rda);            // ������ʾ
//����оƬLED
wr_max30100_one_data(0xae,0x07,0x43);       // ���õ���������LED
delayms(10);   
//�¶ȹ��ܲ���
wr_max30100_one_data(0xae,0x09,0x66);       // 0X06��ַB3λTEMP_EN��1
delayms(50);                                // �ȴ��¶�ת����ɣ����ȴ���������������
rd_max30100_one_data(0xae,0x16);            // �����¶��ź�
printf("temp1=%d\n",rda);                   // ������ʾ
temp1=rda;
rd_max30100_one_data(0xae,0x17);            // �����¶�С����������
printf("temp2=%d\n",rda);                   // ������ʾ
temp2=rda;
temp=temp1+(temp2*0.0625);                  // �����¶ȣ�С��������С�¶�ֵ0.0625
printf("��ǰ�¶�=%.4f\n",temp);             // ������ʾ��ǰ�¶�
delayms(100);  
printf("\n");                               // ������ʾ��ǰ�¶�
}
}
