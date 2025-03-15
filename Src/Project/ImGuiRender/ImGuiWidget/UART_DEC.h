#ifndef UARTDEC_H
#define UARTDEC_H

#include "WidgetBase.h"
#include <vector>
#include <string>
#include <mutex>

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

    std::vector<std::string> mPortList;  // 串口列表
    std::string mSelectedPort;           // 当前选中的串口
    char mSendBuffer[1024];              // 发送数据缓冲区
    std::string mReceiveBuffer;          // 接收数据缓冲区
    bool mIsPortOpen;                    // 串口是否打开
    std::mutex mMutex;                   // 用于线程安全的互斥锁
};

#endif // !UARTDEC_H