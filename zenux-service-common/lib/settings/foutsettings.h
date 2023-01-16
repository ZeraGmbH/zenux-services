#ifndef FOUTSETTINGS_H
#define FOUTSETTINGS_H

#include <xmlsettings.h>
#include <QString>
#include <QList>

namespace SourceSystem
{
    struct cChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        quint16 m_nDspServerPort; // where to control the channel
        quint8 m_nDspChannel; // where to configure the channel
        bool avail; // is this channel available ?
    };
}

class FOutSettings : public XMLSettings
{
    Q_OBJECT
public:
    FOutSettings(Zera::XMLConfig::cReader *xmlread);
    ~FOutSettings();
    QList<SourceSystem::cChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<SourceSystem::cChannelSettings*> m_ChannelSettingsList;
};

#endif // FOUTSETTINGS_H
