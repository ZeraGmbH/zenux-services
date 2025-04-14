#ifndef TEST_REGRESSION_SEC1000_H
#define TEST_REGRESSION_SEC1000_H

#include "mocksec1000d.h"
#include "secinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_sec1000 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase_data();
    void init();
    void cleanup();

    void serverUp();
    void dumpScpi();
    void interruptDeviceNodeIo();
    void interruptNotifications();
private:
    void fireInterrupt();
    QStringList claimAllEcChannels(int ecUnitCount);
    struct ServerSendData
    {
        quint32 msgnr;
        quint8 reply;
        QVariant answer;
    };
    void collectServerSends(QList<ServerSendData> &serverSendList);
    void registerInterruptNotifiers(QStringList ecalChannels);
    bool checkRegisterResponses(const QList<ServerSendData> &responses);
    void prepareInterruptMaskValue(QStringList ecalChannels);
    bool checkInterruptMasks(const QList<ServerSendData> &responses);
    quint8 calcNonZeroTestMask(int ecChannelNum);

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockSec1000d> m_server;
    Zera::ProxyClientPtr m_proxyClient;
    Zera::cSECInterfacePtr m_secIFace;
};


#endif // TEST_REGRESSION_SEC1000_H
