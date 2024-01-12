#ifndef MOCKATMELCOMMONVERSIONS_H
#define MOCKATMELCOMMONVERSIONS_H

#include <atmelcommon.h>

class MockAtmelCommonVersions : public AtmelCommonVersions
{
public:
    MockAtmelCommonVersions();
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
};

#endif // MOCKATMELCOMMONVERSIONS_H
