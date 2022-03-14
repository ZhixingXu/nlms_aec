#include <iostream>
#include <vector>
#include <string.h>
using namespace std;
#define LOGV(tag,vector) \
    do \
    {\
        cout<<tag<<":::";\
        print_vector(vector);\
    } while(0)


#define WavHanderSize  46

static char wav_headerM[WavHanderSize];
static char wav_headerR[WavHanderSize];
template <typename T>
vector<T> operator+(const vector<T> &a, const vector<T> &b)
{
    if (a.size() != b.size())
    {
        cout<<"+error"<<endl;
        exit(-1);
    }
    
    vector<T> re;
    for (uint i = 0; i < a.size(); i++)
    {
        re.push_back(a[i] + b[i]);
    }
    return re;
}
template <typename T>
vector<T> operator+(const vector<T> &a, float n)
{
    vector<T> re;
    for (uint i = 0; i < a.size(); i++)
    {
        re.push_back(a[i] + n);
    }
    return re;
}
template <typename T>
vector<T> operator*(const vector<T> &a, const vector<T> &b)
{
    if (a.size() != b.size())
    {
        cout<<"*error"<<endl;
        exit(-1);
    }
    
    vector<T> re;
    for (uint i = 0; i < a.size(); i++)
    {
        re.push_back(a[i] * b[i]);
    }
    return re;
}
template <typename T>
vector<T> operator*(const float n, const vector<T> &b)
{
    vector<T> re;
    for (uint i = 0; i < b.size(); i++)
    {
        re.push_back(n * b[i]);
    }
    return re;
}
template <typename T>
vector<T> operator-(const vector<T> &a, const vector<T> &b)
{
    if (a.size() != b.size())
    {
        cout<<"error"<<endl;
        exit(-1);
    }
    int min=a.size() > b.size()?b.size():a.size();
    vector<T> re;
    for (uint i = 0; i < min; i++)
    {
        re.push_back(a[i] - b[i]);
    }
    return re;
}
template <typename T>
vector<T> conv_cycle(const vector<T>&num1,const vector<T>&num2){
	int n = num1.size()+num2.size()-1;
    vector<T>ans(n,0);
    // cout<<ans.size()<<endl;
    for (int i = num2.size()-1; i >= 0; i--)
    {
        int start=ans.size()-num1.size()-num2.size()+1+i;//num2.size()-i;
        // cout<<"start:"<<start<<endl;
        for (int j = start; j < start + num1.size(); j++)
        {
            ans[j]+=num2[i]*num1[j-start];
        }
        
    }
    int len=num1.size();
    int offset=ans.size()-len;
    for (int i = ans.size()-1; i >= len; i--)
    {
        // ans[i-len]+=ans[i];
        ans.pop_back();
    }
    
    //shuai
	return ans;
}
template <typename T>
vector<T> conv(const vector<T>&num1,const vector<T>&num2){
	int n = num1.size()+num2.size()-1;
    vector<T>ans(n,0);
    // cout<<ans.size()<<endl;
    for (int i = num2.size()-1; i >= 0; i--)
    {
        int start=ans.size()-num1.size()-num2.size()+1+i;//num2.size()-i;
        // cout<<"start:"<<start<<endl;
        for (int j = start; j < start + num1.size(); j++)
        {
            ans[j]+=num2[i]*num1[j-start];
        }
        
    }
    while(ans.size()>num1.size()){
        ans.pop_back();
    }
	return ans;
}
template <typename T>
T sum(vector<T>num){
    T sum=0;
    for (int i = 0; i < num.size(); i++)
    {
        sum+=num[i];
    }
    return sum;
}
template <typename T>
vector<T>& inverse_vector(vector<T>&num){
    for (int i = 0; i < num.size()/2; i++)
    {
        T tmp=num[i];
        num[i]=num[num.size()-1-i];
        num[num.size()-1-i]=tmp;
    }
    return num;
}
template <typename T>
void print_vector(vector<T>&num){
    for (int i = 0; i < num.size(); i++)
    {
        cout<<num[i]<<",";
    }
    cout<<endl;
    
}
//little end to big end
uint16_t little2big(uint16_t num){
    return num;//((num&0xff)<<8)|((num&0xff00)>>8);
}
uint16_t big2little(uint16_t num){
    return num;//((num&0xff)<<8)|((num&0xff00)>>8);
}

int main(int argc, char const *argv[])
{
    if(argc!=5){
        cout<<"para is not enough:./aec_v1 ./data/mic.wav ./data/ref.wav ./data/shuai.wav"<<endl;
        return 0;
    }


    FILE *fp_sound1 = fopen(argv[1], "rb");//mic.wav
    FILE *fp_sound2 = fopen(argv[2], "rb");//ref.wav
    FILE *fp_mic = fopen(argv[3], "wb");//output
    FILE *fp_ref = fopen(argv[4], "wb");
    if(!fp_mic||!fp_sound2||!fp_mic){
        return 0;
    }
    // 读取wav文件头信息
	fread(wav_headerM, WavHanderSize, 1, fp_sound1);
	fread(wav_headerR, WavHanderSize, 1, fp_sound2);
	fwrite(wav_headerM, WavHanderSize, 1, fp_mic);
    fwrite(wav_headerM, WavHanderSize, 1, fp_ref);

    #define SAMPLE_POINT 8*2
    #define FILTER_ORDER SAMPLE_POINT

    uint16_t buf[SAMPLE_POINT];
    uint16_t buf2[SAMPLE_POINT];

    memset(buf,0,sizeof(buf));
    memset(buf2,0,sizeof(buf2));

    vector<float>echo_path_filter={0.        , 0.04322727, 0.1654347 , 0.3454915 , 0.55226423,
       0.75      , 0.9045085 , 0.9890738 , 0.9890738 , 0.9045085 ,
       0.75      , 0.55226423, 0.3454915 , 0.1654347 , 0.04322727,
       0};
    print_vector(echo_path_filter);
    //
    static int ttt=0;
    
    
    // echo_path_filter=inverse_vector(echo_path_filter);
    while (fread(buf,sizeof(uint16_t),FILTER_ORDER,fp_sound1)==FILTER_ORDER)
    {
        vector<float>input;//(buf,buf+FILTER_ORDER);
        for (int i = 0; i < FILTER_ORDER; i++)
        {
            input.push_back(float(little2big(buf[i]))/65536);
        }
        //
        if(fread(buf2,sizeof(uint16_t),FILTER_ORDER,fp_sound2)<1)
            break;
        
        vector<float>farend;//(buf2,buf2+FILTER_ORDER);
        for (int i = 0; i < FILTER_ORDER; i++)
        {
            farend.push_back(float(little2big(buf2[i]))/65536);
        }

        vector<float>echo_path=0.73*farend;//conv_cycle(farend, echo_path_filter);//
        vector<float>out = input + echo_path;//

        uint16_t dat[FILTER_ORDER];
        for (int i = 0; i < FILTER_ORDER; i++)
        {
            dat[i]=big2little(uint16_t(out[i]*65536));
        }
        fwrite(dat,sizeof(uint16_t),FILTER_ORDER,fp_mic);

        for (int i = 0; i < FILTER_ORDER; i++)
        {
            dat[i]=big2little(uint16_t(echo_path[i]*65536));
        }
        fwrite(dat,sizeof(uint16_t),FILTER_ORDER,fp_ref);

    }
    fclose(fp_sound1 );
    fclose(fp_sound2 );
    fclose(fp_mic );
    fclose(fp_ref);
    return 0;
}
