#ifndef EMOBCTRLFACTORYFORTEST_H
#define EMOBCTRLFACTORYFORTEST_H

#include "factoryemobctrl.h"

class EmobCtrlFactoryForTest : public FactoryEmobCtrl
{
public:
    static void prepareNextTestControllers(QVector<bool> responding);
private:
    static QVector<bool> m_respondingControllers;
};

#endif // EMOBCTRLFACTORYFORTEST_H
