//
//  main.cpp
//  SYOpenALPlayer
//
//  Created by shenyuanluo on 2017/12/30.
//  Copyright © 2017年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <iostream>
#include "SYWavParser.hpp"
#include "SYOpenALPlayer.hpp"


void playWavOnPath(const char* filePath);


int main(int argc, const char * argv[])
{
    playWavOnPath("Source/TestAudio.wav");
    
    return 0;
}


void playWavOnPath(const char* filePath)
{
    FILE *pfile;
    unsigned char* buff;
    pfile = fopen(filePath, "rb");
    if (NULL == pfile)
    {
        std::cout << "Open audio file failed !" << std::endl;
        return;
    }
    
    SYOpenALPlayer alPlayer;
    alPlayer.initOpenAL();

    WAVHead* wavHead;
    buff    = (unsigned char*)malloc(4069 * 3);
    wavHead = (WAVHead*)malloc(sizeof(WAVHead));
    fread(wavHead, 1, sizeof(WAVHead), pfile);
    
    bool isSuccess = alPlayer.configOpenAL(wavHead->fmt.numChannels,
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
        free(buff);
        free(wavHead);
        fclose(pfile);
        return;
    }
    if (1 != wavHead->fmt.audioFormat)
    {
        std::cout << "Not PCM !" << std::endl;
        free(buff);
        free(wavHead);
        fclose(pfile);
        return;
    }

    // 循环读取 Audio 文件数据
    size_t ret;
    ret = fread(buff, 1, BUFF_SIZE * BUFF_NUM, pfile);
    
    alPlayer.openAudio(buff, (int)ret);
    
    alPlayer.playSound();
    
    while (!feof(pfile))
    {
        int val = alPlayer.numOfBuffProcessed();
        if (0 >= val) // 一个 buf 都还没处理完，持续等待播放
        {
            continue;
        }
        while (val--)
        {
            // 读取下一缓存区数据
            ret = fread(buff, 1, BUFF_SIZE, pfile);
            
            alPlayer.openAudio(buff, (int)ret);
        }
    }
    std::cout << "文件读取完毕" << std::endl;
    // 文件读取完毕
    fclose(pfile);
    free(buff);
    free(wavHead);

    SourceState state;
    do      // 等待 OpenAL 播放完毕
    {
        state = alPlayer.sourceState();
    } while (SourcePlaying == state);
}
