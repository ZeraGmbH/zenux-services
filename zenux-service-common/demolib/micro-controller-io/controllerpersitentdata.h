#ifndef CONTROLLERPERSITENTDATA_H
#define CONTROLLERPERSITENTDATA_H

#include "abstractmockallservices.h"
#include <QString>
#include <QMap>
#include <memory>

class ControllerPersitentData
{
public:
    typedef QMap<int, AbstractMockAllServices::hotplugI2cBus> MuxChannelDeviceNameMap;
    struct TPersitentControllerData
    {
        bool m_permission = false;
        quint16 m_criticalStatus = 0;
        quint16 m_criticalStatusMask = 0;
        quint16 m_clampConnectMask = 0;
        QString m_FPGAVersion = "Unknown";
        QString m_serialNumber = "Unknown";
        QString m_writablePcbVersion = "Unknown";

        quint8 m_pllChannel = 0;

        quint8 m_accuStatus = 0;
        quint8 m_accuCharge = 37;

        MuxChannelDeviceNameMap m_hotpluggedDevices;
        QByteArray m_emobExchangeData = "Data";
    };
    static TPersitentControllerData &getData();
    static void injectInterruptFlags(quint16 clampConnectMask);
    static void setHotplugDevices(const MuxChannelDeviceNameMap &hotDevicesToSet);
    static bool isHotControllerAvailable(qint8 muxChannel);
    static bool isControllerAvailable(qint8 muxChannel);
    static void cleanupPersitentData();
private:
    static std::unique_ptr<TPersitentControllerData> m_persitentData;
};

#endif // CONTROLLERPERSITENTDATA_H
