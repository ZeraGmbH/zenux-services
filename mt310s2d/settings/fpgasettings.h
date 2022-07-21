#ifndef FPGASETTINGS_H
#define FPGASETTINGS_H

#include <xmlsettings.h>
#include <xmlconfigreader.h>
#include <QString>

namespace FPGASettings
{
enum configstate
{
    setFPGADevNode
};
}

class cFPGASettings : public XMLSettings
{
    Q_OBJECT
public:
    cFPGASettings(Zera::XMLConfig::cReader *xmlread);
    QString& getDeviceNode();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QString m_sDeviceNode;
};


#endif // FPGASETTINGS_H
