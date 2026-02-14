#ifndef SAMPLINGSETTINGS_H
#define SAMPLINGSETTINGS_H

#include <xmlsettings.h>
#include <memory>

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
    const QList<ChannelSettings*>& getChannelSettings();
public slots:
    void configXMLInfo(const QString &key) override;
private:
    QList<ChannelSettings*> m_ChannelSettingsList;
};

typedef std::shared_ptr<SamplingSettings> SamplingSettingsPtr;

#endif // SAMPLINGSETTINGS_H
