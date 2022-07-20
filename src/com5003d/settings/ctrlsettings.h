#ifndef CTRLSETTINGS
#define CTRLSETTINGS

#include <QObject>
#include <QString>

#include <xmlsettings.h>

namespace CTRLSettings
{
enum configstate
{
    setCTRLDevNode
};
}

class cCtrlSettings : public XMLSettings
{
    Q_OBJECT

public:
    cCtrlSettings(Zera::XMLConfig::cReader *xmlread);
    QString& getDeviceNode();

public slots:
    virtual void configXMLInfo(QString key);

private:
    QString m_sDeviceNode;
};


#endif // CTRLSETTINGS

