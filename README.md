# SYOpenALPlayer

## WAVE 文件

> `WAVE` 文件格式是微软用来存储多媒体文件的一个子集；遵循 [RIFF资源交换档案格式](https://zh.wikipedia.org/zh-cn/%E8%B5%84%E6%BA%90%E4%BA%A4%E6%8D%A2%E6%A1%A3%E6%A1%88%E6%A0%BC%E5%BC%8F)。
> 
> 一个 `RIFF` 文件从一个文件头开始，然后是一个数据块的序列。WAVE文件通常只是一个重复的文件，其中包含一个**WAVE**块，它由两个子块组成——一个**fmt**块指定数据格式和一个包含实际示例数据的**data**块。

### 标准的 WAVE 文件格式（图片来源于[这里](http://soundfile.sapp.org/doc/WaveFormat/)）
![](SYWavParser/wav_sound_format.gif)

## OpenAL
> `OpenAL`（Open Audio Library）是自由软件界的跨平台音效API。它设计给多通道三维位置音效的特效表现。其API风格模仿自 [OpenGL](https://zh.wikipedia.org/wiki/OpenGL)。

### API 结构和功能
1. `OpenAL` 主要的功能是在**来源物体**、**音效缓冲**和**收听者**中编码。
	- 来源物体：包含一个指向缓冲器的指针、声音的速度、位置和方向，以及声音强度。
	- 音效缓冲：包含8或16位、单声道或立体声 `PCM` 格式的音效数据，表现引擎进行所有必要的计算，如距离衰减、多普勒效应等。
	- 收听者：包含收听者的速度、位置和方向，以及全部声音的整体增益。
2. 不同于OpenGL规格，OpenAL规格包含两个API分支：
	- 以实际 `OpenAL` 函数组成的核心。
	- ALC：用于管理表现内容、资源使用情况，并将跨平台风格封在其中。还有**ALUT**程序库，提供高级“易用”的函数，其定位相当于OpenGL的GLUT。

### 实现的基本步骤：
1. 得到设备信息
2. 将环境与设备关联
3. 在缓存中加入声音数据
4. 在声源中加入缓存数据
5. 播放声源

## 参考资料
- [OpenAL](https://zh.wikipedia.org/wiki/OpenAL)
- [http://soundfile.sapp.org/doc/WaveFormat/](http://soundfile.sapp.org/doc/WaveFormat/)
- [Sound Examples](http://www.music.helsinki.fi/tmt/opetus/uusmedia/esim/index-e.html)
