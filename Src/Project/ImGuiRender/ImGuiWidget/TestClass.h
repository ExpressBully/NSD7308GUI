#ifndef  TESTCLASS_H
#define  TESTCLASS_H

#include "WidgetBase.h"

class TestClass : public WidgetBase
{
    public:
        TestClass();
        virtual ~TestClass();

        virtual void Render() override;
};


#endif // ! TESTCLASS_H
