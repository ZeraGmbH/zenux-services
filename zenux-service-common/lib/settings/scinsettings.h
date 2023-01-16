#ifndef SCHEADSETTINGS_H
#define SCHEADSETTINGS_H

#include <xmlsettings.h>

class ScInSettings : public XMLSettings
{
    Q_OBJECT
public:
    struct ChannelSettings // what we want to get configured
    {
        QString m_alias; // the names channel
        bool m_avail;    // is this channel available ?
    };
    ScInSettings(Zera::XMLConfig::cReader *xmlread);
    ~ScInSettings();
    QList<ChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};



#endif // SCHEADSETTINGS_H
