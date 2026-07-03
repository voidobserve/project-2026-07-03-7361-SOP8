/******************************************************************************
;  *       @型号                   : MC32F7361
;  *       @创建日期               : 2021.12.21
;  *       @公司/作者              : SINOMCU-FAE
;  *       @晟矽微技术支持         : 2048615934
;  *       @晟矽微官网             : http://www.sinomcu.com/
;  *       @版权                   : 2021 SINOMCU公司版权所有.
;  *---------------------- 建议 ---------------------------------
;  *                   变量定义时使用全局变量
******************************************************************************/
#ifndef __USER_H__
#define __USER_H__
#include "mc32-common.h"
#include "MC32F7361.h"

/*****************************************************************
;       Function : Define variables
;*****************************************************************/

#define s8 char
// #define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define uint8_t unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long int

typedef unsigned char u8;

#define DEF_SET_BIT0 0x01
#define DEF_SET_BIT1 0x02
#define DEF_SET_BIT2 0x04
#define DEF_SET_BIT3 0x08
#define DEF_SET_BIT4 0x10
#define DEF_SET_BIT5 0x20
#define DEF_SET_BIT6 0x40
#define DEF_SET_BIT7 0x80

#define DEF_CLR_BIT0 0xFE
#define DEF_CLR_BIT1 0xFD
#define DEF_CLR_BIT2 0xFB
#define DEF_CLR_BIT3 0xF7
#define DEF_CLR_BIT4 0xEF
#define DEF_CLR_BIT5 0xDF
#define DEF_CLR_BIT6 0xBF
#define DEF_CLR_BIT7 0x7F

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define USE_MY_DEBUG 0

#define MECHANICAL_KEYING_PIN P16D // 机械按键检测引脚

// 定义433遥控器按键的键值（从波形上看是从左到右排列）
enum
{
    KEY_NONE = 0x00,
    KEY_ON = 0x01,
    KEY_OFF = 0x03,

    KEY_R2_C1 = 0x04,
    KEY_R2_C2 = 0x06,

    KEY_R3_C1 = 0x07,
    KEY_R3_C2 = 0x09,

    KEY_R4_C1 = 0x0A,
    KEY_R4_C2 = 0x0C,

    KEY_R5_C1 = 0x0D,
    KEY_R5_C2_MULTI_FLASH = 0x0F,

    KEY_R6_C1_FLASH = 0x10,
    KEY_R6_C2_FADE = 0x12,

    KEY_MECHANICAL_KEYING = 0x25, /* 机械按键 */
};

// 定义红外按键的键值（从波形上看是从左到右排列）
enum
{
    IR_KEY_NONE = 0,
    IR_KEY_R1_C1 = 0xA2, // 第一行第一列的键值
    IR_KEY_R2_C1 = 0x22, // 第二行第一列的键值
    IR_KEY_R3_C1 = 0xE0,
    IR_KEY_R4_C1 = 0x68,
    IR_KEY_R5_C1 = 0x30,
    IR_KEY_R6_C1 = 0x10,

    IR_KEY_R1_C2 = 0xE2,
    IR_KEY_R2_C2 = 0xC2,
    IR_KEY_R3_C2 = 0x90,
    IR_KEY_R4_C2 = 0xB0,
    IR_KEY_R5_C2 = 0x7A,
    IR_KEY_R6_C2 = 0x5A,
};

// 定义各个LED模式下，要发送的脉冲个数，它的完整数据帧为（若干低电平脉冲  ）
/*
    Rx_Cx，表示第x行，第x列的按键
*/
enum
{
    LED_MODE_NONE = 0,               // 表示LED已经熄灭
    LED_MODE_R2_C1 = 1,              // 红
    LED_MODE_R2_C2 = 5,              // 紫
    LED_MODE_R3_C1 = 2,              // 绿
    LED_MODE_R3_C2 = 6,              // 黄
    LED_MODE_R4_C1 = 3,              // 橙
    LED_MODE_R4_C2 = 7,              // 蓝
    LED_MODE_R5_C1 = 4,              // 白
    LED_MODE_R5_C2_MULTI_FLASH = 10, // MULTI_FLASH 所有灯一起跳变，颜色不同
    LED_MODE_R6_C1_FLASH = 9,        // FLASH 所有灯一起跳变，颜色相同
    LED_MODE_R6_C2_FADE = 8,         // FADE 呼吸
};

volatile u8 cur_led_mode; // 记录当前LED的状态

//===============Field Protection Variables===============
u8 abuf;
u8 statusbuf;

//===============Global Function===============
void Sys_Init(void);
void CLR_RAM(void);
void IO_Init(void);

//============Define  Flag=================
typedef union
{
    unsigned char byte;
    struct
    {
        u8 bit0 : 1;
        u8 bit1 : 1;
        u8 bit2 : 1;
        u8 bit3 : 1;
        u8 bit4 : 1;
        u8 bit5 : 1;
        u8 bit6 : 1;
        u8 bit7 : 1;
    } bits;
} bit_flag;
volatile bit_flag flag1;
#define flag_is_recved_data flag1.bits.bit0
#define flag_is_dev_open flag1.bits.bit1 // 标志位，灯串是否使能

#define last_level_in_ir_pin flag1.bits.bit2 // 在红外接收对应的中断函数中，表示上次引脚对应的电平

#if USE_MY_DEBUG

#define LED_CTL_PIN DEBUG_PIN // 控制灯串的、给灯串发送控制命令的引脚

#else

#define LED_CTL_PIN P04D // 控制灯串的、给灯串发送控制命令的引脚

#endif

// ===================================================
// 遥控器解码相关配置                                //
// ===================================================
// 注意，红外接收头和433接收模块不能同时使用
#if USE_MY_DEBUG
#define RFIN_PIN P13D // RF信号接收引脚
#else
#define RFIN_PIN P13D // RF信号接收引脚
#endif

// 注意，红外接收头和433接收模块不能同时使用
#define IR_RECV_PIN P13D // 红外信号接收引脚

volatile u8 rf_data; // 存放RF接收到的数据

volatile u8 ir_data; // 存放红外接收到的数据

// 毫秒级延时 (误差：在1%以内，1ms、10ms、100ms延时的误差均小于1%)
// 前提条件：FCPU = FHOSC / 4
void delay_ms(u16 xms)
{
    while (xms)
    {
        u16 i = 517;
        while (i--)
        {
            Nop();
        }
        xms--; // 把 --操作放在while()判断条件外面，更节省空间

        __asm;
        clrwdt; // 喂狗
        __endasm;
    }
}

/************************************************
;  *    @函数名            : EEPROM_Write_Byte
;  *    @说明              : 内部EEPROM写2个字节
;  *    @输入参数          : 地址  数据
;  *    @返回参数          :
;  ***********************************************/
void EEPROM_Write_Byte(uint8_t addr, uint16_t data)
{
    HFEN = 1;
    EEAR = addr;
    EEDRH = (data >> 8) & 0xff;
    EEDRL = data & 0xff;
    GIE = 0;
    EEPR = 0x5A;
    EEPR = 0xA5; // 解除保护
    EECR = 0x01; // 启动EEPROM读写  写数据操作
    Nop();
    Nop();
    while (EEWRITE) // 检查 EE 写操作是否完成
    {
        // ClrWdt();                      //清看门狗
    }
    GIE = 1;
}
/************************************************
;  *    @函数名            : EEPROM_Read_Byte
;  *    @说明              : 内部EEPROM读2个字节
;  *    @输入参数          : 地址
;  *    @返回参数          : 读到的内容
;  ***********************************************/
uint16_t EEPROM_Read_Byte(u8 addr)
{
    uint16_t EEDR;
    EEAR = addr;
    EECR = 0x02; // 启动 EE 读操作
    Nop();
    Nop();
    while (EEREAD) // 检查 EE 写操作是否完成
    {
        // ClrWdt();                      //清看门狗
    }
    EEDR = (EEDRH << 8) | EEDRL;
    return EEDR;
}

// #if USE_MY_DEBUG
#define DEBUG_PIN LED_CTL_PIN
#if 0  // 以下程序约占用81字节空间
// 通过一个引脚输出数据(发送一次约400ms)
// #define DEBUG_PIN P22D
void send_data_msb(u32 send_data)
{
    // 先发送格式头
    // __set_input_pull_up(); // 高电平
    DEBUG_PIN = 1;
    delay_ms(15);
    // __set_output_open_drain(); // 低电平
    DEBUG_PIN = 0;
    delay_ms(7); //

    for (u8 i = 0; i < 32; i++)
    {
        if ((send_data >> (32 - 1 - i)) & 0x01)
        {
            // 如果要发送逻辑1
            // __set_input_pull_up();  	   // 高电平
            DEBUG_PIN = 1;
            delay_ms(5); //
            // __set_output_open_drain(); // 低电平
            DEBUG_PIN = 0;
            delay_ms(10); //
        }
        else
        {
            // 如果要发送逻辑0
            // __set_input_pull_up();  	   // 高电平
            DEBUG_PIN = 1;
            delay_ms(5); //
            // __set_output_open_drain(); // 低电平
            DEBUG_PIN = 0;
            delay_ms(5); //
        }
    }

    // 最后，设置为低电平
    // __set_output_open_drain(); // 低电平
    DEBUG_PIN = 0;
    delay_ms(1);
    DEBUG_PIN = 1;
    delay_ms(1);
    DEBUG_PIN = 0;
}
#endif // #if USE_MY_DEBUG

#endif

/**************************** end of file *********************************************/
