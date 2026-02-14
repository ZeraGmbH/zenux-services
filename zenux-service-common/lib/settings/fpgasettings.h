#ifndef FPGASETTINGS_H
#define FPGASETTINGS_H

#include <xmlsettings.h>
#include <memory>

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
    void configXMLInfo(const QString &key) override;
private:
    QString m_ctrlDeviceNode;
    QString m_msgDeviceNode;
    QString m_secDeviceNode;
    QString m_dspDeviceNode;
};

typedef std::shared_ptr<FPGASettings> FPGASettingsPtr;

#endif // FPGASETTINGS_H
