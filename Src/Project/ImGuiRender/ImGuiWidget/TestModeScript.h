#ifndef TESTMODE_SCRIPT_H
#define TESTMODE_SCRIPT_H

#include "WidgetBase.h"
#include <imgui.h>
#include <cstdint>
#include <vector>
#include <string>
#include "UART_DEC.h"
#include "Timer.h"

#define ScriptLinesNumInput 32
#define DelayDefault_us 10
#define DelayMinimum_us 1
#define DelayMaximum_us 10000
#define ScriptGPIOJudgeLine 0xF0
#define DelayGPIOMinimum_us 5

class TestModeScript : public WidgetBase
{
public:
    TestModeScript();
    virtual ~TestModeScript();

    virtual void Render() override;

private:
    void RenderScriptRow(int index);
    void RenderColumnHeader();
    void UpdateScriptNum(uint8_t* ScriptNum);

    void SendScriptNum(uint8_t  ScriptNum);      //发送脚本的数据条数
    void SendScriptSingleData(uint8_t Add, uint8_t Data, uint16_t Delay, bool IsRead); //发送脚本的数据格式合成
    void SendScriptAllData(uint8_t Add[ScriptLinesNumInput], uint8_t Data[ScriptLinesNumInput], uint16_t Delay[ScriptLinesNumInput], bool IsRead[ScriptLinesNumInput],uint8_t ScriptNum); //发送脚本的数据格式合成
    void ScriptStart();        //脚本开始单次运行
    void CyclicScriptStart(uint8_t CyclicTime); //脚本开始循环运行
    void CyclicScriptStop();          //脚本停止运行
    void ScriptGUIClear();           //清除GUI中的脚本内容
    void ScriptMCUClear();            //清除MCU中的脚本内容


    void LoadScriptFromTXT(); //从txt文件中载入脚本
    void SaveScriptToTXT(bool promptFilename); //将脚本导入为txt文件



    uint8_t Add[ScriptLinesNumInput];      // 地址值 (0-255)
    uint8_t Data[ScriptLinesNumInput];     // 数据值 (0-255)
    uint16_t Delay[ScriptLinesNumInput];   // 延迟值 (0-10000)
    bool IsRead[ScriptLinesNumInput];      // R/W状态
    uint8_t ScriptNum = 0;//统计脚本数量
    int CyclicScriptDelay_ms = 1; //循环发送脚本时的时间间隔 ms
};

#endif // TESTMODE_SCRIPT_H