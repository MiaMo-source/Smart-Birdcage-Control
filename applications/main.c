/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-27     RT-Thread    first version
 * 2024-11-XX     Happy     Modified for smart birdcage with MP3 module
 */

#include <rtthread.h>
#include <rtdbg.h>
#include <rtdevice.h>
#include <board.h>
#include "GD5800_Serial.h"

// 定义PWM相关参数
#define PWM_DEV_NAME        "pwm4"      /* PWM设备名称 */
#define PWM_DEV_CHANNEL     1           /* PWM通道 */
#define SERVO_PERIOD        20000000    /* 周期为20ms，单位为纳秒ns */
#define MIN_PULSE_WIDTH     500000     /* 最小脉冲宽度，对应0度，单位为纳秒ns */
#define MAX_PULSE_WIDTH     2500000     /* 最大脉冲宽度，对应180度，单位为纳秒ns */

/* 新增蓝牙配置 */
#define BLUETOOTH_UART_NAME       "uart3"          // 蓝牙模块连接的串口设备名
#define BLUETOOTH_BAUD_RATE       9600             // 蓝牙模块默认波特率
#define RX_BUFFER_SIZE            128              // 接收缓冲区大小

/* 全局变量 */
static rt_device_t bluetooth_uart;                 // 蓝牙串口设备句柄
static struct rt_semaphore rx_sem;                 // 接收信号量
static char rx_buffer[RX_BUFFER_SIZE];             // 接收缓冲区

struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
static rt_sem_t sound_sem;

/* 串口接收回调函数 */
static rt_err_t uart_rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);  // 释放信号量通知有数据到达
    return RT_EOK;
}

/* 蓝牙遥控线程 */
static void bluetooth_control_thread_entry(void *parameter)
{
    rt_size_t rx_length;
    int direction = 0; // 0:停止, 1:正转, 2:反转
    int speed = 0;     // 速度百分比(0-100)

    rt_kprintf("蓝牙控制线程启动\n");

    // 初始化信号量
    rt_sem_init(&rx_sem, "bt_rx", 0, RT_IPC_FLAG_FIFO);

    // 查找蓝牙串口设备
    bluetooth_uart = rt_device_find(BLUETOOTH_UART_NAME);
    if (bluetooth_uart == RT_NULL) {
        rt_kprintf("错误：找不到 %s 设备！\n", BLUETOOTH_UART_NAME);
        return;
    }

    // 配置串口参数
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BLUETOOTH_BAUD_RATE;
    rt_device_control(bluetooth_uart, RT_DEVICE_CTRL_CONFIG, &config);

    // 打开串口设备 (中断接收模式)
    if (rt_device_open(bluetooth_uart, RT_DEVICE_FLAG_INT_RX) != RT_EOK) {
        rt_kprintf("错误：无法打开 %s 设备！\n", BLUETOOTH_UART_NAME);
        return;
    }

    // 设置接收回调函数
    rt_device_set_rx_indicate(bluetooth_uart, uart_rx_callback);

    rt_kprintf("蓝牙串口初始化完成，等待指令...\n");

    while (1) {
        // 等待接收信号量
        if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) == RT_EOK) {
            // 读取接收到的数据
            rx_length = rt_device_read(bluetooth_uart, 0, rx_buffer, RX_BUFFER_SIZE);

            if (rx_length > 0) {
                // 添加字符串结束符
                rx_buffer[rx_length] = '\0';
                rt_kprintf("收到蓝牙指令: %s\n", rx_buffer);

                // 解析指令 (示例格式: "{direction:1,speed:50}")
                // 在实际应用中，这里需要实现完整的协议解析
                if (rt_strstr(rx_buffer, "direction:1")) {
                    direction = 1;
                    rt_kprintf("指令解析: 正转\n");
                }
                else if (rt_strstr(rx_buffer, "direction:2")) {
                    direction = 2;
                    rt_kprintf("指令解析: 反转\n");
                }
                else if (rt_strstr(rx_buffer, "direction:0")) {
                    direction = 0;
                    rt_kprintf("指令解析: 停止\n");
                }

                // 解析速度
                char *speed_ptr = rt_strstr(rx_buffer, "speed:");
                if (speed_ptr) {
                    speed = atoi(speed_ptr + 6); // 跳过"speed:"
                    rt_kprintf("指令解析: 速度 %d%%\n", speed);
                }

                // 根据解析结果控制电机
                if (direction == 1) {
                    rt_kprintf("执行: 逗鸟棒电机正转，速度 %d%%\n", speed);
                    // motor_control(BIRD_STICK_MOTOR_PIN, 1);
                }
                else if (direction == 2) {
                    rt_kprintf("执行: 逗鸟棒电机反转，速度 %d%%\n", speed);
                    // motor_control(BIRD_STICK_MOTOR_PIN, 2);
                }
                else {
                    rt_kprintf("执行: 逗鸟棒电机停止\n");
                    // motor_control(BIRD_STICK_MOTOR_PIN, 0);
                }
            }
        }

        rt_thread_mdelay(10);
    }
}
/*
 * 初始化蓝牙模块的AT指令：
 *   AT+NAMEBirdCage    // 设置设备名为"BirdCage"
 *   AT+BAUD7           // 设置波特率为115200(可选)
 *   AT+PIN8888         // 设置配对密码为8888
 */
// 将角度转换为脉冲宽度
static rt_uint32_t angle_to_pulse_width(int angle)
{
    /* 角度范围检查 */
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    /* 线性映射：angle[0,180] -> pulse[MIN_PULSE_WIDTH,MAX_PULSE_WIDTH] */
    return MIN_PULSE_WIDTH + (angle * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180);
}

/* 声音传感器触发引脚定义 */
#define SOUND_SENSOR_PIN        GET_PIN(D, 1)  /* PD1 */

/* 其他引脚定义 */
#define FOOD_MOTOR_PIN          2
#define BIRD_STICK_MOTOR_PIN    3
#define BUZZER_PIN              5

// 定义电机控制函数
void motor_control(int pin, int state)
{
    // 这里实现电机控制的具体逻辑，例如使用GPIO控制
}


/* 中断回调函数 */
void sound_trigger(void *args)
{
    rt_kprintf("检测到声音过大信号！\n");
   rt_sem_release(sound_sem);

}

static void sound_sensor_trigger_init(void)
{
    /* 配置PD1引脚为悬空输入模式 */
    rt_pin_mode(SOUND_SENSOR_PIN, PIN_MODE_INPUT);

    /* 绑定中断，下降沿触发模式，回调函数为sound_trigger */
    rt_pin_attach_irq(SOUND_SENSOR_PIN, PIN_IRQ_MODE_RISING, sound_trigger, RT_NULL);

    /* 使能中断 */
    rt_pin_irq_enable(SOUND_SENSOR_PIN, PIN_IRQ_ENABLE);

    rt_kprintf("声音传感器触发中断已初始化 (PD1引脚)\n");
}

// 读取声音传感器状态
int read_sound_sensor(void)
{
    return rt_pin_read(SOUND_SENSOR_PIN);
}

// 定时加水线程
static void water_add_thread_entry(void *parameter)
{
    int open_angle = 90; // 舵机打开角度
    int close_angle = 0; // 舵机关闭角度
    rt_uint32_t open_pulse, close_pulse;

    // 查找设备
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("加水控制失败！找不到 %s 设备！\n", PWM_DEV_NAME);
        return;
    }

    // 计算打开和关闭的脉冲宽度
    open_pulse = angle_to_pulse_width(open_angle);
    close_pulse = angle_to_pulse_width(close_angle);

    while (1)
    {
        rt_thread_mdelay(30 * 60 * 1000); // 每30分钟执行一次

        // 打开加水舵机
        rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, SERVO_PERIOD, open_pulse);
        rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
        rt_kprintf("加水舵机已打开\n");

        rt_thread_mdelay(1000); // 加水持续1秒

        // 关闭加水舵机
        rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, SERVO_PERIOD, close_pulse);
        rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
        rt_kprintf("加水舵机已关闭\n");
    }
}

// 定时加鸟食线程
static void food_add_thread_entry(void *parameter)
{
    while (1)
    {
        rt_thread_mdelay(3 * 60 * 60 * 1000); // 每3小时执行一次
        motor_control(FOOD_MOTOR_PIN, 1); // 开启加鸟食电机
        rt_thread_mdelay(1000); // 加鸟食持续1秒
        motor_control(FOOD_MOTOR_PIN, 0); // 关闭加鸟食电机
    }
}

// 镇定功能线程
static void calm_bird_thread_entry(void *parameter)
{
    sound_sem=rt_sem_create("sound_sem", 0, RT_IPC_FLAG_PRIO);
    if(sound_sem==RT_NULL){
        rt_kprintf("sound_sem ERROR!!!\n");
    }
    while (1)
    {
            rt_sem_take(sound_sem, RT_WAITING_FOREVER);
            rt_kprintf("检测到声音触发，播放安抚音乐\n");
            send_mp3_command("01");
    }
}

int main(void)
{
    int count = 1;

    // 创建定时加水线程
    rt_thread_t water_add_thread = rt_thread_create("water_add", water_add_thread_entry, RT_NULL, 1024, 25, 10);
    if (water_add_thread != RT_NULL)
    {
        rt_thread_startup(water_add_thread);
    }

    // 创建定时加鸟食线程
    rt_thread_t food_add_thread = rt_thread_create("food_add", food_add_thread_entry, RT_NULL, 1024, 25, 10);
    if (food_add_thread != RT_NULL)
    {
        rt_thread_startup(food_add_thread);
    }

    // 创建蓝牙遥控线程
    rt_thread_t bluetooth_control_thread = rt_thread_create("bluetooth_control", bluetooth_control_thread_entry, RT_NULL, 1024, 25, 10);
    if (bluetooth_control_thread != RT_NULL)
    {
        rt_thread_startup(bluetooth_control_thread);
    }

    // 创建镇定功能线程
    rt_thread_t calm_bird_thread = rt_thread_create("calm_bird", calm_bird_thread_entry, RT_NULL, 1024, 25, 10);
    if (calm_bird_thread != RT_NULL)
    {
        rt_thread_startup(calm_bird_thread);
    }
    sound_sensor_trigger_init();


    return RT_EOK;
}
