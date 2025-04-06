#ifndef UARTDEC_H
#define UARTDEC_H

#include "WidgetBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <cstdint>

class UART_DEC : public WidgetBase
{
public:
    UART_DEC();
    virtual ~UART_DEC();

    virtual void Render() override;

private:
    void RefreshPortList();  // 刷新串口列表
    void OpenPort();         // 打开串口
    void ClosePort();        // 关闭串口
    void SendData();         // 发送数据
    void ReceiveData();      // 接收数据

    void OnDataReceived(const std::string& data);  // 数据接收回调函数

    std::vector<std::string> mPortList;  // 串口列表
    std::vector<std::string> mSendHistory; // 发送数据历史记录
    std::string mSelectedPort;           // 当前选中的串口
    char mSendBuffer[1024];              // 发送数据缓冲区
    std::string mReceiveBuffer;          // 接收数据缓冲区
    std::string mReceiveBufferDis;       // 接收数据缓冲区(显示用)
    char DataSended[1024];              // 发送数据显示存储区
    bool mIsPortOpen;                    // 串口是否打开
    std::mutex mMutex;                   // 用于线程安全的互斥锁
    uint8_t ConvertHexStringToUint8(const std::string& hexStr);

    std::string ToHexString(const std::string& data);
    std::vector<uint8_t> HexStringToBytes(const std::string& hexString);
    std::string BytesToHexString(const std::vector<uint8_t>& bytes);
};

#endif // !UARTDEC_H