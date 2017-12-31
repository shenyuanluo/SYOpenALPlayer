//
//  SYOpenALPlayer.cpp
//  SYOpenALPlayer
//
//  Created by shenyuanluo on 2017/12/30.
//  Copyright © 2017年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYOpenALPlayer.hpp"


#pragma mark - Private
#pragma mark -- 更新 OpenAL 缓存
void SYOpenALPlayer::refreshQueueBuffer()
{
    ALint processedNum = numOfBuffProcessed();
    while (processedNum--)
    {
        ALuint buffID;
        alSourceUnqueueBuffers(m_sourceID, 1, &buffID);
        alDeleteBuffers(1, &buffID);
    }
}


#pragma mark -- 检出 OpenAL 状态
ALenum SYOpenALPlayer::checkALError()
{
    ALenum errState = AL_NO_ERROR;
    errState = alGetError();
    switch (errState)
    {
        case AL_NO_ERROR:
            std::cout << "AL_NO_ERROR" << std::endl;
            break;
            
        case AL_INVALID_NAME:
            std::cout << "AL_INVALID_NAME : Invalid Name paramater passed to AL call" << std::endl;
            break;
            
        case AL_INVALID_ENUM:
            std::cout << "AL_INVALID_ENUM : Invalid parameter passed to AL call" << std::endl;
            break;
            
        case AL_INVALID_VALUE:
            std::cout << "AL_INVALID_VALUE : Invalid enum parameter value" << std::endl;
            break;
            
        case AL_INVALID_OPERATION:
            std::cout << "AL_INVALID_OPERATION : Illegal call" << std::endl;
            break;
            
        case AL_OUT_OF_MEMORY:
            std::cout << "AL_OUT_OF_MEMORY : No mojo" << std::endl;
            break;
            
        default:
            std::cout << "Unknown error code" << std::endl;
            break;
    }
    return errState;
}


#pragma mark - Public
#pragma mark -- 构造函数
SYOpenALPlayer::SYOpenALPlayer()
{
    m_fillBufLen  = 0;
    m_totalBufLen = BUFF_NUM * BUFF_SIZE;
    m_initBuf     = (ALubyte*)malloc(m_totalBufLen);
}


#pragma mark -- 析构函数
SYOpenALPlayer::~SYOpenALPlayer()
{
    clearOpenAL();
}


#pragma mark -- 初始化 OpenAL
bool SYOpenALPlayer::initOpenAL()
{
    m_device  = alcOpenDevice(NULL);     // 参数为 NULL, 让 ALC 使用默认设备
    m_context = alcCreateContext(m_device, NULL);
    alcMakeContextCurrent(m_context);
    
    alGenSources(1, &m_sourceID);   // 初始化音源 ID
    alGenBuffers(BUFF_NUM, m_buffers);
    
    return true;
}


#pragma mark -- 配置 OpenAL
bool SYOpenALPlayer::configOpenAL(ALuint channels, ALuint bits, ALuint frequency)
{
    ALenum format;
    if (8 == bits)
    {
        if (1 == channels)
        {
            format = AL_FORMAT_MONO8;
        }
        else if (2 == channels)
        {
            format = AL_FORMAT_STEREO8;
        }
        else
        {
            format = 0;
        }
    }
    else if (16 == bits)
    {
        if (1 == channels)
        {
            format = AL_FORMAT_MONO16;
        }
        else if (2 == channels)
        {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            format = 0;
        }
    }
    else
    {
        format = 0;
    }
    if (0 == format)
    {
        std::cout << "Incompatible format : channels = " << channels << "bits = " << bits << std::endl;
        return false;
    }
    m_format    = format;
    m_frequency = frequency;
    return true;
}


#pragma mark -- 设置是否循环播放
void SYOpenALPlayer::setLoop(ALint isLoop)
{
    alSourcei(m_sourceID, AL_LOOPING, isLoop);
}


#pragma mark -- 设置音量大小
void SYOpenALPlayer::setVolume(ALfloat volume)
{
    alSourcef(m_sourceID, AL_GAIN, volume);
}


#pragma mark -- 设置播放速度
void SYOpenALPlayer::setSpeed(ALfloat speed)
{
    alSpeedOfSound(1.0f);
}


#pragma mark -- 获取 OpenAL 已经处理（播放）完毕的缓冲个数
ALint SYOpenALPlayer::numOfBuffProcessed()
{
    ALint bufNum;
    alGetSourcei(m_sourceID, AL_BUFFERS_PROCESSED, &bufNum);
    return bufNum;
}


#pragma mark -- 获取音源状态
SourceState SYOpenALPlayer::sourceState()
{
    ALint val;
    SourceState state;
    alGetSourcei(m_sourceID, AL_SOURCE_STATE, &val);
    switch (val)
    {
        case AL_INITIAL:
            state = SourceInitial;
            break;
            
        case AL_PLAYING:
            state = SourcePlaying;
            break;
            
        case AL_PAUSED:
            state = SourcePaused;
            break;
            
        case AL_STOPPED:
            state = SourceStopped;
            break;
            
        default:
            state = SourceInitial;
            break;
    }
    return state;
}


#pragma mark -- 打开音频缓存进行播放
void SYOpenALPlayer::openAudio(ALubyte* buffer, ALuint length)
{
    if (NULL == buffer || 0 == length)
    {
        std::cout << "Can not open audio !" << std::endl;
        return;
    }
    if (0 == m_fillBufLen
        || m_totalBufLen > m_fillBufLen)   // 先初始化完所有预备缓冲区
    {
        ALint needLen   = m_totalBufLen - m_fillBufLen;
        ALint cpyLen    = length > needLen ? needLen : length;
        ALint remainLen = length - cpyLen;
        
        memcpy(m_initBuf, buffer, cpyLen);
        m_fillBufLen += cpyLen;
        
        if (m_totalBufLen == m_fillBufLen)  // 缓冲已满，可以开始播放
        {
            for (ALint i = 0; i < BUFF_NUM; i++)
            {
                std::cout << "填第 " << i << " 个缓冲区" << std::endl;
                alBufferData(m_buffers[i], m_format, m_initBuf + i * BUFF_SIZE, BUFF_SIZE, m_frequency);
            }
            std::cout << "所有缓冲区入队列 !" << std::endl;
            alSourceQueueBuffers(m_sourceID, BUFF_NUM, m_buffers);
            free(m_initBuf);
            m_initBuf = NULL;
            
            playSound();
        }
        if (0 < remainLen)  // 缓冲满还有剩余
        {
            ALuint bufId;
            alGenBuffers(1, &bufId);
            alBufferData(bufId, m_format, buffer +  cpyLen, remainLen, m_frequency);
            alSourceQueueBuffers(m_sourceID, 1, &bufId);
        }
    }
    else
    {
        refreshQueueBuffer();   // 更新缓存
        
        ALuint loopBufID;
        alGenBuffers(1, &loopBufID);
        alBufferData(loopBufID, m_format, buffer, (ALsizei)length, m_frequency);
        // 新替换缓冲区重新如队列等待 OpenAL 处理
        alSourceQueueBuffers(m_sourceID, 1, &loopBufID);
        
        playSound();
    }
}


#pragma mark -- 开启播放
void SYOpenALPlayer::playSound()
{
    if (SourcePlaying != sourceState())
    {
        alSourcePlay(m_sourceID);
    }
}


#pragma mark -- 暂停播放
void SYOpenALPlayer::pauseSound()
{
    if (SourcePaused != sourceState())
    {
        alSourcePause(m_sourceID);
    }
}


#pragma mark -- 停止播放
void SYOpenALPlayer::stopSound()
{
    if (SourceStopped != sourceState())
    {
        alSourceStop(m_sourceID);
    }
}


#pragma mark -- 清理 OpenAL
void SYOpenALPlayer::clearOpenAL()
{
    alDeleteSources(1, &m_sourceID);
    alDeleteBuffers(BUFF_NUM, m_buffers);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}
