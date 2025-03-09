#ifndef  PWMSLIDER_H
#define  PWMSLIDER_H

#include "WidgetBase.h"

class PWM_Slider : public WidgetBase
{
public:
    PWM_Slider();
    virtual ~PWM_Slider();

    virtual void Render() override;
};


#endif // ! PWMSLIDER_H

