#ifndef ACCUMULATORSETTINGS_H
#define ACCUMULATORSETTINGS_H

#include <xmlsettings.h>

class accumulatorSettings : public XMLSettings
{
public:
    accumulatorSettings(Zera::XMLConfig::cReader *xmlread);
    ~accumulatorSettings();
    bool isAvailable();
public slots:
    virtual void configXMLInfo(QString key);
private:
    bool m_bAvail;
};

#endif // ACCUMULATORSETTINGS_H
