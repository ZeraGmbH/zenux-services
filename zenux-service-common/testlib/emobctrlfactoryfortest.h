#ifndef EMOBCTRLFACTORYFORTEST_H
#define EMOBCTRLFACTORYFORTEST_H

#include "emobctrlfactory.h"

class EmobCtrlFactoryForTest : public EmobCtrlFactory
{
public:
    static void prepareNextTestControllers(QVector<bool> responding);
private:
    static QVector<bool> m_respondingControllers;
};

#endif // EMOBCTRLFACTORYFORTEST_H
