#ifndef FPGASETTINGS_H
#define FPGASETTINGS_H

#include <xmlsettings.h>

class FPGASettings : public XMLSettings
{
    Q_OBJECT
public:
    FPGASettings(Zera::XMLConfig::cReader *xmlReader);
    QString getCtrlDeviceNode();
    QString getMsgDeviceNode();
    QString getSecDeviceNode();
    QString getDspDeviceNode();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QString m_ctrlDeviceNode;
    QString m_msgDeviceNode;
    QString m_secDeviceNode;
    QString m_dspDeviceNode;
};

#endif // FPGASETTINGS_H
