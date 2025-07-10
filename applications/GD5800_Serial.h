/**
 * RT-Thread 平台 GD5800 MP3 模块驱动
 *
 * 基于 Arduino 库修改，适配 RT-Thread 操作系统
 *
 * @author James Sleeman (原作者)
 * @author Happy (RT-Thread 移植)
 * @license MIT License
 */

#ifndef GD5800Serial_h
#define GD5800Serial_h

#ifdef __cplusplus
extern "C"
{
#endif

//
//#include <rtthread.h>
//#include <rtdevice.h>
//
// // 定义枚举和常量
//typedef enum {
//    MP3_EQ_NORMAL = 0,    // 普通音效
//    MP3_EQ_POP = 1,       // 流行音乐
//    MP3_EQ_ROCK = 2,      // 摇滚音乐
//    MP3_EQ_JAZZ = 3,      // 爵士音乐
//    MP3_EQ_CLASSIC = 4,   // 古典音乐
//    MP3_EQ_BASS = 5       // 重低音
//} MP3_EQ;
//
//typedef enum {
//    MP3_DEVICE_UDISK = 0,         // U 盘设备
//    MP3_DEVICE_FLASH = 3,         // 内置闪存
//    MP3_DEVICE_FLASH_PLAY = 4     // 切换到闪存并播放
//} MP3_DEVICE;
//
//typedef enum {
//    MP3_LOOP_ALL = 0,     // 全部循环
//    MP3_LOOP_FOLDER = 1,  // 文件夹循环
//    MP3_LOOP_ONE = 2,     // 单曲循环
//    MP3_LOOP_RAM = 3      // RAM 模式循环
//} MP3_LOOP;
//
//typedef enum {
//    MP3_STATUS_STOPPED = 0,  // 停止状态
//    MP3_STATUS_PLAYING = 1,  // 播放状态
//    MP3_STATUS_PAUSED = 2,   // 暂停状态
//    MP3_STATUS_FF = 3,       // 快进状态
//    MP3_STATUS_FR = 4        // 快退状态
//} MP3_STATUS;
//
//// 定义GD5800结构体
//typedef struct {
//    struct rt_device* serial_dev;  // 串口设备句柄
//    short rxPin;                   // 接收引脚（Arduino兼容保留）
//    short txPin;                   // 发送引脚（Arduino兼容保留）
//} GD5800_Serial;
//
//// 函数声明
//GD5800_Serial* GD5800_Serial_Create(short rxPin, short txPin);
//void GD5800_Serial_Destroy(GD5800_Serial* mp3);
//
///**
// * 控制函数
// */
//void  GD5800_Play(GD5800_Serial* mp3);                 // 播放
//void  GD5800_Restart(GD5800_Serial* mp3);              // 重新播放当前曲目
//void  GD5800_Pause(GD5800_Serial* mp3);                // 暂停
//void  GD5800_Next(GD5800_Serial* mp3);                 // 下一曲
//void  GD5800_Prev(GD5800_Serial* mp3);                 // 上一曲
//void  GD5800_FastReverse(GD5800_Serial* mp3);          // 快退
//void  GD5800_FastForward(GD5800_Serial* mp3);          // 快进
//
///**
// * 文件操作
// */
//void  GD5800_PlayFileByIndexNumber(GD5800_Serial* mp3, unsigned int fileNumber);  // 按索引播放
//void  GD5800_PushPlayFileByIndexNumber(GD5800_Serial* mp3, unsigned int fileNumber);  // 插播指定文件
//
///**
// * 音量和音效控制
// */
//void  GD5800_VolumeUp(GD5800_Serial* mp3);             // 音量增加
//void  GD5800_VolumeDn(GD5800_Serial* mp3);             // 音量减少
//void  GD5800_SetVolume(GD5800_Serial* mp3, unsigned char volumeFrom0To30);  // 设置音量(0-30)
//void  GD5800_SetEqualizer(GD5800_Serial* mp3, MP3_EQ equalizerMode);  // 设置音效模式
//void  GD5800_SetLoopMode(GD5800_Serial* mp3, MP3_LOOP loopMode);      // 设置循环模式
//void  GD5800_SetDevice(GD5800_Serial* mp3, MP3_DEVICE device);         // 设置播放设备
//
///**
// * 状态查询
// */
//MP3_STATUS  GD5800_GetStatus(GD5800_Serial* mp3);      // 获取播放状态
//unsigned char  GD5800_GetVolume(GD5800_Serial* mp3);    // 获取当前音量
//MP3_EQ  GD5800_GetEqualizer(GD5800_Serial* mp3);       // 获取当前音效模式
//MP3_LOOP  GD5800_GetLoopMode(GD5800_Serial* mp3);      // 获取当前循环模式
//unsigned int  GD5800_CountFiles(GD5800_Serial* mp3);   // 获取文件总数
//unsigned int  GD5800_CurrentFileIndexNumber(GD5800_Serial* mp3);  // 获取当前文件索引
//

void send_mp3_command(const char *hex_str);

#ifdef __cplusplus
}
#endif

#endif
