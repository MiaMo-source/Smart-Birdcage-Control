/**
 * RT-Thread 平台 GD5800 MP3 模块驱动实现
 *
 * 基于 Arduino 库修改，适配 RT-Thread 操作系统
 */

#include "GD5800_Serial.h"
//
//// 内部函数声明
//static void sendCommand(GD5800_Serial* mp3, unsigned char command, unsigned char arg1, unsigned char arg2);
//static unsigned int sendCommandWithResponse(GD5800_Serial* mp3, unsigned char command);
//static void flushSerialBuffer(GD5800_Serial* mp3);
//
///**
// * 创建并初始化 GD5800 对象
// * @param rxPin 接收引脚（Arduino兼容保留，实际使用串口设备）
// * @param txPin 发送引脚（Arduino兼容保留，实际使用串口设备）
// * @return 成功返回对象指针，失败返回 NULL
// */
//GD5800_Serial* GD5800_Serial_Create(short rxPin, short txPin)
//{
//    GD5800_Serial* mp3 = (GD5800_Serial*)rt_malloc(sizeof(GD5800_Serial));
//    if (mp3 == RT_NULL) {
//        rt_kprintf("Failed to allocate memory for GD5800_Serial!\n");
//        return RT_NULL;
//    }
//
//    // 初始化成员变量
//    mp3->rxPin = rxPin;
//    mp3->txPin = txPin;
//
//    // 查找并打开串口设备（修改为使用 uart2）
//    mp3->serial_dev = rt_device_find("uart2");
//    if (mp3->serial_dev == RT_NULL) {
//        rt_kprintf("Failed to find UART2 device!\n");
//        rt_free(mp3);
//        return RT_NULL;
//    }
//
//    // 设置串口参数（9600-8-N-1）
//    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
//    config.baud_rate = BAUD_RATE_9600;
//    if (rt_device_control(mp3->serial_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK) {
//        rt_kprintf("Failed to set serial device configuration!\n");
//        rt_device_close(mp3->serial_dev);
//        rt_free(mp3);
//        return RT_NULL;
//    }
//
//    // 以读写模式打开串口
//    if (rt_device_open(mp3->serial_dev, RT_DEVICE_FLAG_INT_RX ) != RT_EOK) {
//        rt_kprintf("Failed to open serial device!\n");
//        rt_free(mp3);
//        return RT_NULL;
//    }
//
//    rt_kprintf("GD5800 MP3 module initialized on UART2\n");
//    return mp3;
//}
//
///**
// * 销毁 GD5800 对象并释放资源
// * @param mp3 对象指针
// */
//void GD5800_Serial_Destroy(GD5800_Serial* mp3)
//{
//    if (mp3) {
//        if (mp3->serial_dev) {
//            rt_device_close(mp3->serial_dev);
//        }
//        rt_free(mp3);
//    }
//}
//
///**
// * 发送命令到 GD5800 模块
// * @param mp3 对象指针
// * @param command 命令字节
// * @param arg1 第一个参数
// * @param arg2 第二个参数
// */
//static void sendCommand(GD5800_Serial* mp3, unsigned char command, unsigned char arg1, unsigned char arg2)
//{
//    if (!mp3 || !mp3->serial_dev) return;
//
//    // 计算参数数量
//    unsigned char args = 0;
//    switch(command) {
//        case 0x41: args = 2; break;  // 指定曲目播放
//        case 0x43: args = 2; break;  // 指定曲目插播
//        case 0x31: args = 1; break;  // 设置音量
//        case 0x32: args = 1; break;  // 设置 EQ
//        case 0x33: args = 1; break;  // 设置循环模式
//        case 0x35: args = 1; break;  // 设置播放设备
//        default: args = 0; break;
//    }
//
//    // 构建命令包
//    unsigned char cmd[10];
//    int index = 0;
//    cmd[index++] = 0x7E;            // 帧头
//    cmd[index++] = 2 + args;        // 数据长度
//    cmd[index++] = command;         // 命令
//    if (args >= 1) cmd[index++] = arg1;  // 参数1
//    if (args >= 2) cmd[index++] = arg2;  // 参数2
//    cmd[index++] = 0xEF;            // 帧尾
//
//    // 清空接收缓冲区
//    flushSerialBuffer(mp3);
//
//    // 发送命令
//    rt_kprintf("Sending command: ");
//    for (int i = 0; i < index; i++) {
//        rt_kprintf("%02X ", cmd[i]);
//    }
//    rt_kprintf("\n");
//
//    rt_device_write(mp3->serial_dev, 0, cmd, index);
//
//    // 等待命令执行（根据实际情况调整延时）
//    rt_thread_mdelay(50);
//}
//
///**
// * 发送带响应的命令并获取返回值
// * @param mp3 对象指针
// * @param command 命令字节
// * @return 命令返回值
// */
//static unsigned int sendCommandWithResponse(GD5800_Serial* mp3, unsigned char command)
//{
//    if (!mp3 || !mp3->serial_dev) return 0;
//
//    // 发送命令（无参数）
//    sendCommand(mp3, command, 0, 0);
//
//    // 读取响应
//    unsigned char buffer[16] = {0};
//    int len = rt_device_read(mp3->serial_dev, 0, buffer, sizeof(buffer) - 1);
//
//    // 解析响应（简化处理，实际应根据协议解析）
//    if (len > 0) {
//        // 示例：假设返回值是一个无符号整数
//        unsigned int result = 0;
//        for (int i = 0; i < len; i++) {
//            if (buffer[i] >= '0' && buffer[i] <= '9') {
//                result = result * 10 + (buffer[i] - '0');
//            }
//        }
//        return result;
//    }
//
//    return 0;
//}
//
///**
// * 清空串口接收缓冲区
// * @param mp3 对象指针
// */
//static void flushSerialBuffer(GD5800_Serial* mp3)
//{
//    if (!mp3 || !mp3->serial_dev) return;
//
//    unsigned char dummy;
//    while (rt_device_read(mp3->serial_dev, 0, &dummy, 1) > 0);
//}
//
///* ====================================== */
///*            播放控制函数                */
///* ====================================== */
//
//void  GD5800_Play(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x01, 0, 0);
//}
//
//void  GD5800_Restart(GD5800_Serial* mp3)
//{
//    unsigned char oldVolume = GD5800_GetVolume(mp3);
//    GD5800_SetVolume(mp3, 0);    // 静音
//    GD5800_Next(mp3);           // 下一曲
//    GD5800_Pause(mp3);          // 暂停
//    GD5800_SetVolume(mp3, oldVolume);  // 恢复音量
//    GD5800_Prev(mp3);           // 上一曲（即重新开始当前曲目）
//}
//
//void  GD5800_Pause(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x02, 0, 0);
//}
//
//void  GD5800_Next(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x03, 0, 0);
//}
//
//void  GD5800_Prev(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x04, 0, 0);
//}
//
//void  GD5800_FastReverse(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x0B, 0, 0);
//}
//
//void  GD5800_FastForward(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x0A, 0, 0);
//}
//
//void  GD5800_PlayFileByIndexNumber(GD5800_Serial* mp3, unsigned int fileNumber)
//{
//    sendCommand(mp3, 0x41, (fileNumber >> 8) & 0xFF, fileNumber & 0xFF);
//}
//
//void  GD5800_PushPlayFileByIndexNumber(GD5800_Serial* mp3, unsigned int fileNumber)
//{
//    sendCommand(mp3, 0x43, (fileNumber >> 8) & 0xFF, fileNumber & 0xFF);
//}
//
///* ====================================== */
///*            参数设置函数                */
///* ====================================== */
//
//void  GD5800_VolumeUp(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x05, 0, 0);
//}
//
//void  GD5800_VolumeDn(GD5800_Serial* mp3)
//{
//    sendCommand(mp3, 0x06, 0, 0);
//}
//
//void  GD5800_SetVolume(GD5800_Serial* mp3, unsigned char volumeFrom0To30)
//{
//    // 确保音量在有效范围内
//    if (volumeFrom0To30 > 30) volumeFrom0To30 = 30;
//    sendCommand(mp3, 0x31, volumeFrom0To30, 0);
//}
//
//void  GD5800_SetEqualizer(GD5800_Serial* mp3, MP3_EQ equalizerMode)
//{
//    // 确保 EQ 模式在有效范围内
//    if (equalizerMode > MP3_EQ_BASS) equalizerMode = MP3_EQ_NORMAL;
//    sendCommand(mp3, 0x32, equalizerMode, 0);
//}
//
//void  GD5800_SetLoopMode(GD5800_Serial* mp3, MP3_LOOP loopMode)
//{
//    // 确保循环模式在有效范围内
//    if (loopMode > MP3_LOOP_RAM) loopMode = MP3_LOOP_ALL;
//    sendCommand(mp3, 0x33, loopMode, 0);
//}
//
//void  GD5800_SetDevice(GD5800_Serial* mp3, MP3_DEVICE device)
//{
//    sendCommand(mp3, 0x35, device, 0);
//}
//
///* ====================================== */
///*            状态查询函数                */
///* ====================================== */
//
//MP3_STATUS  GD5800_GetStatus(GD5800_Serial* mp3)
//{
//    return (MP3_STATUS)sendCommandWithResponse(mp3, 0x42);
//}
//
//unsigned char  GD5800_GetVolume(GD5800_Serial* mp3)
//{
//    return (unsigned char)sendCommandWithResponse(mp3, 0x11);
//}
//
//MP3_EQ  GD5800_GetEqualizer(GD5800_Serial* mp3)
//{
//    return (MP3_EQ)sendCommandWithResponse(mp3, 0x44);
//}
//
//MP3_LOOP  GD5800_GetLoopMode(GD5800_Serial* mp3)
//{
//    return (MP3_LOOP)sendCommandWithResponse(mp3, 0x13);
//}
//
//unsigned int  GD5800_CountFiles(GD5800_Serial* mp3)
//{
//    return sendCommandWithResponse(mp3, 0x16);
//}
//
//unsigned int  GD5800_CurrentFileIndexNumber(GD5800_Serial* mp3)
//{
//    return sendCommandWithResponse(mp3, 0x1A);
//}
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_UART_NAME "uart2"
static rt_device_t serial;
static struct rt_semaphore rx_sem;

/* 接收数据回调函数 */
static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    if (size > 0)
    {
        rt_sem_release(&rx_sem);
    }
    return RT_EOK;
}

/* 获取接收到的字符 */
static char uart_sample_get_char(void)
{
    char ch;
    while (rt_device_read(serial, 0, &ch, 1) == 0)
    {
        //rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

/* 串口接收线程 */
static void uart_receive_thread(void *parameter)
{
    char ch;
    while (1)
    {
        ch = uart_sample_get_char();
        /* 实时显示接收到的数据 */
        rt_kprintf("Recv: 0x%02X", (unsigned char)ch);
        if (ch >= 0x20 && ch <= 0x7E) /* 可打印字符 */
        {
            rt_kprintf(" ('%c')\n", ch);
        }
        else
        {
            rt_kprintf(" (不可打印)\n");
        }
    }
}

/* 初始化串口 */
static int uart_init(void)
{
    char uart_name[] = SAMPLE_UART_NAME;

    /* 查找串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("找不到串口设备 %s\n", uart_name);
        return -RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* 以中断接收模式打开串口 */
    if (rt_device_open(serial, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("无法打开串口设备\n");
        return -RT_ERROR;
    }

    /* 设置波特率9600 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_9600;
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* 设置接收回调 */
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    /* 创建接收线程 */
    rt_thread_t thread = rt_thread_create("uart_recv",
                                        uart_receive_thread,
                                        RT_NULL,
                                        1024,
                                        25,
                                        10);
    if (thread)
    {
        rt_thread_startup(thread);
        rt_kprintf("串口接收线程已启动\n");
    }
    else
    {
        rt_kprintf("无法创建接收线程\n");
        return -RT_ERROR;
    }

    return RT_EOK;
}

/* 发送命令 */
static void send_command(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("用法: send <hex_string>\n");
        rt_kprintf("示例: send 3A3B\n");
        return;
    }

    char *hex_str = argv[1];
    int len = strlen(hex_str);

    /* 检查十六进制字符串长度 */
    if (len % 2 != 0)
    {
        rt_kprintf("错误: 十六进制字符串长度必须为偶数\n");
        return;
    }

    /* 转换十六进制字符串 */
    int data_len = len / 2;
    rt_uint8_t *data = (rt_uint8_t *)rt_malloc(data_len);
    if (!data)
    {
        rt_kprintf("内存分配失败\n");
        return;
    }

    for (int i = 0; i < data_len; i++)
    {
        char byte_str[3] = { hex_str[2*i], hex_str[2*i+1], '\0' };
        data[i] = (rt_uint8_t)strtol(byte_str, NULL, 16);
    }

    /* 发送数据 */
    rt_size_t sent = rt_device_write(serial, 0, data, data_len);
    rt_kprintf("发送 %d 字节: %s\n", sent, hex_str);

    rt_free(data);
}

/* 初始化函数 */
static int uart_sample_init(void)
{
    rt_kprintf("正在初始化串口...\n");
    return uart_init();
}
INIT_APP_EXPORT(uart_sample_init);

/* 导出命令 */
MSH_CMD_EXPORT(send_command, 发送十六进制数据到UART2);



// 发送MP3命令函数
void send_mp3_command(const char *hex_str)
{
    // 检查输入有效性
    if (hex_str == RT_NULL || strlen(hex_str) == 0) {
        rt_kprintf("错误: 输入字符串为空\n");
        return;
    }

    size_t len = strlen(hex_str);
    if (len % 2 != 0) {
        rt_kprintf("错误: 十六进制字符串长度必须为偶数\n");
        return;
    }

    // 计算字节数
    size_t byte_count = len / 2;
    if (byte_count < 1 || byte_count > 3) {
        rt_kprintf("错误: 命令长度必须在1-3字节之间\n");
        return;
    }

    // 转换十六进制字符串为字节
    rt_uint8_t bytes[3];
    for (int i = 0; i < byte_count; i++) {
        char byte_str[3] = { hex_str[i*2], hex_str[i*2+1], '\0' };
        bytes[i] = (rt_uint8_t)strtol(byte_str, NULL, 16);
    }

    // 构建命令帧
    rt_uint8_t frame[6];
    size_t frame_len = 0;

    frame[frame_len++] = 0x7E;                  // 起始字节
    frame[frame_len++] = 2 + (byte_count - 1);  // 长度字段

    // 命令和参数
    for (int i = 0; i < byte_count; i++) {
        frame[frame_len++] = bytes[i];
    }

    frame[frame_len++] = 0xEF;                  // 结束字节

    // 发送前清空接收缓冲区
    char ch;
    while (rt_device_read(serial, 0, &ch, 1) > 0) {
        // 丢弃接收到的数据
    }

    // 发送命令帧
    rt_size_t sent = rt_device_write(serial, 0, frame, frame_len);

    // 打印调试信息
    rt_kprintf("发送命令: ");
    for (int i = 0; i < frame_len; i++) {
        rt_kprintf("%02X ", frame[i]);
    }
    rt_kprintf("(%d 字节)\n", sent);
}

// 命令处理函数
static void mp3_command(int argc, char *argv[])
{
    if (argc < 2) {
        rt_kprintf("用法: mp3_command <hex_command>\n");
        rt_kprintf("示例: mp3_command 30       # 播放\n");
        rt_kprintf("      mp3_command 310A     # 设置音量为10\n");
        rt_kprintf("      mp3_command 410005   # 播放第5首曲目\n");
        return;
    }

    send_mp3_command(argv[1]);
}
MSH_CMD_EXPORT(mp3_command, send mp3 command);

static void play_command(int argc, char *argv[])
{
    send_mp3_command("01");
}
MSH_CMD_EXPORT(play_command, send mp3 command);
