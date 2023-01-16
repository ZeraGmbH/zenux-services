#ifndef FPGASETTINGS_H
#define FPGASETTINGS_H

#include <xmlsettings.h>

class FPGASettings : public XMLSettings
{
    Q_OBJECT
public:
    FPGASettings(Zera::XMLConfig::cReader *xmlread, QString topXmlNode);
    QString& getDeviceNode();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QString m_sDeviceNode;
};

#endif // FPGASETTINGS_H
