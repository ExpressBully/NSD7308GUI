#include "PortData.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "../ImGuiRender/ImGuiWidget/UART_DEC.h"


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

    //// 初始化异步读取结构
    //ZeroMemory(&mReadOverlapped, sizeof(OVERLAPPED));
    //mReadOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //mReadPending = false;

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 5;
    //timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 20;
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

    // 清空串口接收缓冲区
    //PurgeComm(mhPort, PURGE_RXCLEAR);

    DWORD bytesRead = 0;
    if (ReadFile(mhPort, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        data.assign(buffer, bytesRead);

        // 检查 mDataReceivedCallback 是否为空
        if (mDataReceivedCallback)
        {
            mDataReceivedCallback(data);  // 触发回调函数
            //std::cout << "Data received and callback triggered: " << data << std::endl;  // 添加日志
        }
    }
    else
    {
        data.clear();
    }
}

/*
void PortData::ReceiveData(std::string& data) {
    data.clear(); // 首先清空输出参数

    if (mReadPending) {
        // 已有未完成的读取操作，直接返回空数据
        return;
    }

    // 清空缓冲区
    ZeroMemory(mReadBuffer, sizeof(mReadBuffer));

    // 发起异步读取
    DWORD bytesRead = 0;
    //if (!ReadFile(mhPort, mReadBuffer, FrameLen*2, &bytesRead, &mReadOverlapped)) {
        if (1) {
            ReadFile(mhPort, mReadBuffer, FrameLen * 2, &bytesRead, &mReadOverlapped);
         DWORD error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            mReadPending = true;

            // 等待读取完成（带超时）
            DWORD waitResult = WaitForSingleObject(mReadOverlapped.hEvent, 10); // 1秒超时
            if (waitResult == WAIT_OBJECT_0) {
                // 读取完成
                if (GetOverlappedResult(mhPort, &mReadOverlapped, &bytesRead, FALSE)) {
                    if (bytesRead > 0) {
                        data.assign(mReadBuffer, bytesRead); // 成功读取数据
                    }
                }
            }
            // 无论成功与否，都取消未完成的操作
            CancelIo(mhPort);
            mReadPending = false;
        }
        // 其他错误情况直接返回空data
    }
    else {
        // 立即完成的情况
        if (bytesRead > 0) {
            data.assign(mReadBuffer, bytesRead); // 成功读取数据
        }
    }
}*/

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



// 函数：从接收到的数据中提取 SPI1 和 SPI2 内容
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


    //std::cout << "bytes.size=" << bytes.size() << std::endl; //输出

    // 检查数据格式是否正确
    if (bytes.size() >= 6 && bytes[0] == 0x55) {
        add = bytes[4];
        data = bytes[5];
        //std::cout << "add= 0x" << std::hex << static_cast<int>(bytes[4]) << std::endl;  //测试控制台输出add值
        //std::cout << "data= 0x" << std::hex << static_cast<int>(bytes[5]) << std::endl; //测试控制台输出data值
        uint16_t SPIDataDisplay = (add << 8) | data;
        UART_DEC::GetInstance().AddRecSPIDataToDisplay(SPIDataDisplay, AllDataDisplayBuffer);//把SPI数据放到显示区

        return true;
    }



    return false;
}
//函数：从写入的 ADD 和 DATA 组成一个MCU认可的帧
/*0xAA 0xA3 0x00 SPI1 SPI2 0x00 0x00 0x00 0x00 0x00*/
void PortData::DataSendAnalysis(char* str, uint8_t Command, uint8_t add, uint8_t data) {

    // 构建帧格式: 0xAA 0xCommand 0x00 0xadd 0xdata
    uint8_t RWFlag = 0x00;
    switch (Command)
    {case 0xA3:
        RWFlag = 0x00; break;
    case 0xA4:
        RWFlag = 0x40; break;
    default:
        break;
    }
    uint8_t frame[10] = {
        0xAA,       // 帧头
        Command,    // 命令
        0x00,       // 保留位
        add|RWFlag,        // 地址
        data,        // 数据
        0x00,       //必要格式
        0x00,
        0x00,
        0x00,
        0x00
    };
    uint16_t SPIDataDisplay = ((add | RWFlag) << 8) | data;
    UART_DEC::GetInstance().AddSendSPIDataToDisplay(SPIDataDisplay, AllDataDisplayBuffer);//把SPI数据放到显示区

    // 将帧数据复制到输出缓冲区
    memcpy(str, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(str + sizeof(frame), 0, 1024 - sizeof(frame));
}
/*
    @brief :连续读操作发送用函数，MCU帧合成,合成后的字符串用于发送给MCU
    @para str: 合成后的字符串的地址。
    @para AddNum: 要读取的地址数量，最多5个
    @para &Add: 要解析的送出的结果地址。
*/
void PortData::ContinuesReadDataSendAnalysis(char* str, uint8_t Command, uint8_t AddNum, uint8_t(&Add)[10]) //连续读写用的发送函数
{
    // 构建帧格式: 0xAA 0xCommand 0x00 0xadd 0xdata
    uint8_t RWFlag = 0x00;
    switch (Command)
    {
    case 0xD1:  //连续写操作，bit6 set 0
        RWFlag = 0x00; break;
    case 0xD2: //连续读操作，bit6 set 1
        RWFlag = 0x40; break;
    default:
        break;
    }
    uint8_t frame[10] = {
        0xAA,       // 帧头
        Command,    // 命令
        0x00,       // 保留位
        AddNum,        // 要读的地址数
        0x00,        // 数据
        0x00,       //必要格式
        0x00,
        0x00,
        0x00,
        0x00
    };
    for (uint8_t i = 0; i < AddNum; i++)
    {
        frame[i + 4] = Add[i];
    }


    // 将帧数据复制到输出缓冲区
    memcpy(str, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(str + sizeof(frame), 0, 1024 - sizeof(frame));
}


/*
    @brief :连续读操作解析用函数，将接收到的后5位解析成：8位数据
    @para str: 送入要解析的字符串的地址.
    @para DataNum: 输入要解析的数据数量（最大为5）
    @para data: 解析出结果的存放地址。
    @return :是否成功接收到一帧
*/
bool PortData::ContinuesDataRecAnalysis(const std::string& str, uint8_t DataNum, uint8_t(&data)[10]) //输入字符串'str'，输出格式是否正确。并将另外解析出的结果塞入data变量中
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


    //std::cout << "bytes.size=" << bytes.size() << std::endl; //输出


    // 检查数据格式是否正确
    if (bytes.size() == 10 && bytes[0] == 0x55) {
        for (uint8_t i = 0; i < DataNum;i++)
        {
            data[i] = bytes[i + 4]; //进行赋值
        }
        return true;
    }


    return false;
}

