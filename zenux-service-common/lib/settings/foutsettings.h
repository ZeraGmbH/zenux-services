#ifndef FOUTSETTINGS_H
#define FOUTSETTINGS_H

#include <xmlsettings.h>

class FOutSettings : public XMLSettings
{
    Q_OBJECT
public:
    struct ChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        quint16 m_nDspServerPort; // where to control the channel
        quint8 m_nDspChannel; // where to configure the channel
        bool avail; // is this channel available ?
    };
    FOutSettings(Zera::XMLConfig::cReader *xmlread);
    ~FOutSettings();
    QList<ChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};

#endif // FOUTSETTINGS_H
