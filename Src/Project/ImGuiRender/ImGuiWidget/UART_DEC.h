#ifndef UARTDEC_H
#define UARTDEC_H

#include "WidgetBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include "RegData.h"
#include "../../Port/PortData.h"
#include <fstream>
#include <algorithm>
//#include "ImGuiFileDialog.h" // 需提前集成 ImGuiFileDialog


extern uint16_t SPISend_Display;
extern uint16_t SPIRec_Display;
extern std::string AllDataDisplayBuffer; //所有数据存储用的

class UART_DEC : public WidgetBase
{
public:
    UART_DEC();
    virtual ~UART_DEC();

    virtual void Render() override;
    void SetCallback(const std::function<void(bool)>& callback)
    {
        mCallBack = callback;
    }
private:
    bool isSend = false; //发送按钮是否按下
    bool isRead = false; //读取按钮是否按下


    void RefreshPortList();  // 刷新串口列表
    void OpenPort();         // 打开串口
    void ClosePort();        // 关闭串口

    void OnDataReceived(const std::string& data);  // 数据接收回调函数

    std::vector<std::string> mPortList;  // 串口列表
    std::vector<std::string> mSendHistory; // 发送数据历史记录
    std::string mSelectedPort;           // 当前选中的串口
    char UARTSendBuffer[1024];              // 发送数据缓冲区
    std::string UARTReceiveBuffer;          // 接收数据缓冲区
    std::string mReceiveBufferDis;       // 接收数据缓冲区(显示用)
    char DataSended[1024];              // 发送数据显示存储区
    bool mIsReading = false;                    //是否点击Read准备好接收数据
    std::mutex mMutex;                   // 用于线程安全的互斥锁
    uint8_t ConvertHexStringToUint8(const std::string& hexStr);

    std::string ToHexString(const std::string& data);
    std::vector<uint8_t> HexStringToBytes(const std::string& hexString);
    std::string BytesToHexString(const std::vector<uint8_t>& bytes);

    std::function<void(bool)> mCallBack; // 值类型

    void SaveDataToCSV(const std::string& dataBuffer);//把AllDataDisplayBuffer内容输出为CSV

public:
    static UART_DEC& GetInstance() {
        static UART_DEC instance;
        return instance;
    }

    void SendData(char mSendBuffer[1024]);         // 发送数据
    void ReceiveData(std::string& mReceiveBuffer);      // 接收数据


    /*
    @brief :写入CLR-FLT位唤醒,进入STANDBY
    */
    void WakeUp_CLR_FLT()
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x08;
        uint8_t dataBytes = 0x89;

        char RegMapSendBuffer[1024];
        portdata.DataSendAnalysis(RegMapSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::SendData(RegMapSendBuffer);
    };


    /*
        brief : 把发出的4位16进制SPI数据拼接成一个可显示的字符串
    */
    void AddSendSPIDataToDisplay(uint16_t SPISend_Display, std::string& AllDataDisplay);

    /*
        @brief : 把接收的4位16进制SPI数据拼接成一个可显示的字符串
        @para : 
    */
    void AddRecSPIDataToDisplay(uint16_t SPIRec_Display, std::string& AllDataDisplayBuffer);

    //void UART_DEC::ReceiveData();
};  

#endif // !UARTDEC_H