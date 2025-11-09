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
    void init();
    void cleanup();
    void readPcbVersionNoEmob();
    void readPcbVersionOneEmobChannelIAUX();
    void readPcbVersionOneEmobAddAndRemoveIAUX();
    void readPcbVersionTwoEmobTwoChannels();

    void readCtrlVersionNoEmob();
    void readCtrlVersionOneEmobChannelIAUX();
    void readCtrlVersionOneEmobAddRemoveIAUX();
    void readCtrlVersionTwoEmobTwoChannels();

private:
    void setupServers();
    void registerNotifications();
    void unregisterNotifications();
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer); // is a slot

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    QList<int> m_notificationsReceivedCtrl;
    QList<int> m_notificationsReceivedPcb;
};

#endif // TEST_READ_VERSIONS_H
