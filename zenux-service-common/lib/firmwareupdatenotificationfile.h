#ifndef FIRMWAREUPDATENOTIFICATIONFILE_H
#define FIRMWAREUPDATENOTIFICATIONFILE_H

#include <QString>

class FirmwareUpdateNotificationFile
{
public:
    FirmwareUpdateNotificationFile();
    virtual ~FirmwareUpdateNotificationFile();
private:
    static const QString m_upgradeNotificationFileName;
};

#endif // FIRMWAREUPDATENOTIFICATIONFILE_H
