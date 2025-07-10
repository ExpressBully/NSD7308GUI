#ifndef FAULTSTATE_H
#define FAULTSTATE_H

#include "WidgetBase.h"
#include <imgui.h>
#include <functional>

class FaultState : public WidgetBase
{
public:
    FaultState();
    virtual ~FaultState();

    virtual void Render() override;

    struct FaultStatus {
        int FAULT = 0;
        int VMOV = 0;
        int VMUV = 0;
        int TSD = 0;
        int SPI_ERR = 0;
        int POR = 0;
        int ITRIP_CMP = 0;
        int OLP_CMP = 0;
        int DRVOFF_STAT = 0;
        int OLA = 0;
        int OLA1 = 0;
        int OLA2 = 0;
        int OCP = 0;
        int OCP_H1 = 0;
        int OCP_H2 = 0;
        int OCP_L1 = 0;
        int OCP_L2 = 0;
    } status;


private:
    void RenderStatusBox(const char* label, int state);
    ImFont* smallFont = nullptr; // 小号字体

};

#endif // FAULTSTATE_H