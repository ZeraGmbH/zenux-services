#ifndef SAMPLINGSETTINGS_H
#define SAMPLINGSETTINGS_H

#include <xmlsettings.h>

class SamplingSettings : public XMLSettings
{
public:
    struct ChannelSettings // what we want to get configured
    {
        bool m_bAvail; // is this channel available ?
        QStringList m_pllChannels;
    };
    SamplingSettings(Zera::XMLConfig::cReader *xmlread);
    ~SamplingSettings();
    QList<ChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};

#endif // SAMPLINGSETTINGS_H
