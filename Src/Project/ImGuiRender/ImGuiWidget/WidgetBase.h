#ifndef WIDGET_BASE_H
#define WIDGET_BASE_H

#include <imgui/imgui.h>

class WidgetBase
{
public:
    WidgetBase() {}
    virtual ~WidgetBase() {}

    virtual void Render() = 0;

    void RenderFlag(bool Flag) {
        mIsRender = Flag;
    }

protected:
    bool mIsRender = true;

};

#endif // WIDGET_BASE_H