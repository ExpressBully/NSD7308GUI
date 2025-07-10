#ifndef PORT_DATA_H
#define PORT_DATA_H

#include <Windows.h>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <mutex>
#define FrameLen 10 //帧格式的长度，10个字节

extern bool mIsPortOpen;                    // 串口是否打开
extern bool mIsTestMode;                     //是否是测试模式，决定渲染的页面。

class PortData
{
public:
    using DataReceivedCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    PortData();
    PortData(HANDLE hPort, DCB& dcb);
    ~PortData();

    void ResetPort(HANDLE hPort, DCB& dcb);

    //void SendData(const std::string& data);
    void SendData(const char* data, int len);
    void ReceiveData(std::string& data);
    void ReceiveData(char* data, int len);

    void SetDataReceivedCallback(DataReceivedCallback callback);
    void SetErrorCallback(ErrorCallback callback);

    bool IsOpen() const;//

    bool DataRecAnalysis(const std::string& hexData, uint8_t& add, uint8_t& data); // 解析收到的帧格式函数，从接收到的数据中提取 ADD 和 DATA
    void DataSendAnalysis(char* str, uint8_t Command, uint8_t add, uint8_t data); // 单个字节读写，解析发送的帧格式函数，从发送的 ADD 和 DATA，组成下位机的16进制数

    bool ContinuesDataRecAnalysis(const std::string& hexData, uint8_t DataNum, uint8_t(&data)[10]); // 连续接收指令用的解析函数
    void ContinuesReadDataSendAnalysis(char* str, uint8_t Command, uint8_t AddNum, uint8_t(&Add)[10]); //连续读用的MCU帧合成函数



protected:
    bool OpenPort();
    bool ClosePort();

private:
    HANDLE mhPort;
    DCB mDcb;
    DataReceivedCallback mDataReceivedCallback;
    ErrorCallback mErrorCallback;

    //char mReadBuffer[1024];          // 异步读取缓冲区
    //bool mReadPending;               // 标记是否有未完成的异步读取操作
    //OVERLAPPED mReadOverlapped;      // 异步读取操作结构

    };

#endif // PORT_DATA_H



