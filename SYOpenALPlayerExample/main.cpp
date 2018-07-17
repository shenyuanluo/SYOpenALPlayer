//
//  main.cpp
//  SYOpenALPlayer
//
//  Created by shenyuanluo on 2017/12/30.
//  Copyright © 2017年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <iostream>
#include "SYWavHeader.h"
#include "SYOpenALPlayer.h"


#pragma mark -- 测试 OpenAL
void testOpenAL(const char* filePath)
{
    FILE *pfile;
    pfile = fopen(filePath, "rb");
    if (NULL == pfile)
    {
        std::cout << "Open audio file failed !" << std::endl;
        return;
    }
    
    SYOpenALPlayer alPlayer;
    
    WAVHead* wavHead;
    wavHead = (WAVHead*)malloc(sizeof(WAVHead));
    fread(wavHead, 1, sizeof(WAVHead), pfile);
    
    bool isSuccess = alPlayer.ConfigOpenAL(wavHead->fmt.numChannels,
                                           wavHead->fmt.bitsPerSample,
                                           wavHead->fmt.sampleRate);
    if (false == isSuccess)
    {
        std::cout << "Config Open AL failed !" << std::endl;
        return;
    }
    
    if ('f' != wavHead->fmt.fmtChunkID[0]
        || 'm' != wavHead->fmt.fmtChunkID[1]
        || 't' != wavHead->fmt.fmtChunkID[2]
        || ' ' != wavHead->fmt.fmtChunkID[3])
    {
        std::cout << "Open audio file is not 'fmt ' !" << std::endl;
        free(wavHead);
        fclose(pfile);
        return;
    }
    if (1 != wavHead->fmt.audioFormat)
    {
        std::cout << "Not PCM !" << std::endl;
        free(wavHead);
        fclose(pfile);
        return;
    }
    
    // 循环读取 Audio 文件数据
    unsigned char* buff = (unsigned char*)malloc(BUFF_SIZE * BUFF_NUM);;
    size_t ret;
    ret = fread(buff, 1, BUFF_SIZE * BUFF_NUM, pfile);
    
    alPlayer.OpenAudio(buff, (int)ret);
    
    alPlayer.PlaySound();
    
    while (!feof(pfile))
    {
        int val = alPlayer.NumOfBuffProcessed();
        if (0 >= val) // 一个 buf 都还没处理完，持续等待播放
        {
            continue;
        }
        while (val--)
        {
            // 读取下一缓存区数据
            ret = fread(buff, 1, BUFF_SIZE, pfile);
            
            alPlayer.OpenAudio(buff, (int)ret);
        }
    }
    std::cout << "文件读取完毕" << std::endl;
    // 文件读取完毕
    fclose(pfile);
    free(buff);
    free(wavHead);
    
    ALSourceState state;
    do      // 等待 OpenAL 播放完毕
    {
        state = alPlayer.SourceState();
    } while (alSource_playing == state);
}


int main(int argc, const char * argv[])
{
    testOpenAL("Resources/TestAudio.wav");
    
    return 0;
}
