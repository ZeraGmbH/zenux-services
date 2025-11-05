#ifndef TEST_EMOB_LOCK_H
#define TEST_EMOB_LOCK_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_emob_lock : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void pushButtonNoParamNoEmob();
    void pushButtonNoParamEmobIAUX();
    void pushButtonNoParamEmobI3IAUX();
    void pushButtonIAUXNoEmob();
    void pushButtonIAUXEmobIAUX();
    void pushButtonI3EmobI3();
    void pushButtonI3EmobIAUX();

    void readLockStateNoParamNoEmob();
    void readLockStateNoParamEmobIAUX();
    void readLockStateNoParamEmobI3IAUX();
    void readLockStateIAUXNoEmob();
    void readLockStateIAUXEmobIAUX();
    void readLockStateI3EmobI3();
    void readLockStateI3EmobIAUX();

private:
    void setupServers();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_EMOB_LOCK_H
