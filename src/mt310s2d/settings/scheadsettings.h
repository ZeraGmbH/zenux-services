#ifndef SCHEADSETTINGS_H
#define SCHEADSETTINGS_H

#include <xmlsettings.h>
#include <xmlconfigreader.h>
#include <QString>

namespace SCHeadSystem
{
    enum configstate
    {
        cfgSH0Alias,
        cfgSH0avail,
    };
    struct cChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        bool avail; // is this channel available ?
    };
}

class cSCHeadSettings : public XMLSettings
{
    Q_OBJECT
public:
    cSCHeadSettings(Zera::XMLConfig::cReader *xmlread);
    ~cSCHeadSettings();
    QList<SCHeadSystem::cChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<SCHeadSystem::cChannelSettings*> m_ChannelSettingsList;
};



#endif // SCHEADSETTINGS_H
