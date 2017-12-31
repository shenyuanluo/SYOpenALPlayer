//
//  SYOpenALPlayer.hpp
//  SYOpenALPlayer
//
//  Created by shenyuanluo on 2017/12/30.
//  Copyright © 2017年 http://blog.shenyuanluo.com/ All rights reserved.
//


/* OpenAL 实现的基本步骤：
 1. 得到设备信息
 2. 将环境与设备关联
 3. 在缓存中加入声音数据
 4. 在声源中加入缓存数据
 5. 播放声源
 */


#ifndef SYOpenALPlayer_hpp
#define SYOpenALPlayer_hpp

#include <iostream>
#include <string>
#include <OpenAL/OpenAL.h>


#define BUFF_NUM  3
#define BUFF_SIZE 4096


typedef enum {
    SourceInitial               = 0x00,     // 初始
    SourcePlaying               = 0x01,     // 播放
    SourcePaused                = 0x02,     // 暂停
    SourceStopped               = 0x03,     // 停止
}SourceState;


class SYOpenALPlayer
{
    
private:
    ALCdevice*      m_device;               // 硬件，获取设备音频硬件资源
    ALCcontext*     m_context;              // 内容，给播放器提供上下文环境描述
    ALuint          m_sourceID;             // 音源，标识每一个音源
    ALuint          m_buffers[BUFF_NUM];    // 缓存
    ALenum          m_format;               // 格式
    ALsizei         m_frequency;            // 采样频率
    ALubyte*        m_initBuf;              // 缓冲区
    ALint           m_fillBufLen;           // 初始化缓冲区长度
    ALint           m_totalBufLen;          // 缓冲区总长度
    
    
    /**
     更新 OpenAL 缓存
     */
    void refreshQueueBuffer();
    
    /**
     检出 OpenAL 状态

     @return OpenAL 状态值
     */
    ALenum checkALError();
    
    
public:
    SYOpenALPlayer();
    
    ~SYOpenALPlayer();
    
    /**
     初始化 OpenAL

     @return 是否初始化成功（true：成功； false：失败）
     */
    bool initOpenAL();
    
    /**
     配置 OpenAL

     @param channels 声道数
     @param bits 位深
     @param frequency 采样频率
     @return 是否配置成功（true：成功； false：失败）
     */
    bool configOpenAL(ALuint channels, ALuint bits, ALuint frequency);
    
    /**
     设置是否循环播放

     @param isLoop 是否循环（1：循环；0：不循环）
     */
    void setLoop(ALint isLoop);
    
    /**
     设置音量大小

     @param volume 音量值（0.0 —— 1.0）
     */
    void setVolume(ALfloat volume);
    
    /**
     设置播放速度

     @param speed 播放速度（1.0：正常速度）
     */
    void setSpeed(ALfloat speed);
    
    /**
     获取 OpenAL 已经处理（播放）完毕的缓冲个数

     @return 缓冲个数
     */
    ALint numOfBuffProcessed();
    
    /**
     获取音源状态

     @return 音源状态值，参见‘SourceState’
     */
    SourceState sourceState();
    
    /**
     打开音频缓存进行播放

     @param buffer 需要播放的音频缓存数据
     @param length 缓存长度
     */
    void openAudio(ALubyte* buffer, ALuint length);
    
    /**
     开启播放
     */
    void playSound();
    
    /**
     暂停播放
     */
    void pauseSound();
    
    /**
     停止播放
     */
    void stopSound();
    
    /**
     清理 OpenAL
     */
    void clearOpenAL();
};

#endif /* SYOpenALPlayer_hpp */
