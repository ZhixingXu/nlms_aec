#ifndef WAV_READER_H
#define WAV_READER_H

#include <stdio.h>
/* RIFF WAVE file struct. 
 * For details see WAVE file format documentation  
 * (for example at <a href="http://www.wotsit.org)." target="_blank">http://www.wotsit.org).</a>  */  
typedef struct WAV_HEADER_S  
{  
    char            riffType[4];    //4byte,资源交换文件标志:RIFF     
    unsigned int    riffSize;       //4byte,从下个地址到文件结尾的总字节数   
    char            waveType[4];    //4byte,wav文件标志:WAVE      
    char            formatType[4];  //4byte,波形文件标志:FMT(最后一位空格符)   
    unsigned int    formatSize;     //4byte,音频属性(compressionCode,numChannels,sampleRate,bytesPerSecond,blockAlign,bitsPerSample)所占字节数  
    unsigned short  compressionCode;//2byte,格式种类(1-线性pcm-WAVE_FORMAT_PCM,WAVEFORMAT_ADPCM)  
    unsigned short  numChannels;    //2byte,通道数  
    unsigned int    sampleRate;     //4byte,采样率  
    unsigned int    bytesPerSecond; //4byte,传输速率  
    unsigned short  blockAlign;     //2byte,数据块的对齐，即DATA数据块长度  
    unsigned short  bitsPerSample;  //2byte,采样精度-PCM位宽  
    char            dataType[4];    //4byte,数据标志:data  
    unsigned int    dataSize;       //4byte,从下个地址到文件结尾的总字节数，即除了wav header以外的pcm data length  
}WAV_HEADER_t; 

class wav_base
{
public:
    unsigned int mem_cnt;
    FILE*fp;
    WAV_HEADER_t header;
};

class wav_reader:private wav_base
{
public:
    wav_reader(const char* fn);
    ~wav_reader();
    int get_channels();
    int get_sample_rate();
    int get_sample_width();
    int get_frame_cnt();
    int get_data(char*lbuf,char*rbuf,int buf_len);
};

class wav_writer:private wav_base
{
public:
    wav_writer(const char* fn);
    wav_writer(const char* fn,int nchannel,int fs,int wid);
    ~wav_writer();
    int set_channels(int n);
    int set_sample_rate(int fs);
    // int set_frame_cnt(int);
    int set_sample_width(int wid);
    int write_data(char*lbuf,char*right,int len);
};





#endif