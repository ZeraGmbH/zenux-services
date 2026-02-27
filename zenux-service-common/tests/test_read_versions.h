#ifndef TEST_READ_VERSIONS_H
#define TEST_READ_VERSIONS_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_read_versions : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void readPcbVersionNoEmob();
    void readPcbVersionOneEmobChannelIAUX();
    void readPcbVersionOneEmobAddAndRemoveIAUX();
    void readPcbVersionTwoEmobTwoChannels();
    void readPcbVersionAccuNoEmob();

    void readCtrlVersionNoEmob();
    void readCtrlVersionOneEmobChannelIAUX();
    void readCtrlVersionOneEmobAddRemoveIAUX();
    void readCtrlVersionTwoEmobTwoChannels();
    void readCtrlVersionAccuNoEmob();

private:
    void setupServers();
    void registerNotifications();
    void unregisterNotifications();
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer); // is a slot
    void waitForAccuDetected();

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    QList<int> m_notificationsReceivedCtrl;
    QList<int> m_notificationsReceivedPcb;
};

#endif // TEST_READ_VERSIONS_H
