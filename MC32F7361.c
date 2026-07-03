/******************************************************************************
;  *       @型号                 : MC32F7361
;  *       @创建日期             : 2021.12.21
;  *       @公司/作者            : SINOMCU-FAE
;  *       @晟矽微技术支持       : 2048615934
;  *       @晟矽微官网           : http://www.sinomcu.com/
;  *       @版权                 : 2021 SINOMCU公司版权所有.
;  *----------------------摘要描述---------------------------------

******************************************************************************/

#include "user.h"

/*
    10us级延时，前提条件：FCPU = FHOSC / 4
*/
void delay_10us(u16 x_10us)
{
    while (x_10us)
    {
        u8 i = 5;
        // u8 i = 6; // 12us
        while (i--)
        {
            Nop();
        }

        x_10us--;
    }
}

// 330us延时，前提条件 ： FCPU = FHOSC / 4
// void delay_330us(void)
// {
//     u8 i = 255;
//     while (i--)
//     {
//         Nop();
//     }
// }

// 控制信号是 若干低电平脉冲
// 传参是指 period 个低电平脉冲 + 高电平
/*
    函数内部负责发送若干数量的脉冲，保存当前的模式到EEPROM中
*/
void led_mode_handle(const u8 period)
{
    u8 i;
    u8 j;

    EEPROM_Write_Byte(1, cur_led_mode); // 保存当前的模式到EEPROM中
    EEPROM_Write_Byte(2, (u16)0xA5A6);

    GIE = 0; // 屏蔽总中断

    for (i = 0; i < 10; i++)
    {
        LED_CTL_PIN = 1;

        for (j = 0; j < period; j++)
        {
            LED_CTL_PIN = 0;
            delay_10us(1);
            LED_CTL_PIN = 1;

            delay_10us(5);
        }

        delay_ms(2); // 每个指令间隔1ms以上
    }

    GIE = 1; // 使能总中断
}

/************************************************
;  *    @函数名          : CLR_RAM
;  *    @说明            : 清RAM
;  *    @输入参数        :
;  *    @返回参数        :
;  ***********************************************/
void CLR_RAM(void)
{
    for (FSR0 = 0; FSR0 < 0xff; FSR0++)
    {
        INDF0 = 0x00;
    }
    FSR0 = 0xFF;
    INDF0 = 0x00;
}
/************************************************
;  *    @函数名            : IO_Init
;  *    @说明              : IO初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void IO_Init(void)
{
    // IOP0 = 0x00;   // io口数据位
    /* 由于433款和红外款都使用同一个板，而433版本的不用LED指示灯，
    连接到LED指示灯的引脚要输出高电平，不点亮LED */
    IOP0 = (0x01 << 0); // io口数据位，P00输出高电平
    OEP0 = 0x3F;        // io口方向 1:out  0:in
    PUP0 = 0x00;        // io口上拉电阻   1:enable  0:disable
    PDP0 = 0x00;        // io口下拉电阻   1:enable  0:disable
    P0ADCR = 0x00;      // io类型选择  1:模拟输入  0:通用io

    /* 由于433款和红外款都使用同一个板，而433版本的不用LED指示灯，
    连接到LED指示灯的引脚要输出高电平，不点亮LED */
    // IOP1 = 0x00;   // io口数据位
    IOP1 = (0x01 << 7) | (0x01 << 5); // io口数据位, P15输出高电平，P17输出高电平
    OEP1 = 0xFF;                      // io口方向 1:out  0:in
    PUP1 = 0x00;                      // io口上拉电阻   1:enable  0:disable
    PDP1 = 0x00;                      // io口下拉电阻   1:enable  0:disable
    P1ADCR = 0x00;                    // io类型选择  1:模拟输入  0:通用io

    IOP2 = 0x00; // io口数据位
    OEP2 = 0x0F; // io口方向 1:out  0:in
    PUP2 = 0x00; // io口上拉电阻   1:enable  0:disable
    PDP2 = 0x00; // io口下拉电阻   1:enable  0:disablea

    PMOD = 0x00;  // P00、P01、P13 io端口值从寄存器读，推挽输出
    DRVCR = 0x80; // 普通驱动
}

/************************************************
;  *    @函数名            : TIMER2_INT_Init
;  *    @说明              : 定时器2初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void TIMER2_INT_Init(void)
{
    T2CR = DEF_SET_BIT0 | DEF_SET_BIT1; // CPU,8分频
    T2CNT = 100 - 1;
    T2LOAD = 100 - 1; // 100us
    T2EN = 1;
    T2IE = 1;
}
/************************************************
;  *    @函数名            : Sys_Init
;  *    @说明              : 系统初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void Sys_Init(void)
{
    GIE = 0;
    CLR_RAM();
    IO_Init();
    // TIMER0_INT_Init();
    // TIMER1_INT_Init();

    TIMER2_INT_Init();

// TIMER3_INT_Init();

// 433信号检测脚
#if USE_MY_DEBUG
    // P13PU = 1; // 上拉
    // P13OE = 0; // 输入模式
#else
    P13PU = 1; // 上拉
    P13OE = 0; // 输入模式
#endif

    // 机械按键检测引脚：
    // P13PU = 1; // 上拉
    // P13OE = 0; // 输入模式

    P16PU = 1; // 上拉
    P16OE = 0; // 输入模式

    GIE = 1;
}

/**
 * @brief 按键处理函数
 *
 * @param key 键值
 * @param key_type 按键类型 0--433遥控按键，1--红外按键
 */
void key_handle(u8 key, u8 key_type)
{
    volatile u8 tmp = cur_led_mode;

    if (1 == key_type) // 如果是红外按键，需要将键值转换成433遥控器对应功能的键值
    {
        switch (key)
        {
        case IR_KEY_R1_C1:
            key = KEY_ON;
            break;

        case IR_KEY_R2_C1:
            key = KEY_R2_C1;
            break;

        case IR_KEY_R3_C1:
            key = KEY_R3_C1;
            break;

        case IR_KEY_R4_C1:
            key = KEY_R4_C1;
            break;

        case IR_KEY_R5_C1:
            key = KEY_R5_C1;
            break;

        case IR_KEY_R6_C1:
            key = KEY_R6_C1_FLASH;
            break;

        case IR_KEY_R1_C2:
            key = KEY_OFF;
            break;

        case IR_KEY_R2_C2:
            key = KEY_R2_C2;
            break;

        case IR_KEY_R3_C2:
            key = KEY_R3_C2;
            break;

        case IR_KEY_R4_C2:
            key = KEY_R4_C2;
            break;

        case IR_KEY_R5_C2:
            key = KEY_R5_C2_MULTI_FLASH;
            break;

        case IR_KEY_R6_C2:
            key = KEY_R6_C2_FADE;
            break;

        default:                // 如果是无效的键值
            cur_led_mode = tmp; // 让灯光的状态不变
            return;             // 直接退出
        }
    }

    switch (key)
    {

    case KEY_ON: // ON、开机按键
        LED_CTL_PIN = 1;

        // 如果已经开机，不再进行相应处理
        if (flag_is_dev_open)
        {
            return;
        }
        else
        {
            delay_ms(15);
            // led_mode_handle(cur_led_mode); // 发送之前存放的模式对应的数据
        }

        flag_is_dev_open = 1; // 表示设备已 开启
        break;

    case KEY_OFF: // OFF、关机按键
        LED_CTL_PIN = 0;
        flag_is_dev_open = 0; // 表示设备已 关闭
        break;

    case KEY_R2_C1:
        cur_led_mode = LED_MODE_R2_C1;
        break;

    case KEY_R2_C2:
        cur_led_mode = LED_MODE_R2_C2;
        break;

    case KEY_R3_C1:
        cur_led_mode = LED_MODE_R3_C1;
        break;

    case KEY_R3_C2:
        cur_led_mode = LED_MODE_R3_C2;
        break;

    case KEY_R4_C1:
        cur_led_mode = LED_MODE_R4_C1;
        break;

    case KEY_R4_C2:
        cur_led_mode = LED_MODE_R4_C2;
        break;

    case KEY_R5_C1:
        cur_led_mode = LED_MODE_R5_C1;
        break;

    case KEY_R5_C2_MULTI_FLASH:
        cur_led_mode = LED_MODE_R5_C2_MULTI_FLASH;
        break;

    case KEY_R6_C1_FLASH:
        cur_led_mode = LED_MODE_R6_C1_FLASH;
        break;

    case KEY_R6_C2_FADE:
        cur_led_mode = LED_MODE_R6_C2_FADE;
        break;

    case KEY_MECHANICAL_KEYING: // 机械按键按下
        if (flag_is_dev_open)   // 如果设备已经打开
        {
            switch (cur_led_mode)
            {
            case LED_MODE_R2_C1:               // 当前LED处于 红灯 模式
                cur_led_mode = LED_MODE_R3_C1; // 切换为绿灯
                break;
            case LED_MODE_R3_C1:               // 当前LED 处于 绿灯 模式
                cur_led_mode = LED_MODE_R4_C1; // 切换为橙灯
                break;
            case LED_MODE_R4_C1:               // 当前LED处于 橙灯 模式
                cur_led_mode = LED_MODE_R5_C1; // 切换为白灯
                break;
            case LED_MODE_R5_C1:               // 当前LED处于 白灯 模式
                cur_led_mode = LED_MODE_R2_C2; // 切换为紫灯
                break;
            case LED_MODE_R2_C2:               // 当前LED处于 紫灯 模式
                cur_led_mode = LED_MODE_R3_C2; // 切换为黄灯
                break;
            case LED_MODE_R3_C2:               // 当前LED处于 黄灯 模式
                cur_led_mode = LED_MODE_R4_C2; // 切换为蓝灯
                break;
            case LED_MODE_R4_C2:                    // 当前LED处于 蓝灯 模式
                cur_led_mode = LED_MODE_R6_C2_FADE; // 切换为 FADE 模式
                break;
            case LED_MODE_R6_C2_FADE:                // 当前LED处于 FADE 模式
                cur_led_mode = LED_MODE_R6_C1_FLASH; // 切换为 FLASH 模式
                break;
            case LED_MODE_R6_C1_FLASH:                     // 当前LED处于 FLASH 模式
                cur_led_mode = LED_MODE_R5_C2_MULTI_FLASH; // 切换为 MULTI_FALSH 模式
                break;
            case LED_MODE_R5_C2_MULTI_FLASH: // 当前LED处于 MULTI_FLASH 模式 --> 关闭灯光
                LED_CTL_PIN = 0;
                flag_is_dev_open = 0; // 表示设备已 关闭
                break;
            }
        }
        else // 如果设备未开启
        {
            LED_CTL_PIN = 1;
            delay_ms(5);
            cur_led_mode = LED_MODE_R2_C1; // 切换为红灯
            flag_is_dev_open = 1;          // 表示设备已 开启
        }
        break;

    default:
        // 如果是无效的键值
        cur_led_mode = tmp; // 让灯光的状态不变
        return;             // 直接退出
        break;
    }

    if (0 == flag_is_dev_open) // 如果设备未开启，不发送数据帧
    {
        cur_led_mode = tmp; // 让灯光的状态不变，防止关机后，再按下遥控器的其他模式按键，再开机，进而改变了灯光的模式
    }
    else
    {
        led_mode_handle(cur_led_mode);
    }
}

/************************************************
;  *    @函数名            : main
;  *    @说明              : 主程序
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void main(void)
{
    Sys_Init();
    delay_ms(1);

    // 上电后，从 EEPROM 中读取数据
    {
        u16 tmp;
        cur_led_mode = EEPROM_Read_Byte(1);
        tmp = EEPROM_Read_Byte(2);

        if (tmp != (u16)0xA5A6) // 如果不是 0xA5A6，说明是第一次上电
        {
            cur_led_mode = LED_MODE_R2_C1;     // 设置第一次上电的默认模式
            EEPROM_Write_Byte(2, (u16)0xA5A6); // 第一次上电，写入0xA5A6
        }
    }

    LED_CTL_PIN = 1;
    delay_ms(15); // 不能小于5ms，否则从机会接收不到
    led_mode_handle(cur_led_mode);
    flag_is_dev_open = 1;

    while (1)
    {
        // DEBUG_PIN = ~DEBUG_PIN;
        // delay_4_8_ms();

        // 是按键按下时就做对应的操作，还是按键松开时做对应操作

        /* 测试时使用: */
        // DEBUG_PIN = ~DEBUG_PIN;
        // delay_10us();

#if 1
        if (flag_is_recved_data)
        {
            if (rf_data != KEY_NONE)
            {
                key_handle(rf_data, 0);
                rf_data = 0;
            }

            if (ir_data != IR_KEY_NONE)
            {
                key_handle(ir_data, 1);
                ir_data = 0;
            }

            // send_data_msb(ir_data); // 测试接收到的数据

            flag_is_recved_data = 0;
        }
#endif
    }
}
/************************************************
;  *    @函数名            : interrupt
;  *    @说明              : 中断函数
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void int_isr(void) __interrupt
{
    __asm;
    movra _abuf;
    swapar _PFLAG;
    movra _statusbuf;
    __endasm;
    //=======T2========================
    if (T2IF & T2IE)
    {
        T2IF = 0;

        // 100us进入一次

#if 1 // rf信号接收

        static volatile u8 rf_bit_cnt;            // RF信号接收的数据位计数值
        static volatile u32 __rf_data;            // 定时器中断使用的接收缓冲区，避免直接覆盖全局的数据接收缓冲区
        static volatile u8 flag_is_enable_recv;   // 是否使能接收的标志位，要接收到 5ms+ 的低电平才开始接收
        static volatile u8 __flag_is_recved_data; // 表示中断服务函数接收到了rf数据

        static volatile u8 low_level_cnt;  // RF信号低电平计数值
        static volatile u8 high_level_cnt; // RF信号高电平计数值

        // 在定时器 中扫描端口电平
        if (0 == RFIN_PIN)
        {
            // 如果RF接收引脚为低电平，记录低电平的持续时间
            low_level_cnt++;

            /*
                下面的判断条件是避免部分遥控器或接收模块只发送24位数据，最后不拉高电平的情况
            */
            if (low_level_cnt >= 30 && rf_bit_cnt == 23) // 如果低电平大于3000us，并且已经接收了23位数据
            {
                if (high_level_cnt >= 6 && high_level_cnt < 20)
                {
                    __rf_data |= 0x01;
                }
                else if (high_level_cnt >= 1 && high_level_cnt < 6)
                {
                }

                __flag_is_recved_data = 1; // 接收完成标志位置一
                flag_is_enable_recv = 0;
            }
        }
        else
        {
            if (low_level_cnt > 0)
            {
                // 如果之前接收到了低电平信号，现在遇到了高电平，判断是否接收完成了一位数据
                if (low_level_cnt > 50)
                {
                    // 如果低电平持续时间大于50 * 100us（5ms），准备下一次再读取有效信号
                    __rf_data = 0;  // 清除接收的数据帧
                    rf_bit_cnt = 0; // 清除用来记录接收的数据位数

                    flag_is_enable_recv = 1;
                }
                else if (flag_is_enable_recv &&
                         low_level_cnt >= 2 && low_level_cnt < 7 &&
                         high_level_cnt >= 6 && high_level_cnt < 20)
                {
                    // 如果低电平持续时间在360us左右，高电平持续时间在760us左右，说明接收到了1
                    __rf_data |= 0x01;
                    rf_bit_cnt++;
                    if (rf_bit_cnt != 24)
                    {
                        __rf_data <<= 1; // 用于存放接收24位数据的变量左移一位
                    }
                }
                else if (flag_is_enable_recv &&
                         low_level_cnt >= 7 && low_level_cnt < 20 &&
                         high_level_cnt >= 1 && high_level_cnt < 6)
                {
                    // 如果低电平持续时间在840us左右，高电平持续时间在360us左右，说明接收到了0
                    __rf_data &= ~1;
                    rf_bit_cnt++;
                    if (rf_bit_cnt != 24)
                    {
                        __rf_data <<= 1; // 用于存放接收24位数据的变量左移一位
                    }
                }
                else
                {
                    // 如果低电平持续时间不符合0和1的判断条件，说明此时没有接收到信号
                    __rf_data = 0;
                    rf_bit_cnt = 0;
                    flag_is_enable_recv = 0;
                }

                low_level_cnt = 0; // 无论是否接收到一位数据，遇到高电平时，先清除之前的计数值
                high_level_cnt = 0;

                if (24 == rf_bit_cnt)
                {
                    // 如果接收成了24位的数据
                    __flag_is_recved_data = 1; // 接收完成标志位置一
                    flag_is_enable_recv = 0;
                }
            }
            else
            {
                // 如果接收到高电平后，低电平的计数为0

                if (0 == flag_is_enable_recv)
                {
                    __rf_data = 0;
                    rf_bit_cnt = 0;
                    flag_is_enable_recv = 0;
                }
            }

            // 如果RF接收引脚为高电平，记录高电平的持续时间
            high_level_cnt++;
        }

        if (__flag_is_recved_data) //
        {
            rf_bit_cnt = 0;
            __flag_is_recved_data = 0;
            low_level_cnt = 0;
            high_level_cnt = 0;

            // if (rf_data != 0)
            if (0 == flag_is_recved_data) /* 如果之前未接收到数据 或是 已经处理完上一次接收到的数据 */
            // if ((0 == flag_is_recved_data) && /* 如果之前未接收到数据 或是 已经处理完上一次接收到的数据 */
            //     ((__rf_data & 0xFFFF00) == 0xFFFF00)) /* 如果遥控器的地址码一致 */
            {
                rf_data = __rf_data;
                flag_is_recved_data = 1;
            }
            else
            {
                __rf_data = 0;
            }
        }

#endif // rf信号接收

#if 1 // 100us定时的情况下，进行机械按键检测

        /*
            这里还未加入长按后，等到松手才继续检测机械按键的功能
        */

        { // 机械按键检测
            static u16 key_press_cnt;
            if (0 == MECHANICAL_KEYING_PIN)
            {
                if (key_press_cnt == 10000) // 长按
                {
                    rf_data = KEY_OFF;
                    flag_is_recved_data = 1;
                }
                else if (key_press_cnt < 10000)
                {
                    key_press_cnt++;
                }
            }
            else // 抬起
            {
                if (key_press_cnt >= 100 && key_press_cnt < 10000) // 短按
                {
                    rf_data = KEY_MECHANICAL_KEYING;
                    flag_is_recved_data = 1;
                }

                key_press_cnt = 0;
            }
        } // 机械按键检测

#endif // 100us定时的情况下，进行机械按键检测

#if 1     // 红外解码 放到100us的定时器中断
        { // 红外解码
            // static volatile u8 ir_fliter;
            static volatile u16 ir_level_cnt; // 红外信号的下降沿时间间隔计数
            static volatile u32 __ir_data;    //
            // static volatile u16 ir_long_press_cnt; // 檢測紅外遙控長按的計數值

            // 对每个下降沿进行计数
            if (ir_level_cnt <= 1300)
                ir_level_cnt++;

            // ir_fliter <<= 1;
            // if (IR_RECV_PIN)
            // {
            //     ir_fliter |= 1;
            // }
            // ir_fliter &= 0x07;

            // if (ir_fliter == 0x07)
            //     filter_level = 1;
            // else if (ir_fliter == 0x00)
            //     filter_level = 0;

            // if (filter_level)
            if (IR_RECV_PIN)
            {
                last_level_in_ir_pin = 1; // 表示接收到的是高电平

                // 如果之前也是高电平
                if (ir_level_cnt > 1200) // 超时时间(120000us,120ms)
                {
                    // if (__ir_data != 0) // 超时，且接收到数据(红外接收处理函数中会把ir_data清零)
                    if (__ir_data != 0) // 超时，且接收到数据(现在是在中断服务函数中把__ir_data自动清零)
                    {
                        // // 带校验的版本：
                        // if ((u8)(__ir_data >> 8) == (u8)(~__ir_data)) // 如果键值的原码和反码相等
                        // {
                        // flag_is_recved_data = 1;
                        // }

                        // 不带校验的版本
                        if (0 == flag_is_recved_data)
                        {

#if 1 // 如果是样机的遥控，才进行处理，其他遥控器不处理
      // if ((__ir_data & 0xFF0000) == 0xFF0000)
                            {
                                ir_data = ~__ir_data;
                                __ir_data = 0;
                                flag_is_recved_data = 1;
                            }
#endif // 如果是样机的遥控，才进行处理，其他遥控器不处理
                        }
                    }

                    // flag_is_recv_ir_repeat_code = 0; // 认为遥控器按键已经按下，然后松开
                }
            }
            else
            {
                if (last_level_in_ir_pin)
                {
                    // 如果之前检测到的是高电平，现在检测到了低电平
                    if (ir_level_cnt <= 8) // 小于800us，说明接收到无效的数据，重新接收
                    {
                    }
                    else if (ir_level_cnt <= 18) // 小于1800us,说明接收到了逻辑0
                    {
                        __ir_data <<= 1;

                        // P15D = 0; // 测试红外解码
                        // DEBUG_PIN = 0;
                        // P15D = ~P15D; // 测试红外解码
                    }
                    else if (ir_level_cnt <= 26) // 小于2600us,说明接收到了逻辑1
                    {
                        __ir_data <<= 1;
                        __ir_data |= 0x01;

                        // P15D = 1; // 测试红外解码
                        // DEBUG_PIN = 1;
                    }
                    else if (ir_level_cnt <= 150) // 小于15000us,说明接收到了格式头
                    {
                    }
                    else if (ir_level_cnt <= 420) // 小于42ms,说明接收完一次完整的红外信号
                    {
#if 0 // 带校验的版本，命令源码与命令反码进行校验
    
                if ((u8)(__ir_data >> 8) == (u8)(~__ir_data)) // 如果键值的原码和反码相等
                {
                    flag_is_recved_data = 1;
                    flag_is_recv_ir_repeat_code = 1; //
                    ir_long_press_cnt = 0;
                }
#else // 不带校验的版本
                        if (0 == flag_is_recved_data)
                        {
#if 1 // 如果是样机的遥控，才进行处理，其他遥控器不处理
      // if ((__ir_data & 0xFF0000) == 0xFF0000)
                            {
                                ir_data = ~__ir_data;
                                __ir_data = 0;
                                flag_is_recved_data = 1;
                                // flag_is_recv_ir_repeat_code = 1; //
                            }
#endif // 如果是样机的遥控，才进行处理，其他遥控器不处理
                        }
#endif // 不带校验的版本
                    }
                    // else if (ir_level_cnt <= 1200) // 小于120000,120ms,说明接收到了重复码
                    // {
                    //     // if (ir_long_press_cnt < 65535)
                    //     //     ir_long_press_cnt++;
                    // }
                    // else // 超过120000,说明接收到无效的数据
                    // {
                    // }

                    ir_level_cnt = 0;
                }

                last_level_in_ir_pin = 0; // 表示接收到的是低电平
            }
        } // 红外解码
#endif // 红外解码 放到100us的定时器中断

    } // if (T2IF & T2IE)

    __asm;
    swapar _statusbuf;
    movra _PFLAG;
    swapr _abuf;
    swapar _abuf;
    __endasm;
}

/**************************** end of file *********************************************/
