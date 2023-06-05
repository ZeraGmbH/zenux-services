#ifndef ATMELCTRLFACTORYFORTEST_H
#define ATMELCTRLFACTORYFORTEST_H

#include "atmelctrlfactory.h"

class AtmelCtrlFactoryForTest : public AtmelCtrlFactory
{
public:
    static void prepareNextTestControllers(QVector<bool> responding);
private:
    static QVector<bool> m_respondingControllers;
};

#endif // ATMELCTRLFACTORYFORTEST_H
