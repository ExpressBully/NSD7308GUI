#ifndef TESTMODEREG_H
#define TESTMODEREG_H

#include "WidgetBase.h"
#include <imgui.h>
#include <cstdint>
#include <vector>
#include <string>
#include "UART_DEC.h"
#include "Timer.h"

// 预定义选项类型
enum class OptionWidth {
    BITS_1 = 1,
    BITS_2 = 2,
    BITS_3 = 3,
    BITS_4 = 4
};



class TestModeReg : public WidgetBase
{
public:
    TestModeReg();
    virtual ~TestModeReg();

    virtual void Render() override;



private:

    bool m_IsECC_EN = true; // ECC是否启用状态标志
    bool m_IsTSD_EN = true; // TSD是否启用状态标志
    bool m_IsOCP_EN = true; // OCP是否启用状态标志
    char m_eccValueInput[8] = "0"; // 存储输入框内容的缓冲区
    bool m_eccHexInputActive = false; // 跟踪输入框是否处于编辑状态
    bool mIsWriteImediately = false; //是否开启立即写入功能


    uint8_t ECCValue = 0; //ECC值，一共6位，来自输入框
    uint8_t TMREGAValue = 0; //TM_REGA值，一共8位，来自输入框
    uint8_t TMREGBValue = 0; //TM_REGA值，一共8位，来自输入框


    // 初始化测试寄存器
    uint8_t OTP_REG0 = 0;
    uint8_t OTP_REG1 = 0;
    uint8_t OTP_REG2 = 0;
    uint8_t OTP_REG3 = 0;
    uint8_t OTP_REG4 = 0;
    uint8_t OTP_REG5 = 0;
    uint8_t OTP_REG6 = 0;
    uint8_t OTP_REG7 = 0;
    uint8_t TM_REG00 = 0;
    uint8_t TM_REG01 = 0;
    uint8_t TM_REG02 = 0;
    uint8_t TM_REG03 = 0;
    uint8_t TM_REG04 = 0;


    // 初始化测试寄存器变量
    uint8_t OSC = 10;
    uint8_t CH1_HS_CS_OS = 0;

    uint8_t VREF = 0;
    uint8_t IREF = 0;

    uint8_t CH2_HS_CS_OS = 0;
    uint8_t CH1_HS_CS_GAIN = 0;
    uint8_t CH2_HS_CS_GAIN = 0;

    //OTP_REG3
    uint8_t CH2_HS_CS_OS32 = 0;
    uint8_t CH1_LS_CS_GAIN = 0;
    uint8_t CH2_LS_CS_GAIN = 0;

    //OTP_REG4
    uint8_t Dis_Rectify_LS = 0;
    uint8_t CH1_LS_CS_OS = 0;
    uint8_t CH2_LS_CS_OS = 0;
    
    //


    //初始化测试寄存器中的只读型变量
    uint8_t ITRIP_PIN = 3;
    uint8_t DIAG_PIN = 7;
    uint8_t SR_PIN = 4;
    uint8_t MODE_PIN = 1;


    //初始化测试寄存器中的只读型变量的二进制位宽
    int ITRIP_PIN_Width = 3;
    int DIAG_PIN_Width = 3;
    int SR_PIN_Width = 3;
    int MODE_PIN_Width = 2;


    // 存储索引值，用于绘制下拉框
    int selectedOSC = 0;
    int selectedVREF = 0;
    int selectedIREF = 0;
    int selectedDis_Rectify_LS = 0;
    int selectedCH1_HS_CS_GAIN = 0;
    int selectedCH2_HS_CS_GAIN = 0;
    int selectedCH1_LS_CS_GAIN = 0;
    int selectedCH2_LS_CS_GAIN = 0;
    int selectedCH1_HS_CS_OS = 0;
    int selectedCH2_HS_CS_OS = 0;
    int selectedCH1_LS_CS_OS = 0;
    int selectedCH2_LS_CS_OS = 0;

    //存储下拉框内容是否发生改变标志位
    bool mIs_OSC_ChangeFlag = false;
    bool mIs_VREF_ChangeFlag = false;
    bool mIs_IREF_ChangeFlag = false;
    bool mIs_DisRectifyLS_ChangeFlag = false;
    bool mIs_CH1_HSCS_GAIN_ChangeFlag = false;
    bool mIs_CH2_HSCS_GAIN_ChangeFlag = false;
    bool mIs_CH1_LSCS_GAIN_ChangeFlag = false;
    bool mIs_CH2_LSCS_GAIN_ChangeFlag = false;
    bool mIs_CH1_HSCS_OS_ChangeFlag = false;
    bool mIs_CH2_HSCS_OS_ChangeFlag = false;
    bool mIs_CH1_LSCS_OS_ChangeFlag = false;
    bool mIs_CH2_LSCS_OS_ChangeFlag = false;

    //存储输入框是否激活的标志位
    bool g_eccInputActive = false;
    bool g_TMREGAInputActive = false;
    bool g_TMREGBInputActive = false;

    // 初始化下拉框选项表
    const std::vector<std::string> TestModeReg::options1Bit = { "0", "1" };
    const std::vector<std::string> TestModeReg::options2Bits = { "00", "01", "10", "11" };
    const std::vector<std::string> TestModeReg::options3Bits = {
        "000", "001", "010", "011",
        "100", "101", "110", "111"
    };
    const std::vector<std::string> TestModeReg::options4Bits = {
        "0000", "0001", "0010", "0011",
        "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011",
        "1100", "1101", "1110", "1111"
    };



    /*
    @brief :初始化文本及下拉框的配置。
    @para 文本名字：显示的文本名字
    @para textX/Y： 文本所处的子窗口位置，比如textX=0.1f,textY=0.2f代表希望文本处于：x坐标位于本子窗口10%宽度，y坐标位于本子窗口20%高度
    @para Combo/Y:  下拉框所处的子窗口位置，原理同textX/Y
    @para value：    赋值的对应变量
    @para Index:     存储索引值，别管有什么用。照葫芦命名
    @para width：    该变量的二进制宽度
    @para option：   使用的下拉框选项表
    @para mIsChange: 下拉框内容是否发生改变的标志位
    */
    void InitRegisterConfigs()  //文本以及下拉框选择内容
    {
        regConfigs = {
            // 文本名字        ||    textX,textY  ||  comboX,comboY  ||  value        ||    Index                     ||    width           ||   options     ||   Flag
            {"OSC<3:0>",            0.05f, 0.08f,     0.05f, 0.13f,     &OSC,               &selectedOSC,               OptionWidth::BITS_4,    &options4Bits,    &mIs_OSC_ChangeFlag               },
            {"VREF<2:0>",           0.25f, 0.08f,     0.25f, 0.13f,     &VREF,              &selectedVREF,              OptionWidth::BITS_3,    &options3Bits,    &mIs_VREF_ChangeFlag              },
            {"IREF<2:0>",           0.45f, 0.08f,     0.45f, 0.13f,     &IREF,              &selectedIREF,              OptionWidth::BITS_3,    &options3Bits,    &mIs_IREF_ChangeFlag              },
            {"Dis_Rectify_Ls<1>",   0.65f, 0.08f,     0.65f, 0.13f,     &Dis_Rectify_LS,    &selectedDis_Rectify_LS,    OptionWidth::BITS_1,    &options1Bit,     &mIs_DisRectifyLS_ChangeFlag      },

            {"CH1_HS_CS_GAIN<2:0>", 0.05f, 0.23f,     0.05f, 0.28f,     &CH1_HS_CS_GAIN,    &selectedCH1_HS_CS_GAIN,    OptionWidth::BITS_3,    &options3Bits,    &mIs_CH1_HSCS_GAIN_ChangeFlag     },
            {"CH2_HS_CS_GAIN<2:0>", 0.27f, 0.23f,     0.27f, 0.28f,     &CH2_HS_CS_GAIN,    &selectedCH2_HS_CS_GAIN,    OptionWidth::BITS_3,    &options3Bits,    &mIs_CH2_HSCS_GAIN_ChangeFlag     },
            {"CH1_LS_CS_GAIN<2:0>", 0.49f, 0.23f,     0.49f, 0.28f,     &CH1_LS_CS_GAIN,    &selectedCH1_LS_CS_GAIN,    OptionWidth::BITS_3,    &options3Bits,    &mIs_CH1_LSCS_GAIN_ChangeFlag     },
            {"CH2_LS_CS_GAIN<2:0>", 0.71f, 0.23f,     0.71f, 0.28f,     &CH2_LS_CS_GAIN,    &selectedCH2_LS_CS_GAIN,    OptionWidth::BITS_3,    &options3Bits,    &mIs_CH2_LSCS_GAIN_ChangeFlag     },

            {"CH1 HS CS OS<3:0>",   0.05f, 0.38f,     0.05f, 0.43f,      &CH1_HS_CS_OS,      &selectedCH1_HS_CS_OS,      OptionWidth::BITS_4,    &options4Bits,   &mIs_CH1_HSCS_OS_ChangeFlag       },
            {"CH2 HS CS OS<3:0>",   0.30f, 0.38f,     0.30f, 0.43f,      &CH2_HS_CS_OS,      &selectedCH2_HS_CS_OS,      OptionWidth::BITS_4,    &options4Bits,   &mIs_CH2_HSCS_OS_ChangeFlag       },
            {"CH1_LS_CS_OS<2:0>",   0.55f, 0.38f,     0.55f, 0.43f,      &CH1_LS_CS_OS,      &selectedCH1_LS_CS_OS,      OptionWidth::BITS_3,    &options3Bits,   &mIs_CH1_LSCS_OS_ChangeFlag       },
            {"CH2_LS_CS_OS<2:0>",   0.80f, 0.38f,     0.80f, 0.43f,      &CH2_LS_CS_OS,      &selectedCH2_LS_CS_OS,      OptionWidth::BITS_3,    &options3Bits,   &mIs_CH2_LSCS_OS_ChangeFlag       }
        };
    }

    /*
    @brief: 只读寄存器的值显示,会显示16进制和2进制
    @param 文本名字
    @param textX,textY:显示文本的X Y位置（按子窗口大小换算的比例位置）
    @param value: 显示的值的来源
    @param resultX,resultY : 显示的值的X Y位置（按子窗口大小换算的比例位置）
    @param Resault Binary Width: 要显示的值有几位二进制数
    */
    void InitRegisterDisplayConfigs()
    {
        regDisplayConfigs = {
            // 文本名字      ||    textX,textY  ||  value     ||  resultX,resultY ||  Resault Binary Width
            {"ITRIP_PIN<3:0>",     0.05f, 0.53f,    &ITRIP_PIN,    0.05f, 0.58f,      &ITRIP_PIN_Width},
            {"DIAG_PIN<3:0>",      0.30f, 0.53f,    &DIAG_PIN,     0.30f, 0.58f,      &DIAG_PIN_Width},
            {"SR_PIN<3:0>",        0.5f, 0.53f,    &SR_PIN,        0.5f, 0.58f,       &SR_PIN_Width},
            {"MODE_PIN<3:0>",      0.7f, 0.53f,    &MODE_PIN,      0.7f, 0.58f,       &MODE_PIN_Width}
        };
    }


    //绘制文本和下拉框用结构体
    struct RegisterConfig {
        std::string name;       // 寄存器名称(如"OSC<3:0>")
        float textX, textY;     // 文本位置
        float comboX, comboY;   // 下拉框位置
        uint8_t* value;         // 指向寄存器值的指针
        int* selectedIndex;      // 指向当前选中索引的指针
        OptionWidth width;      // 选项位数类型
        const std::vector<std::string>* options; // 指向外部选项数组的指针
        bool* mIsChangeFlag; //下拉框内容是否发生改变
    };

    // 配置列表
    std::vector<RegisterConfig> regConfigs;

    /*
        @brief :绘制所有的文本及其对应的下拉框，并实现：
        1、对应的config.value变动的时候，对应修改下拉框的值
        2、下拉框值变动时，将变动的下拉框的值赋给对应变量
    */
    void DrawLabelAndCombo(const RegisterConfig& config, ImVec2 windowSize)
    {
        //ImVec2 windowSize = ImGui::GetContentRegionAvail();

        // 绘制文本标签
        ImGui::SetCursorPos(ImVec2(windowSize.x * config.textX, windowSize.y * config.textY));
        ImGui::Text("%s", config.name.c_str());

        // 准备选项指针数组
        std::vector<const char*> optionsPtrs;
        for (const auto& opt : *config.options) {
            optionsPtrs.push_back(opt.c_str());
        }
        // 根据当前寄存器值更新选中索引
        // 确保值在合法范围内
        uint8_t currentValue = *config.value;
        if (currentValue >= config.options->size()) {
            currentValue = static_cast<uint8_t>(config.options->size() - 1);
        }
        *config.selectedIndex = static_cast<int>(currentValue);

        // 绘制下拉框
        ImGui::SetCursorPos(ImVec2(windowSize.x * config.comboX, windowSize.y * config.comboY));
        ImGui::SetNextItemWidth(100);

        // 使用临时变量存储选中索引
        int tempSelected = *config.selectedIndex;
        if (ImGui::Combo(("##" + config.name).c_str(), &tempSelected,
            optionsPtrs.data(), static_cast<int>(optionsPtrs.size())))
        {
            // 当下拉框值变化时，更新寄存器和选中索引
            *config.selectedIndex = tempSelected;
            *config.value = static_cast<uint8_t>(tempSelected);
            *config.mIsChangeFlag = true;
        }
    }






    //绘制文本和16进制 2进制结果框用结构体
    struct RegisterDisplayConfig {
        std::string name;       // 寄存器名称(如"OSC<3:0>")
        float textX, textY;     // 文本位置
        uint8_t* value;         // 指向寄存器值的指针
        float resultX, resultY; // 结果显示框位置
        int* binaryWidth;       // 指向二进制显示位数的指针
    };

    std::vector<RegisterDisplayConfig> regDisplayConfigs;


    /*
        @brief: 绘制文本、二进制位数选择框和结果显示框
    */
    void DrawLabelAndResult(const RegisterDisplayConfig& config, ImVec2 windowSize)
    {
        // 绘制文本标签
        ImGui::SetCursorPos(ImVec2(windowSize.x * config.textX, windowSize.y * config.textY));
        ImGui::Text("%s", config.name.c_str());

        // 计算并格式化结果显示文本
        uint8_t currentValue = *config.value;
        std::string hexText = "0x" + IntToHexString(currentValue);
        std::string binText = "0b" + IntToBinaryString(currentValue, *config.binaryWidth);
        std::string resultText = hexText + " (" + binText + ")";

        // 绘制结果显示框
        ImGui::SetCursorPos(ImVec2(windowSize.x * config.resultX, windowSize.y * config.resultY));
        ImGui::Text("%s", resultText.c_str());
    }

    // 辅助函数：将整数转换为2位16进制字符串
    std::string IntToHexString(uint8_t value)
    {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02X", value);
        return std::string(buffer);
    }

    // 辅助函数：将整数转换为指定位数的二进制字符串
    std::string IntToBinaryString(uint8_t value, int width)
    {
        std::string result;
        // 从最高位开始，只显示指定的位数
        for (int i = width - 1; i >= 0; --i) {
            result += (value & (1 << i)) ? '1' : '0';
        }
        return result;
    }


    /**
     * @brief 绘制文本和输入框控件，包含文本和输入框，并在鼠标移开或者回车的时候把输入框内容进行幅值
     * @param label 标签内容
     * @param windowSize 窗口尺寸
     * @param labelPos 标签位置（按子窗口大小换算的比例位置）
     * @param inputPos 输入框位置（按子窗口大小换算的比例位置）
     * @param inputWidth 输入框宽度（单位为像素）
     * @param Value 输入框的值 赋给的变量（引用地址）
     * @param isInputActive 输入状态标志，别管什么作用
     * @param bitWidth 二进制位数限制（如6对应0x3F），写入超过3F的值会自动更新为3F
     */
    void DrawLabelAndInputControl(
        const char* label,
        ImVec2 windowSize,
        ImVec2 labelPos,
        ImVec2 inputPos,
        float inputWidth,
        uint8_t& Value,
        bool& isInputActive,
        int bitWidth) 
    {
        // 固定16字符缓冲区（自动管理）
        static char inputBuffer[16];
        snprintf(inputBuffer, sizeof(inputBuffer), "%02X", Value);

        std::string inputId = "##";
        inputId += 10*labelPos.x + 10*labelPos.y; //使输入框ID不重复，只要label位置不重复

        // 计算最大值（例如bitWidth=6 → 0x3F）
        const uint8_t maxValue = (1 << bitWidth) - 1;

        // 1. 绘制标签（带动态位数显示）
        ImGui::SetCursorPos(ImVec2(
            windowSize.x * labelPos.x,
            windowSize.y * labelPos.y
        ));
        ImGui::Text(label);

        // 2. 绘制输入框
        ImGui::SetCursorPos(ImVec2(
            windowSize.x * inputPos.x,
            windowSize.y * inputPos.y
        ));
        ImGui::SetNextItemWidth(inputWidth);


        if (ImGui::InputText(
            inputId.c_str(),
            inputBuffer,
            sizeof(inputBuffer),
            ImGuiInputTextFlags_CharsHexadecimal |
            ImGuiInputTextFlags_CharsUppercase))
        {
            isInputActive = true;
        }

        // 3. 处理输入完成
        if (isInputActive && !ImGui::IsItemActive())
        {
            isInputActive = false;

            uint8_t newValue;
            if (sscanf(inputBuffer, "%hhx", &newValue) == 1)
            {
                // 应用位数限制（如bitWidth=6时，0x3F & 0x3F = 0x3F）
                Value = newValue & maxValue;
                printf("ECC value set to: 0x%02X (Max: 0x%02X)\n", Value, maxValue);

                // 更新显示（自动格式化为大写）
                snprintf(inputBuffer, sizeof(inputBuffer), "%02X", Value);
            }
            else
            {
                // 输入无效时重置为当前值
                snprintf(inputBuffer, sizeof(inputBuffer), "%02X", Value);
            }
        }
    }


    struct ButtonState {
        ImU32 color;        //该状态按钮颜色
        ImU32 hoverColor;   //该状态鼠标悬停按钮颜色
        const char* text;   //文本内容
        std::function<void()> action;   //按钮点击，切换时执行的函数
    };

    /**
 * @brief 双状态切换按钮控件
 * @param windowSize：本子窗口窗口大小
 * @param pos 按钮位置，横纵坐标为子窗口大小的比例
 * @param size 按钮尺寸
 * @param currentState 当前状态（引用）
 * @param stateA 状态A配置 {颜色, 悬停颜色, 文本, 回调函数}
 * @param stateB 状态B配置 {颜色, 悬停颜色, 文本, 回调函数}
 */
    void DrawToggleButton(
        ImVec2 windowSize,
        ImVec2 labelPos,
        ImVec2 size,
        bool& currentState,
        const ButtonState& stateA,
        const ButtonState& stateB)
    {
        ImGui::SetCursorPos(ImVec2(
            windowSize.x * labelPos.x,
            windowSize.y * labelPos.y
        ));

        // 根据状态设置颜色
        const auto& state = currentState ? stateA : stateB;
        ImGui::PushStyleColor(ImGuiCol_Button, state.color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, state.hoverColor);

        // 绘制按钮
        if (ImGui::Button(state.text, size))
        {
            state.action();      // 执行回调
            currentState = !currentState; // 切换状态
        }
        ImGui::PopStyleColor(2);
    }


 /**
    * @brief 绘制文本标签和勾选框控件
    * @param windowSize: 子窗口大小
    * @param text 显示的文本内容
    * @param labelPos 文本位置（按子窗口大小换算的比例位置）
    * @param checkboxPos 勾选框位置（按子窗口大小换算的比例位置）
    * @param pFlag 指向bool标志位的指针（勾选状态自动更新）
    * @param checkboxSize 勾选框尺寸（可选，默认20x20）
 */
    void DrawLabelWithCheckbox(
        ImVec2 windowSize,
        const std::string& text,
        ImVec2 labelPos,
        ImVec2 checkboxPos,
        bool* pFlag,
        ImVec2 checkboxSize = ImVec2(20, 20))
    {
        // 1. 绘制文本
        ImGui::SetCursorPos(ImVec2(windowSize.x* labelPos.x, windowSize.y* labelPos.y));
        ImGui::Text("%s", text.c_str());

        // 2. 绘制勾选框
        ImGui::SetCursorPos(ImVec2(windowSize.x * checkboxPos.x, windowSize.y * checkboxPos.y));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::Checkbox(("##" + text).c_str(), pFlag)) {
            // 可选：勾选状态变化时的额外处理

             printf("%s state changed to %s\n", text.c_str(), *pFlag ? "true" : "false");
        }
        ImGui::PopStyleVar();
    }

    ////////////////////////////////////////////以上为绘图用封装函数，描述样式以及位置的绘图类/////////////////////////////////////////////////////
    ////////////////////////////////////////////以下为指令用封装函数，指令分析及发送的执行类////////////////////////////////////////////////

    /*
        *brief: 当开启 立即写 功能时使用，对mIs_ChageFlag进行轮询，得到有为真的，则对对应寄存器进行写入
    */
    void TMRegWriteImediately() 
    {
        PortData portdata;
        Timer timer;
        uint8_t addressBytes = 0x3D;
        uint8_t dataBytes = 0xC1;
        uint8_t Command = 0xA3; //写入
        char TMRegSendBuffer[1024];
        if (mIs_OSC_ChangeFlag || mIs_CH1_HSCS_OS_ChangeFlag)    //OTP_REG0
        {
            addressBytes = 0x2C;
            dataBytes = (OSC << 4) | CH1_HS_CS_OS;
            portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            mIs_OSC_ChangeFlag = mIs_CH1_HSCS_OS_ChangeFlag = false;

        }
        else if (mIs_VREF_ChangeFlag || mIs_IREF_ChangeFlag)    //OTP_REG1
        {
            addressBytes = 0x2D;
            dataBytes = (VREF << 3) | IREF;
            portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            mIs_VREF_ChangeFlag = mIs_IREF_ChangeFlag = false;
        }
        else if (mIs_CH2_HSCS_OS_ChangeFlag || mIs_CH1_HSCS_GAIN_ChangeFlag || mIs_CH2_HSCS_GAIN_ChangeFlag) //OTP_REG2
        {
            addressBytes = 0x2E;
            dataBytes = (CH2_HS_CS_OS << 6) | (CH1_HS_CS_GAIN << 3) | CH2_HS_CS_GAIN;
            portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            if (mIs_CH2_HSCS_OS_ChangeFlag)
            {
                timer.Reset();
                while (timer.ElapsedMillis() < 5.0f);//间隔1ms
                addressBytes = 0x2F;
                dataBytes = (CH2_HS_CS_OS << 4) | (CH1_LS_CS_GAIN << 3) | CH2_LS_CS_GAIN;
                portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
                UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            }
            mIs_CH2_HSCS_OS_ChangeFlag = mIs_CH1_HSCS_GAIN_ChangeFlag = mIs_CH2_HSCS_GAIN_ChangeFlag = false;

        }
        else if (mIs_CH1_LSCS_GAIN_ChangeFlag || mIs_CH2_LSCS_GAIN_ChangeFlag) //OTP_REG3
        {
            addressBytes = 0x2F;
            dataBytes = (CH2_HS_CS_OS << 4) | (CH1_LS_CS_GAIN << 3) | CH2_LS_CS_GAIN;
            portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            mIs_CH1_LSCS_GAIN_ChangeFlag = mIs_CH2_LSCS_GAIN_ChangeFlag = false;
        }
        else if (mIs_DisRectifyLS_ChangeFlag || mIs_CH1_LSCS_OS_ChangeFlag || mIs_CH2_LSCS_OS_ChangeFlag) //OTP_REG4
        {
            addressBytes = 0x30;
            dataBytes = (ECCValue << 7) | (Dis_Rectify_LS << 6) | (CH1_LS_CS_OS << 3) | CH2_LS_CS_OS;
            portdata.DataSendAnalysis(TMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(TMRegSendBuffer);
            mIs_DisRectifyLS_ChangeFlag = mIs_CH1_LSCS_OS_ChangeFlag = mIs_CH2_LSCS_OS_ChangeFlag = false;
        }
    }

    void RunTestMode() //写C1 A0进入测试模式 
    {
        PortData portdata;
        Timer timer;
        uint8_t addressBytes = 0x3D;
        uint8_t dataBytes = 0xC1;
        uint8_t Command = 0xA3; //写入
        char RunTMSendBuffer[1024];

        portdata.DataSendAnalysis(RunTMSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(RunTMSendBuffer);

        timer.Reset();
        while (timer.ElapsedMillis() < 10.0f);//间隔1ms

        dataBytes = 0xA0;
        portdata.DataSendAnalysis(RunTMSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(RunTMSendBuffer);

    }
    void ExitTestMode()//写00退出测试模式 
    {
        PortData portdata;
        uint8_t addressBytes = 0x3D;
        uint8_t dataBytes = 0x00;
        uint8_t Command = 0xA3; //写入
        char RunTMSendBuffer[1024];

        portdata.DataSendAnalysis(RunTMSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(RunTMSendBuffer);
    }

    void WriteALLTMReg()
    {
        OTP_REG0 = (OSC << 4) | CH1_HS_CS_OS;
        OTP_REG1 = (VREF << 3) | IREF;
        OTP_REG2 = (CH2_HS_CS_OS << 6) | (CH1_HS_CS_GAIN << 3) | CH2_HS_CS_GAIN;
        OTP_REG3 = (CH2_HS_CS_OS << 4) | (CH1_LS_CS_GAIN << 3) | CH2_LS_CS_GAIN;
        OTP_REG4 = (ECCValue << 7) | (Dis_Rectify_LS << 6) | (CH1_LS_CS_OS << 3) | CH2_LS_CS_OS;

        PortData portdata;
        Timer timer;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x01;
        uint8_t dataBytes = 0x00;
        char WriteALLTMRegSendBuffer[1024];

        for (addressBytes = 0x2C; addressBytes <= 0x30; addressBytes++) {
            switch (addressBytes) {
            case 0x2C:
                dataBytes = OTP_REG0;
                break;
            case 0x2D:
                dataBytes = OTP_REG1;
                break;
            case 0x2E:
                dataBytes = OTP_REG2;
                break;
            case 0x2F:
                dataBytes = OTP_REG3;
                break;
            case 0x30:
                dataBytes = OTP_REG4;
                break;
            default:
                break;
            }
            portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
            UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
            timer.Reset();
            while (timer.ElapsedMillis() < 5.0f);//间隔1ms
        }
    
    }

    void Write_ECC()   //把ECC值写入
    {
        PortData portdata;
        Timer timer;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x00;
        uint8_t dataBytes = 0x00;
        char WriteECCSendBuffer[1024];

        addressBytes = 0x30;
        //dataBytes = 0x80;
        dataBytes = (ECCValue << 7) | (Dis_Rectify_LS << 6) | (CH1_LS_CS_OS << 3) | CH2_LS_CS_OS;
        portdata.DataSendAnalysis(WriteECCSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteECCSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 5.0f);//间隔5ms

        addressBytes = 0x31;
        dataBytes = (ECCValue << 6) & 0x80;
        portdata.DataSendAnalysis(WriteECCSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteECCSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 5.0f);//间隔5ms

        addressBytes = 0x32;
        dataBytes = (ECCValue << 5) & 0x80;
        portdata.DataSendAnalysis(WriteECCSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteECCSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 5.0f);//间隔5ms

        addressBytes = 0x33;
        dataBytes = (ECCValue << 2) & 0xE0;
        portdata.DataSendAnalysis(WriteECCSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteECCSendBuffer);
        timer.Reset();
        while (timer.ElapsedMillis() < 5.0f);//间隔5ms
    }

    void DIS_ECC()  //0x3C->0x02
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3C;
        uint8_t dataBytes = 0x02;
        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void EN_ECC()   //0x3C->0x00
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3C;
        uint8_t dataBytes = 0x00;
        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void DIS_OCP(bool* mIsTSD_EN)
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3A;
        uint8_t dataBytes = 0x00;

        if (mIsTSD_EN) { dataBytes = 0x08; }
        else { dataBytes = 0x0C; } //这个判断，以防对DIS_TSD位进行更改

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void EN_OCP(bool* mIsTSD_EN) 
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3A;
        uint8_t dataBytes;

        if (mIsTSD_EN)  dataBytes = 0x00;
        else dataBytes = 0x04; //这个判断，以防对DIS_TSD位进行更改

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void DIS_TSD(bool* mIsOCP_EN)  
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3A;
        uint8_t dataBytes;

        if (mIsOCP_EN)  dataBytes = 0x04;
        else dataBytes = 0x0C; //这个判断，以防对DIS_OCP位进行更改

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void EN_TSD(bool* mIsOCP_EN)
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x3A;
        uint8_t dataBytes;

        if (mIsOCP_EN)  dataBytes = 0x00;
        else dataBytes = 0x08; //这个判断，以防对DIS_OCP位进行更改

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }
    
    void Write_TM_REGA()
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x38;
        uint8_t dataBytes = TMREGAValue;

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

    void Write_TM_REGB()
    {
        PortData portdata;
        uint8_t Command = 0xA3; //写入
        uint8_t addressBytes = 0x39;
        uint8_t dataBytes = TMREGBValue;

        char WriteALLTMRegSendBuffer[1024];
        portdata.DataSendAnalysis(WriteALLTMRegSendBuffer, Command, addressBytes, dataBytes);
        UART_DEC::GetInstance().SendData(WriteALLTMRegSendBuffer);
    }

};

#endif