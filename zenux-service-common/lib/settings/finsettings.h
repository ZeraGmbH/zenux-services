#ifndef FINPUTSETTINGS_H
#define FINPUTSETTINGS_H

#include <xmlsettings.h>

class FInSettings : public XMLSettings
{
    Q_OBJECT
public:
    struct ChannelSettings // what we want to get configured
    {
        QString m_sAlias; // the names channel
        bool avail; // is this channel available ?
    };
    FInSettings(Zera::XMLConfig::cReader *xmlread);
    ~FInSettings();
    QList<ChannelSettings*>& getChannelSettings();
public slots:
    void configXMLInfo(const QString &key) override;
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};

#endif // FINPUTSETTINGS_H
