#ifndef EXAMPLE_PAGE_H
#define EXAMPLE_PAGE_H

#include "WidgetBase.h"

class ExamplePage : public WidgetBase
{
    public:
        ExamplePage();
        virtual ~ExamplePage();

         virtual void Render() override;
};

#endif // EXAMPLE_PAGE_H