#ifndef CONTROLLERPERSITENTDATA_H
#define CONTROLLERPERSITENTDATA_H

#include <QString>

class ControllerPersitentData
{
public:
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
    };
    static TPersitentControllerData &getData();
    static void injectInterruptFlags(quint16 clampConnectMask);
    static void cleanupPersitentData();
private:
    static TPersitentControllerData m_persitentData;
};

#endif // CONTROLLERPERSITENTDATA_H
