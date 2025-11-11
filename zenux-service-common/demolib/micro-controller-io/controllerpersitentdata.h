#ifndef CONTROLLERPERSITENTDATA_H
#define CONTROLLERPERSITENTDATA_H

#include <QString>
#include <QMap>

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

        quint8 m_accuStatus = 0;
        quint8 m_accuCharge = 37;

        QMap<int /* muxChannel */, QString> m_emobInstrumentSubtypes;
    };
    static TPersitentControllerData &getData();
    static void injectInterruptFlags(quint16 clampConnectMask);
    static void addInstrumentSubtype(int muxChannel, const QString& instrSubtype);

    static void cleanupPersitentData();
private:
    static TPersitentControllerData m_persitentData;
};

#endif // CONTROLLERPERSITENTDATA_H
