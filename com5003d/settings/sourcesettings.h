#ifndef SOURCESETTINGS_H
#define SOURCESETTINGS_H

#include <xmlsettings.h>
#include <xmlconfigreader.h>
#include <QString>
#include <QList>

namespace SourceSystem
{
    enum configstate
    {
        cfg0Alias,
        cfg1Alias,
        cfg2Alias,
        cfg3Alias,
        cfg0dspserver,
        cfg1dspserver,
        cfg2dspserver,
        cfg3dspserver,
        cfg0dspchannel,
        cfg1dspchannel,
        cfg2dspchannel,
        cfg3dspchannel,
        cfg0avail,
        cfg1avail,
        cfg2avail,
        cfg3avail
    };
    struct cChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        quint16 m_nDspServerPort; // where to control the channel
        quint8 m_nDspChannel; // where to configure the channel
        bool avail; // is this channel available ?
    };
}

class cSourceSettings : public XMLSettings
{
    Q_OBJECT
public:
    cSourceSettings(Zera::XMLConfig::cReader *xmlread);
    ~cSourceSettings();
    QList<SourceSystem::cChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<SourceSystem::cChannelSettings*> m_ChannelSettingsList;
};

#endif // SOURCESETTINGS_H
