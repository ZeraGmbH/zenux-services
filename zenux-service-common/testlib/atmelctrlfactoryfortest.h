#ifndef ATMELCTRLFACTORYFORTEST_H
#define ATMELCTRLFACTORYFORTEST_H

#include "emobctrlfactory.h"

class AtmelCtrlFactoryForTest : public EmobCtrlFactory
{
public:
    static void prepareNextTestControllers(QVector<bool> responding);
private:
    static QVector<bool> m_respondingControllers;
};

#endif // ATMELCTRLFACTORYFORTEST_H
