#ifndef  UARTDEC_H
#define  UARTDEC_H

#include "WidgetBase.h"
#include <vector>
#include <string>
#include <mutex>

class UART_DEC : public WidgetBase
{
public:
    UART_DEC();
    virtual ~UART_DEC();

    virtual void Render() override;
};


#endif // ! UARTDEC_H