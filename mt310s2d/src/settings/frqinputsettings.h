#ifndef FRQINPUTSETTINGS_H
#define FRQINPUTSETTINGS_H

#include <QObject>
#include <QList>
#include <QString>
#include <xmlconfigreader.h>
#include <xmlsettings.h>

namespace FRQInputSystem
{
enum configstate
{
    cfgFin0Alias,
    cfgFin1Alias,
    cfgFin2Alias,
    cfgFin3Alias,
    cfgFin0avail,
    cfgFin1avail,
    cfgFin2avail,
    cfgFin3avail,
};

struct cChannelSettings // what we want to get configured
{
    QString m_sAlias; // the names channel
    bool avail; // is this channel available ?
};
}

class cFRQInputSettings : public XMLSettings
{
    Q_OBJECT
public:
    cFRQInputSettings(Zera::XMLConfig::cReader *xmlread);
    ~cFRQInputSettings();
    QList<FRQInputSystem::cChannelSettings*>& getChannelSettings();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QList<FRQInputSystem::cChannelSettings*> m_ChannelSettingsList;
};

#endif // FRQINPUTSETTINGS_H
