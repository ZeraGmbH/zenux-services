#include "firmwareupdatenotificationfile.h"
#include <QFile>

const QString FirmwareUpdateNotificationFile::m_upgradeNotificationFileName = "/tmp/firmware-update-pending";

FirmwareUpdateNotificationFile::FirmwareUpdateNotificationFile()
{
    const QString upgradeNotificationFileName = m_upgradeNotificationFileName;
    QFile upgradeFile(upgradeNotificationFileName);
    upgradeFile.open(QFile::WriteOnly);
    upgradeFile.close();
}

FirmwareUpdateNotificationFile::~FirmwareUpdateNotificationFile()
{
    QFile::remove(m_upgradeNotificationFileName);
}
