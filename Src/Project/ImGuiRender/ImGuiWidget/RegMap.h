#ifndef REGMAP_H
#define REGMAP_H

#include "WidgetBase.h"
#include <cstdint>
#include "../ImGuiWidget/RegData.h"
#include "RegData.h"
#include "../ImGuiWidget/UART_DEC.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <iostream>
#include "Timer.h"
#include "imgui/imgui.h"
#include "RegMap.h"
#include "RegData.h"
#include "UART_DEC.h"
#include "../ImGuiWidget/UART_DEC.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <bitset>
#include <iostream>
#include "UART_DEC.h"
#include "imgui/imgui.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include "RegOperateInt.h"



class RegMap : public WidgetBase
{
public:
    RegMap();
    virtual ~RegMap();

    virtual void Render() override;

public://定义各个寄存器数组
    uint8_t DEVICE_ID_MAP;
    uint8_t FAULT_SUMMARY_MAP;
    uint8_t STATUS1_MAP;
    uint8_t STATUS2_MAP;
    uint8_t COMMAND_MAP;
    uint8_t SPI_IN_MAP;
    uint8_t CONFIG1_MAP;
    uint8_t CONFIG2_MAP;
    uint8_t CONFIG3_MAP;
    uint8_t CONFIG4_MAP;


private:

    bool mKeepReading = false; // 是否持续更新
    bool mWriteImmediately = false; // 是否立即写
    bool WriteImmediatylyFlag = false; // 立即写标志位

    ImFont* smallFont;

    void RenderRegisterRow(const char* name, uint8_t address, uint8_t* reg, bool editable);
    void RegMapDataUpdate_From_RegData() {
        DEVICE_ID_MAP = DEVICE_ID;
        FAULT_SUMMARY_MAP = FAULT_SUMMARY;
        STATUS1_MAP = STATUS1;
        STATUS2_MAP = STATUS2;
        COMMAND_MAP = COMMAND;
        SPI_IN_MAP = SPI_IN;
        CONFIG1_MAP = CONFIG1;
        CONFIG2_MAP = CONFIG2;
        CONFIG3_MAP = CONFIG3;
        CONFIG4_MAP = CONFIG4;
    };

    void RegMAPDataUpdate_to_RegData() {
        DEVICE_ID = DEVICE_ID_MAP;
        FAULT_SUMMARY = FAULT_SUMMARY_MAP;
        STATUS1 = STATUS1_MAP;
        STATUS2 = STATUS2_MAP;
        COMMAND = COMMAND_MAP;
        SPI_IN = SPI_IN_MAP;
        CONFIG1 = CONFIG1_MAP;
        CONFIG2 = CONFIG2_MAP;
        CONFIG3 = CONFIG3_MAP;
        CONFIG4 = CONFIG4_MAP;
    };

    void ReadALLRegister() {
        PortData portdata;
        Timer timer;
        uint8_t Command = 0xD1; //读取
        uint8_t AddressTable[10] = { 0 };
        uint8_t Addnum = 0;
        uint8_t RecData[10] = { 0 };

        char RegMapSendBuffer[1024];
        std::string RegMapRecieveBuffer;
        float RealTime = 0;

        ////////////////因为操作上限为5个寄存器，这里先对前5个寄存器进行连续读操作//////////////////////////
        Addnum = 5;
        AddressTable[0] = 0x01; //FAULT_SUMMARY
        AddressTable[1] = 0x02; //STATUS1
        AddressTable[2] = 0x03; //STATUS2
        AddressTable[3] = 0x08; //COMMAND
        AddressTable[4] = 0x09; //SPI_IN

        portdata.ContinuesReadDataSendAnalysis(RegMapSendBuffer, Command, Addnum, AddressTable); //进行结果分析
        UART_DEC::GetInstance().SendData(RegMapSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 1.0f);//间隔1ms

        UART_DEC::GetInstance().ReceiveData(RegMapRecieveBuffer);
        bool flag1 = portdata.ContinuesDataRecAnalysis(RegMapRecieveBuffer, Addnum, RecData);

        RegMapRecieveBuffer.clear(); //清空接收到的数据

        FAULT_SUMMARY = RecData[0];
        STATUS1 = RecData[1];
        STATUS2 = RecData[2];
        COMMAND = RecData[3];
        SPI_IN = RecData[4];
        //////////////////////////////////////////////////////////////////////////////////////////

        timer.Reset();
        while (timer.ElapsedMillis() < 1.0f);//间隔1ms
        
        ////////////////这里对后4个寄存器进行连续读操作//////////////////////////
        Addnum = 4;
        AddressTable[0] = 0x0A; //CONFIG1
        AddressTable[1] = 0x0B; //CONFIG2
        AddressTable[2] = 0x0C; //CONFIG3
        AddressTable[3] = 0x0D; //CONFIG4

        portdata.ContinuesReadDataSendAnalysis(RegMapSendBuffer, Command, Addnum, AddressTable); //进行结果分析
        UART_DEC::GetInstance().SendData(RegMapSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 1.0f);//间隔1ms
        UART_DEC::GetInstance().ReceiveData(RegMapRecieveBuffer);
        bool flag2 = portdata.ContinuesDataRecAnalysis(RegMapRecieveBuffer, Addnum, RecData);

        RegMapRecieveBuffer.clear(); //清空接收到的数据

        CONFIG1 = RecData[0];
        CONFIG2 = RecData[1];
        CONFIG3 = RecData[2];
        CONFIG4 = RecData[3];
        //////////////////////////////////////////////////////////////////////////////////////////
        
    };

    void WriteALLRegister() 
    {
        PortData portdata;
        Timer timer;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x01;
        uint8_t dataBytes = 0x00;
        uint8_t addRec = 0x00;
        uint8_t dataRec = 0x00;
        char RegMapSendBuffer[1024];
        std::string RegMapRecieveBuffer;

        for (addressBytes = 0x08; addressBytes <= 0x0D; addressBytes++) {
            switch (addressBytes) {
            case 0x08:
                dataBytes = COMMAND;
                break;
            case 0x09:
                dataBytes = SPI_IN;
                break;
            case 0x0A:
                dataBytes = CONFIG1;
                break;
            case 0x0B:
                dataBytes = CONFIG2;
                break;
            case 0x0C:
                dataBytes = CONFIG3;
                break;
            case 0x0D:
                dataBytes = CONFIG4;
                break;
            default:
                break;
            }
            portdata.DataSendAnalysis(RegMapSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(RegMapSendBuffer);

            timer.Reset();
            while (timer.ElapsedMillis() < 1.0f);//间隔1ms

            UART_DEC::GetInstance().ReceiveData(RegMapRecieveBuffer);
            bool flag1 = portdata.DataRecAnalysis(RegMapRecieveBuffer, addRec, dataRec);
            RegMapRecieveBuffer.clear();

            timer.Reset();
            while (timer.ElapsedMillis() < 2.0f);//间隔1ms
        }
    };

    void WriteCurrentRegister(uint8_t ClickAddress)
    {
        PortData portdata;
        Timer timer;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = ClickAddress;
        uint8_t dataBytes = 0x00;
        char RegMapSendBuffer[1024];

       switch (addressBytes) {
            case 0x08:
                dataBytes = COMMAND;
                break;
            case 0x09:
                dataBytes = SPI_IN;
                break;
            case 0x0A:
                dataBytes = CONFIG1;
                break;
            case 0x0B:
                dataBytes = CONFIG2;
                break;
            case 0x0C:
                dataBytes = CONFIG3;
                break;
            case 0x0D:
                dataBytes = CONFIG4;
                break;
            default:
                break;
        }
            portdata.DataSendAnalysis(RegMapSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(RegMapSendBuffer);     
    };

    /*
       @brief :比较RegMap可编辑部分与底层寄存器是否相同，如若不同则写该寄存器 (仍缺Command寄存器，因为CLR_FLT的点击恢复问题未处理)
       @param m_IsChange: RegOperateInt 是否发生操作变更，如果有的话，则会把底层寄存器进行SPI写入
    */
    void RegMapDiff2RegDataWrite(bool m_IsChange) {

        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x00;
        uint8_t dataBytes = 0x00;
        char RegMapSendBuffer[1024];

            if (SPI_IN_MAP != SPI_IN)
            {
                addressBytes = 0x09;
                if (m_IsChange) dataBytes = SPI_IN;
                else dataBytes = SPI_IN_MAP;
            }
            else if (CONFIG1_MAP != CONFIG1)
            {
                addressBytes = 0x0A;
                if (m_IsChange) dataBytes = CONFIG1;
                else dataBytes = CONFIG1_MAP;
            }
            else if (CONFIG2_MAP != CONFIG2)
            {
                addressBytes = 0x0B;
                if (m_IsChange) dataBytes = CONFIG2;
                else dataBytes = CONFIG2_MAP;
            }
            else if (CONFIG3_MAP != CONFIG3)
            {
                addressBytes = 0x0C;
                if (m_IsChange) dataBytes = CONFIG3;
                else dataBytes = CONFIG3_MAP;
            }
            else if (CONFIG4_MAP != CONFIG4)
            {
                addressBytes = 0x0D;
                if (m_IsChange) dataBytes = CONFIG4;
                else dataBytes = CONFIG4_MAP;
            }
        
        portdata.DataSendAnalysis(RegMapSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(RegMapSendBuffer);
    };

};


#endif
#pragma once
