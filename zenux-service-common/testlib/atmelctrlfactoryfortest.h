#ifndef ATMELCTRLFACTORYFORTEST_H
#define ATMELCTRLFACTORYFORTEST_H

#include "atmelctrlfactory.h"

class AtmelCtrlFactoryForTest : public AtmelCtrlFactory
{
public:
    static void enableTest();
};

#endif // ATMELCTRLFACTORYFORTEST_H
