#ifndef TEXT_WIDGET_H
#define TEXT_WIDGET_H

#include "WidgetBase.h"

class TextWidget : public WidgetBase
{
public:
    TextWidget();
    virtual ~TextWidget();

    virtual void Render() override;
};

#endif // TEXT_WIDGET_H