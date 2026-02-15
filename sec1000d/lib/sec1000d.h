#ifndef SEC1000D_H
#define SEC1000D_H

#include "pcbserver.h"
#include "settingscontainer.h"
#include "abstractfactorydevicenodesec.h"
#include <QTimer>

class QStateMachine;
class QState;
class QSocketNotifier;
class SecCalculatorSettings;
class SecInputSettings;
class Sec1000StatusInterface;
class cSystemInterface;
class SecMainAndChannelInterface;
class SecChannel;
class Sec1000SystemInfo;


class cSEC1000dServer : public PCBServer
{
    Q_OBJECT
public:
    static const int Com5003EcUnitCount;
    static const int Mtxxxs2EcUnitCount;
    explicit cSEC1000dServer(SettingsContainerPtr settings,
                             int ecUnitCount,
                             AbstractFactoryDeviceNodeSecPtr deviceNodeFactory,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ~cSEC1000dServer();
    QString getServerVersion();
    QString getSecDeviceNode();
    int getEcUnitsAvailable() const;
    int getEcUnitsOccupied() const;
    static const ServerParams defaultParams;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void sigServerIsSetUp();

private slots:
    void SECIntHandler(int);
    void doSetupServer();
    void doCloseServer();
    void onProtobufDisconnect(VeinTcp::TcpPeer *peer) override;
private:
    void doConfiguration(int ecUnitCount);
    void init();
    void earlySetup();
    AbstractFactoryDeviceNodeSecPtr m_deviceNodeFactory;
    SecCalculatorSettings* m_pECalcSettings = nullptr;
    SecInputSettings* m_pInputSettings = nullptr;

    Sec1000StatusInterface* m_pStatusInterface = nullptr;
    cSystemInterface* m_pSystemInterface = nullptr;
    SecMainAndChannelInterface* m_pECalculatorInterface = nullptr;
    Sec1000SystemInfo* m_pSystemInfo = nullptr;

    QStateMachine* m_pInitializationMachine = nullptr;
    QList<SecChannel*> m_ECalculatorChannelList;
    QSocketNotifier* m_pNotifier = nullptr;
};


#endif
