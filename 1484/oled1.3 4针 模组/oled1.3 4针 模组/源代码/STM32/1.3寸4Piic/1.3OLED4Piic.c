
/********************金逸晨**************************本公司模块SDA,SCK已内置上拉电阻，
*****************1.3  4PIN IIC OLED FOR STM32F103*************
*****STM32F103驱动**************************
***** PB5----3  SCK   ,  PB6----4  SDA   ,    
***** BY:GU 

*********1.3寸旋转90度显示，结合取模方式，及翻转命令完成旋转显示
********************************************************/


#include "stm32f10x.h"
/**********SPI引脚分配，连接oled屏，更具实际情况修改*********/

#define IIC_SCK_PIN 5
#define IIC_SDA_PIN 6

#define OLED_COLUMN_NUMBER 128
#define OLED_LINE_NUMBER 64
#define OLED_COLUMN_OFFSET 2
#define OLED_PAGE_NUMBER OLED_LINE_NUMBER/8
/**********SPI引脚分配，连接oled屏，更具实际情况修改*********/

#define IIC_SCK_0  GPIOB->BRR=0X0020       // 设置sck接口到PB5   清零
#define IIC_SCK_1  GPIOB->BSRR=0X0020       //置位
#define IIC_SDA_0  GPIOB->BRR=0X0040       // 设置SDA接口到PB6
#define IIC_SDA_1  GPIOB->BSRR=0X0040

const unsigned char  *point;
unsigned char ACK=0;
const unsigned char  OLED_init_cmd[25]=			//SH1106
{
  
  /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
  0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
  0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
       0xAE,//关闭显示
	
       0xD5,//设置时钟分频因子,震荡频率
       0x80,  //[3:0],分频因子;[7:4],震荡频率

       0xA8,//设置驱动路数
       0X3F,//默认(1/64)
	
       0xD3,//设置显示偏移
       0X00,//默认为0
	
       0x40,//设置显示开始行 [5:0],行数.
	
       0x8D,//电荷泵设置
       0x14,//bit2，开启/关闭
       0x20,//设置内存地址模式
       0x02,//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	   
/************显示翻转命令**********************************************/
       0xA1,//段重定义设置,bit0:0,0->0;1,0->127;  A1
	
       0xC0,//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 (C0 翻转显示) C8
/********************************************************************************************************/
       0xDA,//设置COM硬件引脚配置
       0x12,//[5:4]配置  
	   
       0x81,//对比度设置
       0x66,//1~255;默认0X7F (亮度设置,越大越亮)
	   
       0xD9,//设置预充电周期
       0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;
	   
       0xDB,//设置VCOMH 电压倍率
       0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	   
       0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	   
       0xA6,//设置显示方式;bit0:1,反相显示;0,正常显示 
       
       0xAF,//开启显示     
};
const unsigned char picture_rev_90[]=   //旋转90度显示，行列式，低位在前，阴码
{  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,
    0x60,0x70,0x30,0x30,0x38,0x18,0x18,0x18,0x18,0xD8,0xF8,0xF0,0xF0,0xF0,0xE0,0xE0,
    0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x80,0xC0,0xF0,0xB8,0x10,0x00,0xE0,0xC0,0x80,0x80,0x80,0x00,0xF0,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xFC,0xFC,0xFE,0xFB,0x01,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x03,0x03,0x03,0x03,0x83,
    0xFF,0xFF,0xFF,0x7F,0x1C,0x38,0xF0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x1C,
    0x3E,0xE7,0xC3,0x81,0xFF,0xFF,0x18,0x18,0xFF,0xFF,0x18,0x99,0x9B,0xDB,0xFF,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xE0,0xFC,0x3F,0x07,0x00,0xFF,0xFF,0xFF,0x7F,0x0F,0x0F,0x0F,
    0x0F,0x0F,0xEF,0xEF,0xEF,0xCF,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
    0x07,0x07,0x01,0x00,0x00,0x00,0x00,0x07,0x3F,0xFC,0xE0,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xF6,0x3C,0xF0,0xE3,0x36,0x04,
    0xF0,0xE0,0xCC,0x44,0x66,0x33,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x01,0x07,0x1F,0x19,0x00,0x00,0x07,0x07,0x03,0x01,0x01,0x00,0x0F,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0xE0,0xFE,0x42,
    0x62,0xFE,0x62,0x42,0xFE,0xFE,0x42,0x40,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0xFC,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3F,0x3F,0x3F,0x1F,0x1F,0x1E,0x3E,0x3C,0x7C,0x78,0xF8,0xF0,0xF0,0xE0,
    0xE0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xF8,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x30,0x34,0x0C,0x1B,0x03,
    0x3F,0x3F,0x07,0x0F,0x1B,0x33,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86,
    0xC6,0xEC,0xEC,0xC0,0xC0,0xCF,0xCF,0x0C,0x0C,0x0C,0x8C,0xEC,0xCC,0x7E,0xE3,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0C,
    0x0C,0x0F,0x0C,0x0C,0x0F,0x0F,0x00,0x30,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1F,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
    0xC0,0xC0,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xFC,0xFC,0xFE,0xFE,0xFF,0xEF,0xEF,0xE7,
    0xC7,0xC3,0x83,0x03,0x03,0x03,0x03,0x03,0x1F,0xFF,0xFF,0x3F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x88,0x88,0x88,0xBE,0x88,0x88,
    0x88,0x88,0xF8,0xDE,0x42,0x60,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3F,
    0x3F,0x18,0xFF,0xFF,0x8C,0x8C,0xFF,0x7E,0x7E,0xDF,0x1B,0xF1,0xF0,0x00,0xFF,0xF8,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x80,
    0x80,0xFE,0xFE,0x80,0x80,0x80,0x80,0x80,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x03,0x1F,0x7C,0xF0,0x80,0x00,0x00,0xE0,0xFC,0xFF,0xFF,0x1F,
    0x07,0x03,0x07,0x07,0x07,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x03,
    0x07,0x07,0x1F,0xFF,0xFE,0xFC,0xE0,0xF0,0x7C,0x1F,0x03,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x01,0x01,0x01,0x81,0x81,0x80,0x80,0x80,0x83,0x83,0xC1,0xC2,0xE3,0x43,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x1C,0x07,0x03,
    0x01,0x7F,0x7F,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0E,0x1C,0x78,0xFF,0xFF,0xFF,0xFE,
    0x7C,0x78,0xF0,0xF0,0xF0,0xF0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,
    0xF0,0xF8,0xFC,0x7F,0x3F,0x0F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x03,
    0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x01,0xFD,0x01,0xFF,0xFF,0xCD,0xCC,0x7C,0x3C,0x0C,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,
    0x03,0x07,0x06,0x06,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x06,0x06,0x07,0x03,0x03,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x18,
    0x1F,0x1F,0x1F,0x1F,0x38,0x3F,0x00,0x1F,0x00,0x7F,0x3F,0x0D,0x07,0x0F,0x7C,0x30,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char  picture_tab[]={
/*------------------------------------------------------------------------------
;  源文件 / 文字 : C:\Documents and Settings\Administrator\桌面\新建文件夹 (2)\logo.bmp字模
;  宽×高（像素）: 128×64
------------------------------------------------------------------------------*/
//0x80,0x40,0x10,//宽的像素数,高的像素数，宽的字节数，参数设置可选
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x20,0x44,0x08,0x00,0x18,0x48,0x28,0xC8,0x08,0x28,0x48,0x18,0x00,
0x40,0x40,0xFC,0x40,0x40,0xFC,0x00,0x00,0xF8,0x00,0x00,0xFC,0x00,0x40,0x40,0xA0,
0x90,0x88,0x84,0x88,0x90,0x20,0x40,0x40,0x00,0x00,0x40,0x44,0xD8,0x20,0xF0,0xAC,
0xA8,0xE8,0xB8,0xA8,0xE0,0x00,0x00,0x00,0xC0,0x7C,0x54,0x54,0x54,0x54,0x54,0x54,
0x7C,0x40,0x40,0x00,0x00,0xF0,0x90,0x90,0x90,0xFC,0x90,0x90,0x90,0xF0,0x00,0x00,
0x00,0x80,0x88,0x88,0x88,0x88,0x88,0xE8,0xA8,0x98,0x8C,0x88,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x04,0x3E,0x01,0x10,0x11,0x09,0x05,0x3F,0x05,0x09,0x11,0x11,0x00,
0x08,0x18,0x0F,0x24,0x14,0x0F,0x00,0x00,0x0F,0x00,0x00,0x3F,0x00,0x20,0x22,0x2A,
0x32,0x22,0x3F,0x22,0x32,0x2A,0x22,0x20,0x00,0x00,0x20,0x10,0x0F,0x10,0x28,0x24,
0x23,0x20,0x2F,0x28,0x2A,0x2C,0x00,0x30,0x0F,0x04,0x3D,0x25,0x15,0x15,0x0D,0x15,
0x2D,0x24,0x24,0x00,0x00,0x07,0x04,0x04,0x04,0x1F,0x24,0x24,0x24,0x27,0x20,0x38,
0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x81,0x41,0x21,0x21,0x61,0x01,0x01,0x21,0xE1,0xE1,0x01,0xE1,0xE1,
0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x21,0xE1,0x21,0x21,0x21,0x61,0x01,0x01,
0x21,0x21,0xE1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0x10,0x20,0x24,0x1C,0x04,0x00,0x20,0x3F,0x01,0x3E,0x01,0x3F,
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x22,0x22,0x27,0x30,0x00,0x00,
0x20,0x20,0x3F,0x20,0x20,0x00,0x00,0x1E,0x25,0x25,0x25,0x16,0x00,0x00,0x1E,0x21,
0x21,0x21,0x13,0x00,0x01,0x01,0x1F,0x21,0x21,0x00,0x00,0x00,0x21,0x3F,0x22,0x21,
0x01,0x00,0x00,0x1E,0x21,0x21,0x21,0x1E,0x00,0x21,0x3F,0x22,0x01,0x01,0x3E,0x20,
0x00,0x21,0x21,0x3F,0x20,0x20,0x00,0x00,0x1E,0x21,0x21,0x21,0x13,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xF0,0x08,0x04,0x04,0x04,0x0C,0x00,0xF0,0x08,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xFC,0x04,0x00,
0x00,0x00,0x00,0x00,0x0C,0x04,0xFC,0x04,0x0C,0x00,0x04,0xFC,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x81,0x82,0x84,0x84,0x84,0x82,0x80,0x81,0x82,0x84,0x84,0x82,0x81,
0x80,0x80,0x86,0x86,0x80,0x80,0x80,0x80,0x80,0x85,0x83,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x84,0x87,0x84,0x84,
0x84,0x86,0x80,0x80,0x80,0x84,0x87,0x84,0x80,0x80,0x84,0x87,0x84,0x84,0x82,0x81,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,
};


/*******时钟初始化******************/
void SYS_init(unsigned char PLL)
{
	
		 
	RCC->APB1RSTR = 0x00000000;//复位结束			 
	RCC->APB2RSTR = 0x00000000; 	  
  	RCC->AHBENR = 0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.	  
  	RCC->APB2ENR = 0x00000000; //外设时钟关闭.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //使能内部高速时钟HSION
	
	RCC->CFGR &= 0xF8FF0000;   //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //复位HSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //复位HSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE
	while(((RCC->CFGR>>2)& 0x03 )!=0x00); 	
	RCC->CIR = 0x00000000;     //关闭所有中断		 
	//配置向量表				  
  
//	SCB->VTOR = 0x08000000|(0x0 & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	
 	RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	while(((RCC->CR>>17)&0x00000001)==0);//等待外部时钟就绪
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//抵消2个单位
	RCC->CFGR|=PLL<<18;   //设置PLL值 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟	 
	while(((RCC->CFGR>>2)&0x03)!=0x02);   //等待PLL作为系统时钟设置成功
	
	
}

void IO_init(void)
{
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟 
	GPIOB->CRL&=0X00FFFFFF;				//将B56口配置为通用开漏输出,最大50MH
	GPIOB->CRL|=0X07700000;				//B7配置为通用推挽输出,最大50MH
	GPIOB->ODR=0XFFFF;
}
void delay_us(unsigned int _us_time)
{       
  unsigned char x=0;
  for(;_us_time>0;_us_time--)
  {
    x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
	  x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
	  
  }
}

void delay_ms(unsigned int _ms_time)
  {
    unsigned int i,j;
    for(i=0;i<_ms_time;i++)
    {
    for(j=0;j<900;j++)
      {;}
    }
  }

/**************************IIC模块发送函数************************************************

 *************************************************************************/
//写入  最后将SDA拉高，以等待从设备产生应答
void IIC_write(unsigned char date)
{
	unsigned char i, temp;
	temp = date;
			
	for(i=0; i<8; i++)
	{	IIC_SCK_0;
		
        if ((temp&0x80)==0)
            IIC_SDA_0;
        else IIC_SDA_1;
		temp = temp << 1;
		delay_us(1);			  
		IIC_SCK_1;
		delay_us(1);
		
	}
	IIC_SCK_0;
	delay_us(1);
	IIC_SDA_1;
	delay_us(1);
	IIC_SCK_1;
//								不需要应答
//	if (READ_SDA==0)
//		ACK = 1;
//	else ACK =0;
	delay_us(1);
	IIC_SCK_0;
	delay_us(1);
	

}
//启动信号
//SCL在高电平期间，SDA由高电平向低电平的变化定义为启动信号
void IIC_start()
{
	IIC_SDA_1;
	delay_us(1);
	IIC_SCK_1;
	delay_us(1);				   //所有操作结束释放SCL	
	IIC_SDA_0;
	delay_us(3);
	IIC_SCK_0;
	
        IIC_write(0x78);
        
}

//停止信号
//SCL在高电平期间，SDA由低电平向高电平的变化定义为停止信号
void IIC_stop()
{
	IIC_SDA_0;
	delay_us(1);
	IIC_SCK_1;
	delay_us(3);
	IIC_SDA_1;
	
}

void OLED_send_cmd(unsigned char o_command)
  {
    
    IIC_start();
    IIC_write(0x00); 
    IIC_write(o_command);
    IIC_stop();
    
  }
void OLED_send_data(unsigned char o_data)
  { 
    IIC_start();
    IIC_write(0x40);
    IIC_write(o_data);
    IIC_stop();
   }
void Column_set(unsigned char column)
  {
	column+=OLED_COLUMN_OFFSET;
    OLED_send_cmd(0x10|(column>>4));    //设置列地址高位
    OLED_send_cmd(0x00|(column&0x0f));   //设置列地址低位  
    	 
  }
void Page_set(unsigned char page)
  {
    OLED_send_cmd(0xb0+page);
  }
void OLED_clear(void)
  {
    unsigned char page,column;
    for(page=0;page<OLED_PAGE_NUMBER;page++)             //page loop
      { 
          Page_set(page);
          Column_set(0);	  
          for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
            {
              OLED_send_data(0x00);
            }
      }
  }
void OLED_full(void)
  {
    unsigned char page,column;
    for(page=0;page<OLED_PAGE_NUMBER;page++)             //page loop
      { 
        Page_set(page);
        Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            OLED_send_data(0xff);
          }
      }
  }
void OLED_init(void)
  {
    unsigned char i;
    for(i=0;i<25;i++)
      {
        OLED_send_cmd(OLED_init_cmd[i]);
      }
  }

void Picture_display(const unsigned char *ptr_pic)
  {
    unsigned char page,column;
    for(page=0;page<(OLED_LINE_NUMBER/8);page++)        //page loop
      { 
	Page_set(page);
	Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            OLED_send_data(*ptr_pic++);
          }
      }
  }
void Picture_ReverseDisplay(const unsigned char *ptr_pic)
{
    unsigned char page,column,data;
    for(page=0;page<(OLED_LINE_NUMBER/8);page++)        //page loop
      { 
	Page_set(page);
	Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            data=*ptr_pic++;
            data=~data;
            OLED_send_data(data);
          }
      }
  }

int main()
{ 
  point= &picture_rev_90[0];
  IO_init();

  OLED_init();
  OLED_full();
  delay_ms(10000);
  OLED_clear();
  while(1)
  {
	
    Picture_display(point);
    delay_ms(10000);
    
    Picture_ReverseDisplay(point);
    delay_ms(10000);
    
  }
}
