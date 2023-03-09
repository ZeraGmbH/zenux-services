#ifndef ACCUMULATORSETTINGS_H
#define ACCUMULATORSETTINGS_H

#include <xmlsettings.h>

class accumulatorSettings : public XMLSettings
{
public:
    struct BatterySettings
    {
        bool m_bAvail;
    };
    accumulatorSettings(Zera::XMLConfig::cReader *xmlread);
    ~accumulatorSettings();
    QList<BatterySettings*>& getBatterySettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<BatterySettings*> m_BatterySettingsList;
};

#endif // ACCUMULATORSETTINGS_H
