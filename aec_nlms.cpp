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
        ans[i-len]+=ans[i];
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
    if(argc!=4){
        cout<<"para is not enough:./aec_v1 ./data/mic.wav ./data/ref.wav ./data/shuai.wav"<<endl;
        return 0;
    }
 
    FILE *fp_mic = fopen(argv[1], "rb");//mic.wav
    FILE *fp_ref = fopen(argv[2], "rb");//ref.wav
    FILE *fp_aec = fopen(argv[3], "wb");//output
    if(!fp_aec||!fp_ref||!fp_aec){
        return 0;
    }
    // 读取wav文件头信息
	fread(wav_headerM, WavHanderSize, 1, fp_mic);
	fread(wav_headerR, WavHanderSize, 1, fp_ref);
	fwrite(wav_headerM, WavHanderSize, 1, fp_aec);

    #define SAMPLE_POINT 8*4
    int filter_order=SAMPLE_POINT;
    float miu=0;
    uint16_t buf[SAMPLE_POINT];
    uint16_t buf2[SAMPLE_POINT];

    

    memset(buf,0,sizeof(buf));
    memset(buf2,0,sizeof(buf2));

    vector<float>auto_adapt_filter(filter_order,float(0));
    print_vector(auto_adapt_filter);
    //
    static int ttt=0;
    

    while (fread(buf,sizeof(uint16_t),filter_order,fp_ref)==filter_order)
    {
        fseek(fp_ref,(1-filter_order)*sizeof(uint16_t),SEEK_CUR);

        vector<float>input;//(buf,buf+filter_order);
        for (int i = 0; i < filter_order; i++)
        {
            input.push_back(float(little2big(buf[i]))/65536);
        }
 
        vector<float>capture=input;
        float y=sum(input*auto_adapt_filter);//conv_cycle(input,auto_adapt_filter).back();//
        float en=capture.back()-y;//conv_cycle(input,auto_adapt_filter);//capture-auto_adapt_filter*input;
        {
            if(1000==ttt)
                break;
            ttt++;
        }
        printf("%f,",en*en);
        miu=1/(sum(capture*capture)+0.001);
        auto_adapt_filter=auto_adapt_filter+miu*en*input;

        memset(buf,0,sizeof(buf));
    }
    cout<<"ttt:"<<ttt<<endl;
    fseek(fp_ref,WavHanderSize,SEEK_SET);
    LOGV("auto_adapt_filter",auto_adapt_filter);
    ttt=0;

    cout<<endl;
    // auto_adapt_filter=inverse_vector(auto_adapt_filter);
    while (fread(buf,sizeof(uint16_t),filter_order,fp_mic)==filter_order)
    {
        vector<float>input;//(buf,buf+filter_order);
        for (int i = 0; i < filter_order; i++)
        {
            input.push_back(float(little2big(buf[i]))/65536);
        }
        //
        fread(buf2,sizeof(uint16_t),filter_order,fp_ref);
        vector<float>farend;//(buf2,buf2+filter_order);
        for (int i = 0; i < filter_order; i++)
        {
            farend.push_back(float(little2big(buf2[i]))/65536);
        }
        vector<float>out=input-conv_cycle(farend,auto_adapt_filter);//farend*auto_adapt_filter;//

        uint16_t dat[filter_order];
        for (int i = 0; i < filter_order; i++)
        {
            dat[i]=big2little(uint16_t(out[i]*65536));
        }
        fwrite(dat,sizeof(uint16_t),filter_order,fp_aec);

    }
    fclose(fp_mic );
    fclose(fp_ref );
    fclose(fp_aec );
    return 0;
}
