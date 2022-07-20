#ifndef SENSESETTINGS_H
#define SENSESETTINGS_H

#include <xmlsettings.h>

namespace SenseSystem
{

enum configstate
{
    cfg0Alias1,
    cfg1Alias1,
    cfg2Alias1,
    cfg3Alias1,
    cfg4Alias1,
    cfg5Alias1,

    cfg0Alias2,
    cfg1Alias2,
    cfg2Alias2,
    cfg3Alias2,
    cfg4Alias2,
    cfg5Alias2,

    cfg0ctrlchannel,
    cfg1ctrlchannel,
    cfg2ctrlchannel,
    cfg3ctrlchannel,
    cfg4ctrlchannel,
    cfg5ctrlchannel,

    cfg0dspchannel,
    cfg1dspchannel,
    cfg2dspchannel,
    cfg3dspchannel,
    cfg4dspchannel,
    cfg5dspchannel,

    cfg0overloadbit,
    cfg1overloadbit,
    cfg2overloadbit,
    cfg3overloadbit,
    cfg4overloadbit,
    cfg5overloadbit,

    cfg0avail,
    cfg1avail,
    cfg2avail,
    cfg3avail,
    cfg4avail,
    cfg5avail,

};

struct cChannelSettings // what we want to get configured
{
    QString m_sAlias[2]; // the alias names for measuring channels
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    qint8 m_nOverloadBit; // where to find overload condition in critical status
    bool avail; // is this channel available ?
};
}

class cSenseSettings : public XMLSettings
{
    Q_OBJECT
public:
    cSenseSettings(Zera::XMLConfig::cReader *xmlread);
    virtual ~cSenseSettings();
    QList<SenseSystem::cChannelSettings*>& getChannelSettings();

public slots:
    virtual void configXMLInfo(QString key);

private:
    QList<SenseSystem::cChannelSettings*> m_ChannelSettingsList;
};


#endif // SENSESETTINGS_H
