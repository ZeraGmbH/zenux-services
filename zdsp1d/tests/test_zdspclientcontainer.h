#ifndef TEST_ZDSPCLIENTCONTAINER_H
#define TEST_ZDSPCLIENTCONTAINER_H

#include "abstractfactorydevicenodedsp.h"
#include <mocktcpnetworkfactory.h>
#include <QObject>

class test_zdspclientcontainer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void emptyReturnsEmptyList();
    void emptyReturnsNullFirst();
    void emptyReturnsNullFindDspInterruptId();
    void emptyReturnsNullFindProxyConnectionId();

    void getList();
    void getFirst();
    void checkDspInterruptIds();
    void checkDspInterruptIdsExceedMax();
    void addRemoveDeletesClient();
    void addSameClientIdIgnored();

    void findByProxyConnectionId();
    void findByDspInterruptId();

    void deleteByProxyConnectionIdNotAdded();
    void deleteByProxyConnectionId();

    void deleteByVeinPeerNotAdded();
    void deleteByVeinPeer();

    void cleanup();
private:
    AbstractFactoryDeviceNodeDspPtr m_devNodeFactory;
    VeinTcp::MockTcpNetworkFactory m_netFactory;
};

#endif // TEST_ZDSPCLIENTCONTAINER_H
