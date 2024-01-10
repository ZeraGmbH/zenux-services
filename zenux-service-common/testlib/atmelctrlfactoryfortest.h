#ifndef ATMELCTRLFACTORYFORTEST_H
#define ATMELCTRLFACTORYFORTEST_H

#include "atmelctrlfactorystatic.h"

class AtmelCtrlFactoryForTest : public AtmelCtrlFactoryStatic
{
public:
    static void prepareNextTestControllers(QVector<bool> responding);
private:
    static QVector<bool> m_respondingControllers;
};

#endif // ATMELCTRLFACTORYFORTEST_H
