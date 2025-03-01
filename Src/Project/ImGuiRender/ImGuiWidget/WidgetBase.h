#ifndef WIDGET_BASE_H
#define WIDGET_BASE_H

#include <imgui/imgui.h>

class WidgetBase
{
public:
    WidgetBase() {}
    virtual ~WidgetBase() {}

    virtual void Render() = 0;
};

#endif // WIDGET_BASE_H