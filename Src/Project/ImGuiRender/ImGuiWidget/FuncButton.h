#ifndef FUNCBUTTON_H
#define FUNCBUTTON_H

#include "WidgetBase.h"
#include <functional>
#include <imgui.h>
#include "../ImGuiWidget/RegData.h"
#include "RegData.h"
#include "../ImGuiWidget/UART_DEC.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"

class FuncButton : public WidgetBase
{
public:
    FuncButton();
    virtual ~FuncButton();

    virtual void Render() override;

    // 按钮状态
    bool wakeState = false;          // 默认Awake
    bool enableDriverState = false; // 默认Disabled

    // 回调函数
    std::function<void(bool)> onWakeChanged = nullptr;
    std::function<void(bool)> onEnableDriverChanged = nullptr;


private:
    void RenderToggle(const char* label, bool* state,
        const char* activeText, const char* inactiveText);

    // 动画参数
    float wakeAnimPos = 1.0f;
    float enableDriverAnimPos = 0.0f;
    const float animSpeed = 0.2f;

    //nSLEEP拉高唤醒，进入INIT2
    void WakeUp_nSLEEPH()
    {

    };


    void DRVOFF_HIGH()
    {

    };
    void DRVOFF_LOW()
    {

    };

};

#endif // FUNCBUTTON_H