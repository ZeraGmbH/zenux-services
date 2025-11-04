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
    void pushButtonNoEmob();
    void pushButtonIAUX();
    void readLockState();
    void pressButtonUl1Channel();
    void readLockStateChannelUl1();
private:
    void setupServers();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_EMOB_LOCK_H
