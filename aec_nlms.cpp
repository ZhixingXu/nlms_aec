#include <iostream>
#include <vector>
#include <cfloat>
#include <cmath>
#include <string.h>
#include <list>

#include "wav.h"
using namespace std;
template<typename T1,typename T2>
float conv(T1*buf1,int len1,T2*buf2,int len2){
    float ans=0;
    for (int i = 0; i < len1; i++)
    {
        // ans+=buf1[i]*
        if(len1-len2>0 && i<len1-len2)
            continue;
        ans+=buf1[i]*buf2[len1-i-1];
    }
    return ans;
}
template<typename T1,typename T2>
void special_print(T1*buf1,int len1,T2*buf2,int len2,float ans){
    cout<<'[';
    for (int i = 0; i < len1; i++)
    {
        cout<<buf1[i];
        char ch=i==len1-1?']':',';
        cout<<ch;
    }
    cout<<',';
    cout<<'[';
    for (int i = 0; i < len2; i++)
    {
        cout<<buf2[i];
        char ch=i==len2-1?']':',';
        cout<<ch;
    }
    cout<<"="<<ans;
    cout<<endl;
}
float updata_miu(short*buf,int len){
    float energy=0.0001;
    for (int i = 0; i < len; i++)
    {
        energy+=buf[i]*buf[i];
    }
    return 0.04/energy;
}

int main(int argc, char const *argv[])
{
    if(argc!=5){
        printf("parameter err\n");
        printf("e.g. ./nlms ./wave/far_end.wav ./wave/echo.wav ./wave/mic_with_echo.wav ./wave/mic_remove_echo.wav\n");
        return 0;
    }
    wav_reader sig(argv[1]);
    wav_reader echo(argv[2]);
    wav_reader sig_with_echo(argv[3]);
   
    int filter_order=16;
    short*buf1=new short[filter_order];
    short*buf2=new short[filter_order];
    short*buf3=new short[filter_order];
    float miu;

    // for (int i = 0; i < filter_order; i++)
    // {
    //     short tmp;
    //     echo.get_data((char*)&tmp,NULL,2);
    //     sig_with_echo.get_data((char*)&tmp,NULL,2);
    // }
    
    
    // 1. 构造自适应滤波器
    float*filter=new float[filter_order];
    for (int i = 0; i < filter_order; i++)
    {
        filter[i]=0;
    }
    
    list<short>buffer;
    while (1)
    {
        int cnt1;
        if(buffer.size()<filter_order){
            cnt1=sig.get_data((char*)buf1,NULL,filter_order*2);
            for (int i = 0; i < cnt1/2; i++)
            {
                buffer.push_back(buf1[i]);
            }
            if(0==cnt1)
                break;
        }
        
        int cnt2=echo.get_data((char*)buf2,NULL,2);
        if(cnt2==0)
            break;
        auto tmp=buffer.cbegin();
        for (int i = 0; i < filter_order; i++)
        {
            buf1[i]=*tmp;
            tmp++;
        }
      
        float estimate_echo=conv(buf1,filter_order,filter,filter_order);///65536;
        // special_print(buf1,filter_order,filter,filter_order,estimate_echo);
        float error=buf2[0]-estimate_echo;
        // 根据信号能量更新步长因子
        miu=updata_miu(buf1,filter_order);
        // 更新自适应滤波器
        for (int i = 0; i < filter_order; i++)
        {
            filter[i]+=miu*error*buf1[filter_order-1-i];
        }
        buffer.pop_front();
    }
    
    for (int i = 0; i < filter_order; i++)
    {
        printf("%f,",filter[i]);
    }
    printf("\n");

    // 2.去除回声
    sig.~wav_reader();
    wav_reader ref(argv[1]);
    wav_writer out(argv[4],
                    ref.get_channels(),
                    ref.get_sample_rate(),
                    ref.get_sample_width());
    buffer.clear();
    while (1)
    {
        int cnt1;
        if(buffer.size()<filter_order){
            cnt1=ref.get_data((char*)buf1,NULL,filter_order*2);
            for (int i = 0; i < cnt1/2; i++)
            {
                buffer.push_back(buf1[i]);
                // printf("%d->",buf1[i]);
            }
            // printf("\n");
            if(0==cnt1)
                break;
        }
        
        int cnt2=sig_with_echo.get_data((char*)buf2,NULL,2);
        if(cnt2==0)
            break;
        auto tmp=buffer.cbegin();
        for (int i = 0; i < filter_order; i++)
        {
            buf1[i]=*tmp;
            // printf("%d--",buf1[i]);
            tmp++;
        }
        float estimate_echo=conv(buf1,filter_order,filter,filter_order);
        short ans=buf2[0]-estimate_echo;
        
        out.write_data((char*)&ans,NULL,2);
        
        buffer.pop_front();
    }

    delete buf1;
    delete buf2;
    delete buf3;
    return 0;
}
