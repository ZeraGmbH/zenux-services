#ifndef HKEYSETTINGS_H
#define HKEYSETTINGS_H

#include <xmlsettings.h>

class HkInSettings : public XMLSettings
{
    Q_OBJECT
public:
    struct ChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        bool avail; // is this channel available ?
    };
    HkInSettings(Zera::XMLConfig::cReader *xmlread);
    ~HkInSettings();
    QList<ChannelSettings *> &getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};

#endif // HKEYSETTINGS_H
