#ifndef DSPSETTINGS_H
#define DSPSETTINGS_H

#include <xmlsettings.h>

namespace DSPSettings
{
    enum configstate
    {
        setDSPBootfile,
        setDSPBoot,
        setDSPChannelNr,
        setDSPSignalPeriod,
        setDSPMeasPeriod
    };
}

class cDSPSettings : public XMLSettings
{
    Q_OBJECT

public:
    cDSPSettings(Zera::XMLConfig::cReader *xmlread);
    QString& getBootFile();
    quint8 getChannelNr();
    bool isBoot();
    quint16 getSamplesSignalPeriod();
    quint16 getsamplesMeasurePeriod();

public slots:
    void configXMLInfo(const QString &key) override;

private:
    QString m_sDeviceNode;
    QString m_sBootFile;
    quint8 m_nBoot; // 0 = no, 1 = yes
    quint8 m_nChannels;
    quint16 m_nSamplesPerSignalPeriod;
    quint16 m_nSamplesPerMeasurePeriod;
};



#endif // DSPSETTINGS_H
