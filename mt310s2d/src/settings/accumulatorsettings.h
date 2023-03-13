#ifndef ACCUMULATORSETTINGS_H
#define ACCUMULATORSETTINGS_H

#include <xmlsettings.h>

class AccumulatorSettings : public XMLSettings
{
public:
    AccumulatorSettings(Zera::XMLConfig::cReader *xmlread);
    ~AccumulatorSettings();
    bool isAvailable();
public slots:
    virtual void configXMLInfo(QString key);
private:
    bool m_bAvail;
};

#endif // ACCUMULATORSETTINGS_H
