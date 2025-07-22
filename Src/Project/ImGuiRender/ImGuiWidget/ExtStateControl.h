#ifndef EXTSTATCONTROL_H
#define EXTSTATCONTROL_H

#include "WidgetBase.h"
#include <cstdint>
#include <string>
#include <vector>
#include "UART_DEC.h"
#include "Timer.h"
#include "RegData.h"
#include <windows.h> 

class ExtStateControl : public WidgetBase
{
public:
    // 操作模式枚举
    enum OperationMode {
        PWM_MODE = 0,       //0
        INDEPENDENT_MODE,   //1
        PH_EN_MODE          //2
    };

    // 启动状态枚举
    enum StartStopState {
        START = 0,
        STOP
    };

    // 箭头方向枚举
    enum ArrowDirection {
        NO_ARROW = 0,
        LEFT_ARROW,
        RIGHT_ARROW
    };

    ExtStateControl();
    virtual ~ExtStateControl();

    virtual void Render() override;

    // 获取当前设置值
    OperationMode GetCurrentMode() const { return m_currentMode; }
    StartStopState GetStartStopState() const { return m_startStopState; }
    float GetIn1Pwm() const { return PWM_IN1; }
    float GetIn2Pwm() const { return PWM_IN2; }
    ArrowDirection GetArrowDirection() const { return m_arrowDirection; }

private:
    //图片绘制用变量
    ImTextureID m_circuitTexture = (ImTextureID)0; // 显式初始化为0
    int m_imageWidth = 0;                   // 图片宽度
    int m_imageHeight = 0;                  // 图片高度


    // 当前操作模式 记录变量
    OperationMode m_currentMode;

    // 启动/停止状态 记录变量
    StartStopState m_startStopState;

    // 箭头方向 记录变量
    ArrowDirection m_arrowDirection;

    //标志位，判断PWM1和PWM2的滑块是否被释放。
    bool SliderReleasedFlag_PWM1 = false;
    bool SliderReleasedFlag_PWM2 = false;
    
    //存储选择好的频率
    uint16_t SelectedFreq = 1000;//
    int FreqSeletedIndex = 0;

    //标志位，判断频率下拉框是否发生更改
    bool FreqChangeFlag = false;

    // PWM控制值 (0-100)
    int PWM_IN1;
    int PWM_IN2;

    uint8_t Config3_Ext; //用于配置模式， 00PH/EN  01Independent 10/11PWM MODE

    //存储下拉框函数DrawCustomDropdown，其配置变量的结构体
    struct DropdownConfig {
        const char* label;      // 可选：标签文本
        ImVec2 Pos;                 // 2. 基于窗口尺寸的相对位置比例 (0~1)
        std::vector<std::string> Options;      // 3. 下拉选项 (2~10个)
        std::vector<uint16_t> values;               // 选项对应的实际值
        uint16_t* CurrentSelectedValue;
        int* SelectedIndex;
        int default_selected;            // 4. 默认选中项索引
        bool* valuechangedflag = nullptr;       // 5. 值改变标志位指针
    };

    /*
    @brief :初始化文本及下拉框的配置。
    @para 文本名字：显示的文本名字
    @para PosX/Y：   控件所处的子窗口位置，比如textX=0.1f,textY=0.2f代表希望文本处于：x坐标位于本子窗口10%宽度，y坐标位于本子窗口20%高度
    @para value：    赋值的对应变量
    @para options：   使用的下拉框选项表 显示文本
    @para values：   对应下拉框选项的实际值
    @para default_selected:     初始的默认选项索引值
    @para valuechangedflag:     下拉框内容是否发生改变的标志位
    */
    // 配置列表
    DropdownConfig dropdownconfig = {
            // 文本名字   ||    PosX,PosY     ||  Drop-down box options              ||                 values           || Default option ||  Flag for option is changed
            "PWM_FREQ",    ImVec2(0.02f, 0.78f), {"1kHz", "5kHz", "10kHz", "15kHz", "25kHz"}, {uint16_t(1000), uint16_t(5000), uint16_t(10000), uint16_t(15000), uint16_t(25000)}, &SelectedFreq, &FreqSeletedIndex, 1,            &FreqChangeFlag};

    /*
    @brief :绘制所有的文本及其对应的下拉框，并实现：
    1、对应的config.value变动的时候，对应修改下拉框的值
    2、下拉框值变动时，将变动的下拉框的值赋给对应变量
    @para config: DropdownConfig 该结构体存放所需的所有参数
    @para window_size: 子窗口尺寸
    @return :是否操作下拉框
*/
    bool DrawCustomDropdown(const DropdownConfig& config, ImVec2 window_size);


    // 渲染PWM控制组件
    void ExtStateControl::RenderPwmControl(const char* label, int* value,
        ImVec2 windowSize,
        ImVec2 Pos,
        bool& slider_released);


    // 电路图相关文本 (根据图片内容)
    struct CircuitText {
        std::string leftPower = "VM\n0.1 μF\nVCP\nCharge Pump\nVDD";
        std::string rightPower = "VDD\nVCP\nGND";
        std::string digitalIO = "Digital IOs\nnSLEEP\nEN/IN1\nPH/IN2\nnSCS\nSDI\nSCLK\nSDO";
        std::string gateDriver = "Gate Driver\nVS\nVDD\nVCP\nH(SNS)\nOUT";
        std::string protection = "nFAULT\nnPROPI\nRIPROPI";
    } m_circuitText;



    void StartPWM();
    void StopPWM();
    void Set_IN1PWMDuty(uint8_t IN1_Duty); //PTD14-FTM2.CH5
    void Set_IN2PWMDuty(uint8_t IN2_Duty); //PTD13-FTM2.CH4
    void Set_PWMFreq(uint16_t PWM_Freq);

    void SetModeWrite(uint8_t REGDATA); //写Config3寄存器


    bool ExtStateControl::LoadTexture(const char* path);


};

#endif // EXTSTATCONTROL_H