#include "PortData.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

PortData::PortData() : mhPort(INVALID_HANDLE_VALUE) {}

PortData::PortData(HANDLE hPort, DCB& dcb)
    : mhPort(hPort), mDcb(dcb)
{
    if (!OpenPort())
    {
        throw std::runtime_error("Failed to configure port");
    }
}

PortData::~PortData()
{
    ClosePort();
}

void PortData::ResetPort(HANDLE hPort, DCB& dcb)
{
    ClosePort();
    mhPort = hPort;
    mDcb = dcb;
    if (!OpenPort())
    {
        throw std::runtime_error("Failed to reconfigure port");
    }
}

bool PortData::OpenPort()
{
    if (mhPort == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    if (!GetCommState(mhPort, &mDcb))
    {
        return false;
    }

    if (!SetCommState(mhPort, &mDcb))
    {
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutConstant = 50;
    return SetCommTimeouts(mhPort, &timeouts);
}

bool PortData::ClosePort()
{
    if (mhPort != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mhPort);
        mhPort = INVALID_HANDLE_VALUE;
        return true;
    }
    return false;
}

//void PortData::SendData(const std::string& data)
//{
//    SendData(data.c_str(), 5);
//
//    std::cout << "data.c_str (hex) = ";
//    for (size_t i = 0; i < 5; i++) {
//        std::cout << std::hex << std::setw(2) << std::setfill('0')
//            << static_cast<unsigned int>(static_cast<uint8_t>(data.c_str()[i])) << " ";
//    }
//
//    //SendData(data.c_str(), 20);
//}

void PortData::SendData(const char* data, int len)
{
    if (mhPort == INVALID_HANDLE_VALUE) return;

    DWORD bytesWritten;
    if (!WriteFile(mhPort, data, len, &bytesWritten, NULL))
    {
        if (mErrorCallback)
        {
            mErrorCallback("WriteFile failed: " + std::to_string(GetLastError()));
        }
    }
}

void PortData::ReceiveData(std::string& data)
{
    static char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    DWORD bytesRead = 0;
    if (ReadFile(mhPort, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        data.assign(buffer, bytesRead);

        // 检查 mDataReceivedCallback 是否为空
        if (mDataReceivedCallback)
        {
            mDataReceivedCallback(data);  // 触发回调函数
            std::cout << "Data received and callback triggered: " << data << std::endl;  // 添加日志
        }
    }
    else
    {
        data.clear();
    }
}

void PortData::ReceiveData(char* data, int len)
{
    DWORD bytesRead;
    if (!ReadFile(mhPort, data, len, &bytesRead, NULL))
    {
        memset(data, 0, len);
    }
}

void PortData::SetDataReceivedCallback(DataReceivedCallback callback)
{
    mDataReceivedCallback = callback;
    std::cout << "Data received callback set in PortData." << std::endl;  // 添加日志

}

void PortData::SetErrorCallback(ErrorCallback callback)
{
    mErrorCallback = callback;
}

bool PortData::IsOpen() const
{
    return mhPort != INVALID_HANDLE_VALUE;
}

// 函数：从接收到的数据中提取 ADD 和 DATA
bool PortData::DataRecAnalysis(const std::string& str, uint8_t& add, uint8_t& data) //输入字符串'str'，输出格式是否正确。并将另外两个输入add 和 data 填入从帧解析出的值(附带16进制转换)。
{
    std::vector<uint8_t> bytes;
    std::stringstream ss;
    //ss << std::hex << str;
    for (size_t i = 0; i < str.size(); ++i) {
        // 将当前字节转换为两位十六进制数
        ss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(static_cast<unsigned char>(str[i])) << " ";
    }
    uint32_t byte;


    // 将字符串转换为字节流
    while (ss >> byte) {
        bytes.push_back(static_cast<uint8_t>(byte));
    }


    std::cout << "bytes.size=" << bytes.size() << std::endl; //输出

    // 检查数据格式是否正确
    if (bytes.size() >= 6 && bytes[0] == 0x55) {
        add = bytes[4];
        data = bytes[5];
        std::cout << "add= 0x" << std::hex << static_cast<int>(bytes[4]) << std::endl;  //测试控制台输出add值
        std::cout << "data= 0x" << std::hex << static_cast<int>(bytes[5]) << std::endl; //测试控制台输出data值
        return true;
    }

    return false;
}
//函数：从接收到的数据中提取 ADD 和 DATA
void PortData::DataSendAnalysis(char* str, uint8_t Command, uint8_t add, uint8_t data) {
    // 构建帧格式: 0xAA 0xCommand 0x00 0xadd 0xdata
    uint8_t frame[5] = {
        0xAA,       // 帧头
        Command,    // 命令
        0x00,       // 保留位
        add,        // 地址
        data        // 数据
    };

    // 将帧数据复制到输出缓冲区
    memcpy(str, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(str + sizeof(frame), 0, 1024 - sizeof(frame));
}