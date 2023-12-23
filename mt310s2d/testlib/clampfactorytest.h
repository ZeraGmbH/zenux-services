#ifndef CLAMPFACTORYTEST_H
#define CLAMPFACTORYTEST_H

#include <clampfactory.h>

class ClampFactoryTest : public ClampFactory
{
public:
    static void enableTest();
    static void setTestClampType(int clampTypeNo);
    static int getTestClampType();
private:
    static int m_clampForTestType;
};

#endif // CLAMPFACTORYTEST_H
