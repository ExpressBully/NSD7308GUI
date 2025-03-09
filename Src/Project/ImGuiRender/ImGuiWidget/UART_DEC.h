#ifndef  UARTDEC_H
#define  UARTDEC_H

#include "WidgetBase.h"
#include <vector>
#include <string>
#include <mutex>

// 全局变量声明
extern std::vector<std::string> usb_ports; // 存储检测到的串口
extern int selected_usb_index;             // 当前选中的串口索引
extern bool ConnectFlag;                   // 连接状态标志
extern bool is_scanning;                   // 是否正在扫描串口
extern std::mutex usb_mutex;               // 用于线程安全的互斥锁



class UART_DEC : public WidgetBase
{
public:
    UART_DEC();
    virtual ~UART_DEC();

    virtual void Render() override;
};


#endif // ! UARTDEC_H