#include "test_firmwareupdatenotificationfile.h"
#include "firmwareupdatenotificationfile.h"
#include <QTest>

QTEST_MAIN(test_firmwareupdatenotificationfile)

void test_firmwareupdatenotificationfile::testFileGeneratedAndDeleted()
{
    static const QString upgradeNotificationFileName = "/tmp/firmware-update-pending";
    QFile::remove(upgradeNotificationFileName);
    QVERIFY(!QFile::exists(upgradeNotificationFileName));

    std::unique_ptr<FirmwareUpdateNotificationFile> fileGen = std::make_unique<FirmwareUpdateNotificationFile>();
    QVERIFY(QFile::exists(upgradeNotificationFileName));

    fileGen.reset();
    QVERIFY(!QFile::exists(upgradeNotificationFileName));
}
