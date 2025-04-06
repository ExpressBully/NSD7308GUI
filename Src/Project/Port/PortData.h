#ifndef PORT_DATA_H
#define PORT_DATA_H

#include <Windows.h>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <mutex>

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

    bool IsOpen() const;

    bool DataRecAnalysis(const std::string& hexData, uint8_t& add, uint8_t& data); // 解析收到的帧格式函数，从接收到的数据中提取 ADD 和 DATA
    void PortData::DataSendAnalysis(char* str, uint8_t Command, uint8_t add, uint8_t data); // 解析发送的帧格式函数，从发送的 ADD 和 DATA，组成下位机的16进制数

protected:
    bool OpenPort();
    bool ClosePort();

private:
    HANDLE mhPort;
    DCB mDcb;
    DataReceivedCallback mDataReceivedCallback;
    ErrorCallback mErrorCallback;
};

#endif // PORT_DATA_H



